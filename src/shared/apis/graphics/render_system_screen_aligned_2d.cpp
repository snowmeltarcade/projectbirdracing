#include "render_system_screen_aligned_2d.h"

#include <vk_mem_alloc.h>
#include <filesystem>
#include <fstream>

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics {
    render_system_screen_aligned_2d::render_system_screen_aligned_2d(const vulkan::device& device,
                                                                     const vulkan::vma& vma,
                                                                     const vulkan::render_pass& render_pass,
                                                                     const vulkan::command_pool& command_pool,
                                                                     const vulkan::queue& graphics_queue,
                                                                     VkExtent2D swap_chain_extent,
                                                                     uint32_t number_of_swap_chain_images,
                                                                     VkSampleCountFlagBits msaa_samples,
                                                                     std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
            _vma(vma),
            _log_manager(log_manager) {
        // descriptor set layout
        VkDescriptorSetLayoutBinding ubo_layout_binding {};
        ubo_layout_binding.binding = 0;
        ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_binding.descriptorCount = 1;
        ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_layout_binding.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 1> bindings { ubo_layout_binding };

        VkDescriptorSetLayoutCreateInfo layout_info {};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        layout_info.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(this->_device.get_native_handle(),
                                        &layout_info,
                                        nullptr,
                                        &this->_descriptor_set_layout) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create descriptor set layout.")
        }

        // ubo buffers
        auto ubo_size = sizeof(ubo);

        this->_ubo_buffers.reserve(number_of_swap_chain_images);
        for (auto i {0u}; i < number_of_swap_chain_images; ++i) {
            this->_ubo_buffers.emplace_back(this->_vma,
                                            ubo_size,
                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VMA_MEMORY_USAGE_CPU_TO_GPU,
                                            this->_log_manager);
        }

        // descriptor pool
        std::array<VkDescriptorPoolSize, 1> pool_sizes {};

        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = number_of_swap_chain_images;

        VkDescriptorPoolCreateInfo pool_info {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes = pool_sizes.data();
        pool_info.maxSets = number_of_swap_chain_images;

        if (vkCreateDescriptorPool(this->_device.get_native_handle(),
                                   &pool_info,
                                   nullptr,
                                   &this->_descriptor_pool) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create descriptor pool.")
        }

        // descriptor sets
        std::vector<VkDescriptorSetLayout> layouts(number_of_swap_chain_images, this->_descriptor_set_layout);

        VkDescriptorSetAllocateInfo descriptor_set_allocate_info {};
        descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptor_set_allocate_info.descriptorPool = this->_descriptor_pool;
        descriptor_set_allocate_info.descriptorSetCount = number_of_swap_chain_images;
        descriptor_set_allocate_info.pSetLayouts = layouts.data();

        this->_descriptor_sets.resize(number_of_swap_chain_images);
        if (vkAllocateDescriptorSets(this->_device.get_native_handle(),
                                     &descriptor_set_allocate_info,
                                     this->_descriptor_sets.data()) != VK_SUCCESS) {
            FATAL_ERROR("Failed to allocate descriptor sets.")
        }

        for (auto i {0u}; i < number_of_swap_chain_images; ++i) {
            VkDescriptorBufferInfo buffer_info {};
            buffer_info.buffer = this->_ubo_buffers[i].get_native_handle();
            buffer_info.offset = 0;
            buffer_info.range = sizeof(ubo);

            std::array<VkWriteDescriptorSet, 1> descriptor_writes {};

            descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[0].dstSet = this->_descriptor_sets[i];
            descriptor_writes[0].dstBinding = 0;
            descriptor_writes[0].dstArrayElement = 0;
            descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_writes[0].descriptorCount = 1;
            descriptor_writes[0].pBufferInfo = &buffer_info;

            vkUpdateDescriptorSets(this->_device.get_native_handle(),
                                   static_cast<uint32_t>(descriptor_writes.size()),
                                   descriptor_writes.data(),
                                   0,
                                   nullptr);
        }

        this->create_pipeline(swap_chain_extent, msaa_samples, render_pass);

        std::vector<vertex> vertices {
            { { 0.0, -0.5, } },
            { { 0.5, 0.5, } },
            { { -0.5, 0.5, } },
            { { 0.1, -0.6, } },
            { { 0.6, 0.6, } },
            { { -0.6, 0.6, } },
        };

        this->create_vertex_buffer(vertices, command_pool, graphics_queue);
    }

    render_system_screen_aligned_2d::~render_system_screen_aligned_2d() {
        this->_vertex_buffer.reset();

        if (this->_graphics_pipeline) {
            vkDestroyPipeline(this->_device.get_native_handle(),
                              this->_graphics_pipeline,
                              nullptr);
            this->_graphics_pipeline = nullptr;
        }

        if (this->_pipeline_layout) {
            vkDestroyPipelineLayout(this->_device.get_native_handle(),
                                    this->_pipeline_layout,
                                    nullptr);
            this->_pipeline_layout = nullptr;
        }

        if (this->_descriptor_set_layout) {
            vkDestroyDescriptorSetLayout(this->_device.get_native_handle(),
                                         this->_descriptor_set_layout,
                                         nullptr);
            this->_descriptor_set_layout = nullptr;
        }

        if (this->_descriptor_pool) {
            vkDestroyDescriptorPool(this->_device.get_native_handle(),
                                    this->_descriptor_pool,
                                    nullptr);
            this->_descriptor_pool = nullptr;
        }
    }

    void render_system_screen_aligned_2d::build_render_commands(vulkan::command_buffer& buffer, uint32_t image_index) {
        ubo ubo{};
        ubo.color = {1,0,0,0.7};

        void* data;
        vmaMapMemory(this->_vma.get_native_handle(),
                     this->_ubo_buffers[image_index].get_native_allocation_handle(),
                     &data);
        memcpy(data, &ubo, sizeof(ubo));
        vmaUnmapMemory(this->_vma.get_native_handle(),
                       this->_ubo_buffers[image_index].get_native_allocation_handle());

        vkCmdBindPipeline(buffer.get_native_handle(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          this->_graphics_pipeline);

        std::array<VkBuffer, 1> vertex_buffers {this->_vertex_buffer->get_native_handle()};
        VkDeviceSize offsets[] {0};

        vkCmdBindVertexBuffers(buffer.get_native_handle(),
                               0,
                               1,
                               &vertex_buffers[0],
                               offsets);

        vkCmdBindDescriptorSets(buffer.get_native_handle(),
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                this->_pipeline_layout,
                                0,
                                1,
                                &this->_descriptor_sets[image_index],
                                0,
                                nullptr);

        vkCmdDraw(buffer.get_native_handle(), 6, 1, 0, 0);
    }

    std::vector<char> read_all_bytes(const std::filesystem::path& path) {
        std::vector<char> bytes;

        std::ifstream fs(path, std::ios::ate | std::ios::binary);
        if (!fs.is_open()) {
            throw std::runtime_error(std::string("Failed to open file: ") + path.c_str());
        }

        auto file_size = fs.tellg();
        bytes.resize(file_size);

        fs.seekg(0);
        fs.read(bytes.data(), file_size);

        fs.close();

        return bytes;
    }

    VkShaderModule create_shader_module(VkDevice device, const std::vector<char>& bytes) {
        VkShaderModuleCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = bytes.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(bytes.data());

        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module.");
        }

        return shader_module;
    }

    void render_system_screen_aligned_2d::create_pipeline(VkExtent2D swap_chain_extent,
                                                          VkSampleCountFlagBits msaa_samples,
                                                          const vulkan::render_pass& render_pass) {
        auto vertex_shader_bytes = read_all_bytes("../../../data/vertex.vert.spv");
        auto fragment_shader_bytes = read_all_bytes("../../../data/fragment.frag.spv");

        auto vertex_shader = create_shader_module(this->_device.get_native_handle(), vertex_shader_bytes);
        auto fragment_shader = create_shader_module(this->_device.get_native_handle(), fragment_shader_bytes);

        VkPipelineShaderStageCreateInfo vertex_shader_stage_info {};
        vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_shader_stage_info.module = vertex_shader;
        vertex_shader_stage_info.pName = "main";
        vertex_shader_stage_info.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo fragment_shader_stage_info {};
        fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragment_shader_stage_info.module = fragment_shader;
        fragment_shader_stage_info.pName = "main";
        fragment_shader_stage_info.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo shader_stages[] {
            vertex_shader_stage_info,
            fragment_shader_stage_info,
        };

        VkVertexInputBindingDescription binding_description {};
        binding_description.binding = 0;
        binding_description.stride = sizeof(vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::array<VkVertexInputAttributeDescription, 1> attribute_descriptions {};

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(vertex, pos);

        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info {};
        vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
        vertex_input_state_create_info.pVertexBindingDescriptions = &binding_description;
        vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
        vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions.data();
//        vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
//        vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;
//        vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
//        vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info {};
        input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swap_chain_extent.width;
        viewport.height = (float)swap_chain_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor {};
        scissor.offset = {0, 0};
        scissor.extent = swap_chain_extent;

        VkPipelineViewportStateCreateInfo viewport_state {};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE; //VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = msaa_samples;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;
        // aliases pixels within a polygon - will impact performance, so enable on higher graphics settings
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.minSampleShading = 0.2f; // closer to 1.0f, the smoother

        VkPipelineColorBlendAttachmentState color_blend_attachment {};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo color_blending {};
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f;
        color_blending.blendConstants[1] = 0.0f;
        color_blending.blendConstants[2] = 0.0f;
        color_blending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipeline_layout_info {};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1;
        pipeline_layout_info.pSetLayouts = &this->_descriptor_set_layout;
        pipeline_layout_info.pushConstantRangeCount = 0;
        pipeline_layout_info.pPushConstantRanges = nullptr;
//        pipeline_layout_info.setLayoutCount = 0;
//        pipeline_layout_info.pSetLayouts = nullptr;
//        pipeline_layout_info.pushConstantRangeCount = 0;
//        pipeline_layout_info.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(this->_device.get_native_handle(),
                                   &pipeline_layout_info,
                                   nullptr,
                                   &this->_pipeline_layout) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create pipeline layout.")
        }

        VkPipelineDepthStencilStateCreateInfo depth_stencil {};
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_TRUE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.minDepthBounds = 0.0f;
        depth_stencil.maxDepthBounds = 1.0f;
        depth_stencil.stencilTestEnable = VK_FALSE;
        depth_stencil.front = {};
        depth_stencil.back = {};

        VkGraphicsPipelineCreateInfo pipeline_create_info {};
        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.stageCount = 2;
        pipeline_create_info.pStages = shader_stages;
        pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
        pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
        pipeline_create_info.pViewportState = &viewport_state;
        pipeline_create_info.pRasterizationState = &rasterizer;
        pipeline_create_info.pMultisampleState = &multisampling;
        pipeline_create_info.pDepthStencilState = &depth_stencil;
        pipeline_create_info.pColorBlendState = &color_blending;
        pipeline_create_info.pDynamicState = nullptr;
        pipeline_create_info.layout = this->_pipeline_layout;
        pipeline_create_info.renderPass = render_pass.get_native_handle();
        pipeline_create_info.subpass = 0;
        pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_create_info.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(this->_device.get_native_handle(),
                                      VK_NULL_HANDLE,
                                      1,
                                      &pipeline_create_info,
                                      nullptr,
                                      &this->_graphics_pipeline) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create graphics pipeline.")
        }

        vkDestroyShaderModule(this->_device.get_native_handle(), vertex_shader, nullptr);
        vkDestroyShaderModule(this->_device.get_native_handle(), fragment_shader, nullptr);
    }

    void render_system_screen_aligned_2d::create_vertex_buffer(const std::vector<vertex>& vertices,
                                                               const vulkan::command_pool& command_pool,
                                                               const vulkan::queue& graphics_queue) {
        VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

        // copy the vertex data from a CPU/GPU shared buffer to a GPU only high performance buffer
        // the shared buffer is technically fine to use, but not as performant
        vulkan::buffer staging_buffer(this->_vma,
                                      buffer_size,
                                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VMA_MEMORY_USAGE_CPU_TO_GPU,
                                      this->_log_manager);

        void* data;
        vmaMapMemory(this->_vma.get_native_handle(),
                     staging_buffer.get_native_allocation_handle(),
                     &data);
        memcpy(data, vertices.data(), (size_t)buffer_size);
        vmaUnmapMemory(this->_vma.get_native_handle(),
                       staging_buffer.get_native_allocation_handle());

        this->_vertex_buffer = std::make_unique<vulkan::buffer>(this->_vma,
                                                                buffer_size,
                                                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                VMA_MEMORY_USAGE_GPU_ONLY,
                                                                this->_log_manager);

        vulkan::command_buffer copy_commands(this->_device,
                                             command_pool,
                                             this->_log_manager);

        if (!copy_commands.begin_one_time_usage()) {
            this->_log_manager->log_message("Failed to create copy commands.",
                                            logging::log_levels::error,
                                            "Vulkan");
        }

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = buffer_size;
        vkCmdCopyBuffer(copy_commands.get_native_handle(),
                        staging_buffer.get_native_handle(),
                        this->_vertex_buffer->get_native_handle(),
                        1,
                        &copyRegion);

        if (!copy_commands.end_one_time_usage(graphics_queue)) {
            this->_log_manager->log_message("Failed to end copy commands.",
                                            logging::log_levels::error,
                                            "Vulkan");
        }
    }
}
