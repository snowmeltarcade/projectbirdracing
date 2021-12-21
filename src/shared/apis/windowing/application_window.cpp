#include "application_window.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <algorithm>
#include <SDL_vulkan.h>
#include <SDL_image.h>
#include <iostream>
#include <optional>
#include <set>
#include <fstream>
#include <array>
#include <chrono>
#include <unordered_map>

namespace pbr::shared::apis::windowing {
    const int MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<Vertex> g_vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
    };

    const std::vector<uint32_t> g_indices {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
    };

    struct uniform_buffer_object {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
        VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        // see: https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Validation_layers
        // under the heading 'Message Callback'

        auto logger = *static_cast<std::shared_ptr<apis::logging::ilog_manager>*>(pUserData);
        logger->log_message(pCallbackData->pMessage, apis::logging::log_levels::error);
        //std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    struct queue_family_indexes {
        std::optional<int> graphics_family_index;
        std::optional<int> present_family_index;
    };

    queue_family_indexes find_queue_families(const VkPhysicalDevice device, VkSurfaceKHR surface) {
        queue_family_indexes indexes;

        auto queue_family_count {0u};
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families;
        queue_families.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

        for (auto i {0u}; i < queue_families.size(); ++i) {
            VkBool32 surface_supported {false};
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surface_supported);

            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indexes.graphics_family_index = i;
            }

            if (surface_supported) {
                indexes.present_family_index = i;
            }
        }

        return indexes;
    }

    struct swap_chain_support_details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> surface_formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    swap_chain_support_details query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface) {
        swap_chain_support_details details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        auto format_count {0u};
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

        details.surface_formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.surface_formats.data());

        auto present_modes_count {0u};
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_modes_count, nullptr);

        details.present_modes.resize(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_modes_count, details.present_modes.data());

        return details;
    }

    VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
        for (const auto& format : formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }

        return formats[0];
    }

    VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& modes) {
        for (const auto& mode : modes) {
            // triple buffering
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return mode;
            }
        }

        // this will always be available
        // is better for mobile devices (its basically vsync mode)
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D select_swap_chain_extent(const VkSurfaceCapabilitiesKHR& capabilities, SDL_Window* window) {
        // use the size Vulkan tells us to use
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            // looks like we can choose
            int width;
            int height;
            SDL_Vulkan_GetDrawableSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    bool is_physical_device_compatible(const VkPhysicalDevice device, VkSurfaceKHR surface) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        auto extensions_count {0u};
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, nullptr);

        // make sure the device supports the extensions we require
        std::vector<VkExtensionProperties> extensions;
        extensions.resize(extensions_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, extensions.data());

        std::set<std::string> required_extensions {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        // any extensions that are left in this set are not supported
        for (const auto& extension : extensions) {
            required_extensions.erase(std::string(extension.extensionName));
        }

        if (!required_extensions.empty()) {
            return false;
        }

        auto qfi = find_queue_families(device, surface);
        if (!qfi.graphics_family_index || !qfi.present_family_index) {
            return false;
        }

        auto swap_chain_details = query_swap_chain_support(device, surface);
        if (swap_chain_details.surface_formats.empty() || swap_chain_details.present_modes.empty()) {
            return false;
        }

        if (!features.samplerAnisotropy) {
            return false;
        }

        return true;
    }

    std::vector<char> read_all_bytes(const std::filesystem::path& path) {
        std::vector<char> bytes;

        std::ifstream fs(path, std::ios::ate | std::ios::binary);
        if (!fs.is_open()) {
            std::cerr << "Failed to open file: " << path << '\n';
            return {};
        }

        auto file_size = fs.tellg();
        bytes.resize(file_size);

        fs.seekg(0);
        fs.read(bytes.data(), file_size);

        fs.close();

        return bytes;
    }

    VkShaderModule create_shader_module(VkDevice device, const std::vector<char>& bytes) {
        VkShaderModuleCreateInfo create_info;
        memset(&create_info, 0, sizeof(create_info));
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = bytes.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(bytes.data());

        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
            std::cerr << "Failed to create shader module.\n";
            return VK_NULL_HANDLE;
        }

        return shader_module;
    }

    bool create_descriptor_set_layout(VkDevice device, VkDescriptorSetLayout* descriptor_set_layout) {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding2{};
        samplerLayoutBinding2.binding = 2;
        samplerLayoutBinding2.descriptorCount = 1;
        samplerLayoutBinding2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding2.pImmutableSamplers = nullptr;
        samplerLayoutBinding2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 3> bindings {uboLayoutBinding, samplerLayoutBinding, samplerLayoutBinding2};

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, descriptor_set_layout) != VK_SUCCESS) {
            std::cerr << "Failed to create descriptor set layout.\n";
            return false;
        }

        return true;
    }

    bool application_window::create_graphics_pipeline(VkDevice device,
                                                      VkExtent2D swap_chain_extent,
                                                      VkPipelineLayout* pipeline_layout,
                                                      VkRenderPass render_pass,
                                                      VkPipeline* graphics_pipeline,
                                                      VkDescriptorSetLayout* descriptor_set_layout) {
        auto vertex_shader_bytes = read_all_bytes("../../../data/vertex.vert.spv");
        auto fragment_shader_bytes = read_all_bytes("../../../data/fragment.frag.spv");

        auto vertex_shader = create_shader_module(device, vertex_shader_bytes);
        auto fragment_shader = create_shader_module(device, fragment_shader_bytes);

        VkPipelineShaderStageCreateInfo vertex_shader_stage_info;
        memset(&vertex_shader_stage_info, 0, sizeof(vertex_shader_stage_info));
        vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_shader_stage_info.module = vertex_shader;
        vertex_shader_stage_info.pName = "main";
        vertex_shader_stage_info.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo fragment_shader_stage_info;
        memset(&fragment_shader_stage_info, 0, sizeof(fragment_shader_stage_info));
        fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragment_shader_stage_info.module = fragment_shader;
        fragment_shader_stage_info.pName = "main";
        fragment_shader_stage_info.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo shader_stages[] = {
            vertex_shader_stage_info,
            fragment_shader_stage_info,
        };

        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
        memset(&vertex_input_state_create_info, 0, sizeof(vertex_input_state_create_info));
        vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto binding_description = Vertex::get_binding_description();
        auto attribute_descriptions = Vertex::get_attribute_descriptions();

        vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
        vertex_input_state_create_info.pVertexBindingDescriptions = &binding_description;
        vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
        vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info;
        memset(&input_assembly_state_create_info, 0, sizeof(input_assembly_state_create_info));
        input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport;
        memset(&viewport, 0, sizeof(viewport));
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swap_chain_extent.width;
        viewport.height = (float)swap_chain_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        memset(&scissor, 0, sizeof(scissor));
        scissor.offset = {0, 0};
        scissor.extent = swap_chain_extent;

        VkPipelineViewportStateCreateInfo viewport_state;
        memset(&viewport_state, 0, sizeof(viewport_state));
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer;
        memset(&rasterizer, 0, sizeof(rasterizer));
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling;
        memset(&multisampling, 0, sizeof(multisampling));
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = this->_msaa_samples;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;
        // aliases pixels within a polygon - will impact performance, so enable on higher graphics settings
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.minSampleShading = 0.2f; // closer to 1.0f, the smoother

        VkPipelineColorBlendAttachmentState color_blend_attachment;
        memset(&color_blend_attachment, 0, sizeof(color_blend_attachment));
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo color_blending;
        memset(&color_blending, 0, sizeof(color_blending));
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f;
        color_blending.blendConstants[1] = 0.0f;
        color_blending.blendConstants[2] = 0.0f;
        color_blending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipeline_layout_info;
        memset(&pipeline_layout_info, 0, sizeof(pipeline_layout_info));
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1;
        pipeline_layout_info.pSetLayouts = descriptor_set_layout;
        pipeline_layout_info.pushConstantRangeCount = 0;
        pipeline_layout_info.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, pipeline_layout) != VK_SUCCESS) {
            std::cerr << "Failed to create pipeline layout.\n";
            return false;
        }

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};

        VkGraphicsPipelineCreateInfo pipeline_create_info;
        memset(&pipeline_create_info, 0, sizeof(pipeline_create_info));
        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.stageCount = 2;
        pipeline_create_info.pStages = shader_stages;
        pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
        pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
        pipeline_create_info.pViewportState = &viewport_state;
        pipeline_create_info.pRasterizationState = &rasterizer;
        pipeline_create_info.pMultisampleState = &multisampling;
        pipeline_create_info.pDepthStencilState = &depthStencil;
        pipeline_create_info.pColorBlendState = &color_blending;
        pipeline_create_info.pDynamicState = nullptr;
        pipeline_create_info.layout = *pipeline_layout;
        pipeline_create_info.renderPass = render_pass;
        pipeline_create_info.subpass = 0;
        pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_create_info.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, graphics_pipeline) != VK_SUCCESS) {
            std::cerr << "Failed to create graphics pipeline.\n";
            return false;
        }

        vkDestroyShaderModule(device, vertex_shader, nullptr);
        vkDestroyShaderModule(device, fragment_shader, nullptr);

        return true;
    }

    bool application_window::create_render_pass(VkFormat swap_chain_image_format, VkDevice device, VkRenderPass* render_pass) {
        VkAttachmentDescription sample_attachment;
        memset(&sample_attachment, 0, sizeof(sample_attachment));
        sample_attachment.format = swap_chain_image_format;
        sample_attachment.samples = this->_msaa_samples;
        sample_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        sample_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        sample_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        sample_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        sample_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        sample_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference sample_attachment_ref;
        memset(&sample_attachment_ref, 0, sizeof(sample_attachment_ref));
        sample_attachment_ref.attachment = 0;
        sample_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = this->findDepthFormat();
        depthAttachment.samples = this->_msaa_samples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = this->_swap_chain_format;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass;
        memset(&subpass, 0, sizeof(subpass));
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &sample_attachment_ref;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        VkSubpassDependency dependency;
        memset(&dependency, 0, sizeof(dependency));
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachments {sample_attachment, depthAttachment, colorAttachmentResolve};

        VkRenderPassCreateInfo render_pass_create_info;
        memset(&render_pass_create_info, 0, sizeof(render_pass_create_info));
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        render_pass_create_info.pAttachments = attachments.data();
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &dependency;

        if (vkCreateRenderPass(device, &render_pass_create_info, nullptr, render_pass) != VK_SUCCESS) {
            std::cerr << "Failed to create render pass.\n";
            return false;
        }

        return true;
    }

    bool application_window::create_swap_chain_framebuffers(std::vector<VkFramebuffer>& swap_chain_framebuffers,
                                        const std::vector<VkImageView>& swap_chain_image_views,
                                        VkRenderPass render_pass, VkExtent2D swap_chain_extent,
                                        VkDevice device) {
        swap_chain_framebuffers.resize(swap_chain_image_views.size());

        for (auto i = 0u; i < swap_chain_image_views.size(); ++i) {
            std::array<VkImageView, 3> attachments {
                this->_samples_image_view,
                this->_depth_image_view,
                swap_chain_image_views[i],
            };

            VkFramebufferCreateInfo frame_buffer_info;
            memset(&frame_buffer_info, 0, sizeof(frame_buffer_info));
            frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frame_buffer_info.renderPass = render_pass;
            frame_buffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
            frame_buffer_info.pAttachments = attachments.data();
            frame_buffer_info.width = swap_chain_extent.width;
            frame_buffer_info.height = swap_chain_extent.height;
            frame_buffer_info.layers = 1;

            if (vkCreateFramebuffer(device, &frame_buffer_info, nullptr, &swap_chain_framebuffers[i]) != VK_SUCCESS) {
                std::cerr << "Failed to create frame buffer: " << i << ".\n";
                return false;
            }
        }

        return true;
    }

    void application_window::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage properties, VkBuffer* buffer, VmaAllocation* allocation) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo {};
        allocInfo.usage = properties;

        vmaCreateBuffer(this->_vma_allocator, &bufferInfo, &allocInfo, buffer, allocation, nullptr);
    }

    void application_window::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        auto commandBuffer = this->beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        this->endSingleTimeCommands(commandBuffer);
    }

    bool hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void application_window::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mip_levels) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mip_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        endSingleTimeCommands(commandBuffer);
    }

    void application_window::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        endSingleTimeCommands(commandBuffer);
    }

    bool application_window::create_vertex_buffer(VkBuffer* vertex_buffer,
                                                  VmaAllocation* vertex_buffer_allocation,
                                                  const std::vector<Vertex>& vertices) {
        VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

        // copy the vertex data from a CPU/GPU shared buffer to a GPU only high performance buffer
        // the shared buffer is technically fine to use, but not as performant
        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferAllocation;
        this->create_buffer(buffer_size,
                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VMA_MEMORY_USAGE_CPU_TO_GPU,
                            &stagingBuffer,
                            &stagingBufferAllocation);

        void* data;
        vmaMapMemory(this->_vma_allocator, stagingBufferAllocation, &data);
        memcpy(data, vertices.data(), (size_t)buffer_size);
        vmaUnmapMemory(this->_vma_allocator, stagingBufferAllocation);

        create_buffer(buffer_size,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VMA_MEMORY_USAGE_GPU_ONLY,
                      vertex_buffer,
                      vertex_buffer_allocation);

        this->copy_buffer(stagingBuffer, *vertex_buffer, buffer_size);

        vmaDestroyBuffer(this->_vma_allocator, stagingBuffer, stagingBufferAllocation);

        return true;
    }

    bool application_window::create_index_buffer(VkBuffer* index_buffer,
                                                 VmaAllocation* index_buffer_allocation,
                                                 const std::vector<uint32_t>& indices) {
        VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();

        // copy the vertex data from a CPU/GPU shared buffer to a GPU only high performance buffer
        // the shared buffer is technically fine to use, but not as performant
        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferAllocation;
        this->create_buffer(buffer_size,
                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VMA_MEMORY_USAGE_CPU_TO_GPU,
                            &stagingBuffer,
                            &stagingBufferAllocation);

        void* data;
        vmaMapMemory(this->_vma_allocator, stagingBufferAllocation, &data);
        memcpy(data, indices.data(), (size_t) buffer_size);
        vmaUnmapMemory(this->_vma_allocator, stagingBufferAllocation);

        create_buffer(buffer_size,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                      VMA_MEMORY_USAGE_GPU_ONLY,
                      index_buffer,
                      index_buffer_allocation);

        this->copy_buffer(stagingBuffer, *index_buffer, buffer_size);

        vmaDestroyBuffer(this->_vma_allocator, stagingBuffer, stagingBufferAllocation);

        return true;
    }

    VkImageView application_window::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mip_levels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.aspectMask = aspect_flags;

        VkImageView imageView;
        if (vkCreateImageView(this->_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    bool application_window::createTextureImageView() {
        this->_texture_image_view = createImageView(this->_texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, this->_mip_levels1);
        this->_texture_image_view2 = createImageView(this->_texture_image2, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, this->_mip_levels2);
        this->_texture_image_view3 = createImageView(this->_texture_image3, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, this->_mip_levels3);
        return true;
    }

    void application_window::createTextureSampler() {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(this->_physical_device, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE; // use [0, 1) coords, not [0, widthWidth/Height)
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(this->_mip_levels3); // I guess we would need one sampler per texture, or texture size, as this value will be different for each texture size

        if (vkCreateSampler(this->_device, &samplerInfo, nullptr, &this->_texture_sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    VkSampleCountFlagBits application_window::getMaxUsableSampleCount() {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(this->_physical_device, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
            physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    void application_window::createDepthResources() {
        VkFormat depthFormat = this->findDepthFormat();

        createImage(this->_swap_chain_extent.width,
                    this->_swap_chain_extent.height,
                    1,
                    this->_msaa_samples,
                    depthFormat,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                    VMA_MEMORY_USAGE_GPU_ONLY,
                    &this->_depth_image,
                    &this->_depth_image_allocation);

        this->_depth_image_view = createImageView(this->_depth_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

        transitionImageLayout(this->_depth_image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
    }

    VkFormat application_window::findDepthFormat() {
        return this->findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    VkFormat application_window::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(this->_physical_device, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    void application_window::create_uniform_buffers() {
        VkDeviceSize bufferSize = sizeof(uniform_buffer_object);

        this->_uniform_buffers.resize(this->_swap_chain_images.size());
        this->_uniform_buffers_allocation.resize(this->_swap_chain_images.size());

        for (size_t i = 0; i < this->_swap_chain_images.size(); i++) {
            this->create_buffer(bufferSize,
                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                VMA_MEMORY_USAGE_CPU_TO_GPU,
                                &this->_uniform_buffers[i],
                                &this->_uniform_buffers_allocation[i]);
        }
    }

    bool application_window::create_descriptor_pool() {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};

        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(this->_swap_chain_images.size());

        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(this->_swap_chain_images.size() * 2); // we have two textures per set

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(this->_swap_chain_images.size());

        if (vkCreateDescriptorPool(this->_device, &poolInfo, nullptr, &this->_descriptor_pool) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create descriptor pool.", apis::logging::log_levels::error);
            return false;
        }

        return true;
    }

    bool application_window::create_descriptor_sets() {
        std::vector<VkDescriptorSetLayout> layouts(this->_swap_chain_images.size(), this->_descriptor_set_layout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = this->_descriptor_pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(this->_swap_chain_images.size());
        allocInfo.pSetLayouts = layouts.data();

        this->_descriptor_sets.resize(this->_swap_chain_images.size());
        if (vkAllocateDescriptorSets(this->_device, &allocInfo, this->_descriptor_sets.data()) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to allocate descriptor sets.", apis::logging::log_levels::error);
            return false;
        }

        for (size_t i = 0; i < this->_swap_chain_images.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = this->_uniform_buffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(uniform_buffer_object);

            std::array<VkDescriptorImageInfo, 2> image_infos;

            image_infos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            //image_infos[0].imageView = this->_texture_image_view;
            image_infos[0].imageView = this->_texture_image_view3; // the model
            image_infos[0].sampler = this->_texture_sampler;

            image_infos[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_infos[1].imageView = this->_texture_image_view2;
            image_infos[1].sampler = this->_texture_sampler;

            std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = this->_descriptor_sets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = this->_descriptor_sets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &image_infos[0];

            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = this->_descriptor_sets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pImageInfo = &image_infos[1];

            vkUpdateDescriptorSets(this->_device,
                                   static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(),
                                   0,
                                   nullptr);
        }

        return true;
    }

    void application_window::update_uniform_buffer(uint32_t current_image_index) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        uniform_buffer_object ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f),
                                time * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));

        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                               glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f));

        ubo.proj = glm::perspective(glm::radians(45.0f),
                                    this->_swap_chain_extent.width / static_cast<float>(this->_swap_chain_extent.height),
                                    0.1f,
                                    10.0f);

        // in Vulkan, -y is up - so flip the Y-scale factor in the projection matrix
        ubo.proj[1][1] *= -1;

        void* data;
        vmaMapMemory(this->_vma_allocator, this->_uniform_buffers_allocation[current_image_index], &data);
        memcpy(data, &ubo, sizeof(ubo));
        vmaUnmapMemory(this->_vma_allocator, this->_uniform_buffers_allocation[current_image_index]);
    }

    void application_window::cleanup_swap_chain() {
        for (auto framebuffer : this->_swap_chain_framebuffers) {
            vkDestroyFramebuffer(this->_device, framebuffer, nullptr);
        }
        this->_swap_chain_framebuffers.clear();

        if (this->_command_pool && !this->_command_buffers.empty()) {
            vkFreeCommandBuffers(this->_device, this->_command_pool,
                                 static_cast<uint32_t>(this->_command_buffers.size()), this->_command_buffers.data());
        }

        for (auto i {0u}; i < this->_swap_chain_images.size(); ++i) {
            vmaDestroyBuffer(this->_vma_allocator,
                             this->_uniform_buffers[i],
                             this->_uniform_buffers_allocation[i]);
        }

        if (this->_descriptor_pool) {
            vkDestroyDescriptorPool(this->_device, this->_descriptor_pool, nullptr);
            this->_descriptor_pool = nullptr;
        }

        if (this->_graphics_pipeline) {
            vkDestroyPipeline(this->_device, this->_graphics_pipeline, nullptr);
            this->_graphics_pipeline = nullptr;
        }

        if (this->_pipeline_layout) {
            vkDestroyPipelineLayout(this->_device, this->_pipeline_layout, nullptr);
            this->_pipeline_layout = nullptr;
        }

        if (this->_render_pass) {
            vkDestroyRenderPass(this->_device, this->_render_pass, nullptr);
            this->_render_pass = nullptr;
        }

        for (auto& view : this->_swap_chain_image_views) {
            vkDestroyImageView(this->_device, view, nullptr);
        }
        this->_swap_chain_image_views.clear();

        if (this->_swap_chain) {
            vkDestroySwapchainKHR(this->_device, this->_swap_chain, nullptr);
            this->_swap_chain = nullptr;
        }
    }

    void application_window::createImage(uint32_t width,
                                         uint32_t height,
                                         uint32_t mip_levels,
                                         VkSampleCountFlagBits samples_count,
                                         VkFormat format,
                                         VkImageTiling tiling,
                                         VkImageUsageFlags usage,
                                         VmaMemoryUsage memory_usage,
                                         VkImage* image,
                                         VmaAllocation* imageAllocation) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mip_levels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = samples_count;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo {};
        allocInfo.usage = memory_usage;

        if (vmaCreateImage(this->_vma_allocator, &imageInfo, &allocInfo, image, imageAllocation, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }
    }

    void application_window::load_image(std::string path, VkImage* image, VmaAllocation* image_allocation, uint32_t& mip_levels) {
        auto surface = IMG_Load(path.c_str());

        // ensure the image is in a format we want to deal with
        auto format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
        auto surfaceRGBA = SDL_ConvertSurface(surface, format, 0);
        SDL_FreeFormat(format);

        SDL_FreeSurface(surface);
        surface = nullptr;

        auto image_width = surfaceRGBA->w;
        auto image_height = surfaceRGBA->h;

        mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(image_width, image_height)))) + 1;

        VkDeviceSize image_size = image_width * image_height * surfaceRGBA->format->BytesPerPixel;

        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferAllocation;

        this->create_buffer(image_size,
                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VMA_MEMORY_USAGE_CPU_TO_GPU,
                            &stagingBuffer,
                            &stagingBufferAllocation);

        void* data;
        vmaMapMemory(this->_vma_allocator,
                     stagingBufferAllocation,
                     &data);
        memcpy(data, surfaceRGBA->pixels, static_cast<size_t>(image_size));
        vmaUnmapMemory(this->_vma_allocator, stagingBufferAllocation);

        SDL_FreeSurface(surfaceRGBA);
        surfaceRGBA = nullptr;

        this->createImage(image_width,
                          image_height,
                          mip_levels,
                          VK_SAMPLE_COUNT_1_BIT,
                          VK_FORMAT_R8G8B8A8_SRGB, // this should really come from the image itself
                          VK_IMAGE_TILING_OPTIMAL,
                          VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                          VMA_MEMORY_USAGE_GPU_ONLY,
                          image,
                          image_allocation);

        this->transitionImageLayout(*image,
                                    VK_FORMAT_R8G8B8A8_SRGB,
                                    VK_IMAGE_LAYOUT_UNDEFINED, // this was set in `createImage` above
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    mip_levels); // the best format to copy to

        this->copyBufferToImage(stagingBuffer,
                                *image,
                                static_cast<uint32_t>(image_width),
                                static_cast<uint32_t>(image_height));

        this->generateMipmaps(*image, VK_FORMAT_R8G8B8A8_SRGB, image_width, image_height, mip_levels);

        vmaDestroyBuffer(this->_vma_allocator, stagingBuffer, stagingBufferAllocation);
    }

    void application_window::create_sample_resources() {
        VkFormat colorFormat = this->_swap_chain_format;

        createImage(this->_swap_chain_extent.width,
                    this->_swap_chain_extent.height,
                    1,
                    this->_msaa_samples,
                    colorFormat,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                    VMA_MEMORY_USAGE_GPU_ONLY,
                    &this->_samples_image,
                    &this->_samples_image_allocation);

        this->_samples_image_view = createImageView(this->_samples_image,
                                                    colorFormat,
                                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                                    1);
    }

    void application_window::generateMipmaps(VkImage image,
                                             VkFormat imageFormat,
                                             int32_t texWidth,
                                             int32_t texHeight,
                                             uint32_t mipLevels) {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(this->_physical_device, imageFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = this->beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (auto i {1u}; i < mipLevels; ++i) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            // if this command buffer is using a dedicated transfer queue, is must also have graphics capability
            vkCmdBlitImage(commandBuffer,
                           image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            if (mipWidth > 1) {
                mipWidth /= 2;
            }

            if (mipHeight > 1) {
                mipHeight /= 2;
            }
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        this->endSingleTimeCommands(commandBuffer);
    }

    void application_window::load_model() {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, "../../../data/viking_room.obj")) {
            throw std::runtime_error(err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1], // vulkan and the obj model format require opposite image V coords
                };

                vertex.color = {1.0f, 1.0f, 1.0f};

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(this->_model_verticies.size());
                    this->_model_verticies.push_back(vertex);
                }

                this->_model_indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    bool application_window::create_texture_image() {
        IMG_Init(IMG_INIT_PNG);

        this->load_image("../../../data/image.png", &this->_texture_image, &this->_texture_image_allocation, this->_mip_levels1);
        this->load_image("../../../data/image2.png", &this->_texture_image2, &this->_texture_image_allocation2, this->_mip_levels2);

        this->load_image("../../../data/viking_room.png", &this->_texture_image3, &this->_texture_image_allocation3, this->_mip_levels3);

        return true;
    }

    VkCommandBuffer application_window::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = this->_command_pool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(this->_device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void application_window::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(this->_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(this->_graphics_queue);

        vkFreeCommandBuffers(this->_device, this->_command_pool, 1, &commandBuffer);
    }

    bool application_window::create_swap_chain() {
        vkDeviceWaitIdle(this->_device);

        this->_signal_swap_chain_out_of_date = false;

        this->cleanup_swap_chain();

        // create the swap chain
        auto swap_chain_details = query_swap_chain_support(this->_physical_device, this->_surface);
        auto surface_format = choose_surface_format(swap_chain_details.surface_formats);
        auto present_mode = choose_present_mode(swap_chain_details.present_modes);
        auto extent = select_swap_chain_extent(swap_chain_details.capabilities, this->_window);

        auto image_count = swap_chain_details.capabilities.minImageCount + 1;
        if (swap_chain_details.capabilities.maxImageCount > 0 && image_count > swap_chain_details.capabilities.maxImageCount) {
            image_count = swap_chain_details.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swap_chain_create_info;
        memset(&swap_chain_create_info, 0, sizeof(swap_chain_create_info));
        swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swap_chain_create_info.surface = this->_surface;
        swap_chain_create_info.minImageCount = image_count;
        swap_chain_create_info.imageFormat = surface_format.format;
        swap_chain_create_info.imageColorSpace = surface_format.colorSpace;
        swap_chain_create_info.imageExtent = extent;
        swap_chain_create_info.imageArrayLayers = 1;
        swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // if we are recreating the swap chain, let the old swap chain continue until the new one is created
        if (this->_swap_chain) {
            swap_chain_create_info.oldSwapchain = this->_swap_chain;
        }

        // this call is duplicated...
        auto qfi = find_queue_families(this->_physical_device, this->_surface);

        uint32_t queue_family_indexes[] = { static_cast<uint32_t>(*qfi.graphics_family_index),
                                            static_cast<uint32_t>(*qfi.present_family_index) };

        if (qfi.graphics_family_index == qfi.present_family_index) {
            swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swap_chain_create_info.queueFamilyIndexCount = 0;
            swap_chain_create_info.pQueueFamilyIndices = nullptr;
        } else {
            swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swap_chain_create_info.queueFamilyIndexCount = 2;
            swap_chain_create_info.pQueueFamilyIndices = queue_family_indexes;
        }

        // maybe useful for when a mobile device changes orientation?
        swap_chain_create_info.preTransform = swap_chain_details.capabilities.currentTransform;

        swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swap_chain_create_info.presentMode = present_mode;
        swap_chain_create_info.clipped = VK_TRUE;
        swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(this->_device, &swap_chain_create_info, nullptr, &this->_swap_chain) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create swap chain.", apis::logging::log_levels::error);
            return false;
        }

        auto swap_chain_image_count {0u};
        vkGetSwapchainImagesKHR(this->_device, this->_swap_chain, &swap_chain_image_count, nullptr);

        this->_swap_chain_images.resize(swap_chain_image_count);
        vkGetSwapchainImagesKHR(this->_device, this->_swap_chain, &swap_chain_image_count, this->_swap_chain_images.data());

        this->_swap_chain_format = swap_chain_create_info.imageFormat;
        this->_swap_chain_extent = swap_chain_create_info.imageExtent;

        this->_swap_chain_image_views.resize(this->_swap_chain_images.size());

        for (auto i {0u}; i < this->_swap_chain_images.size(); ++i)  {
            this->_swap_chain_image_views[i] = this->createImageView(this->_swap_chain_images[i],
                                                                     this->_swap_chain_format,
                                                                     VK_IMAGE_ASPECT_COLOR_BIT,
                                                                     1);
        }

        if (!create_render_pass(this->_swap_chain_format, this->_device, &this->_render_pass)) {
            this->_log_manager->log_message("Failed to create render pass.", apis::logging::log_levels::error);
            return false;
        }

        if (!create_descriptor_set_layout(this->_device, &this->_descriptor_set_layout)) {
            this->_log_manager->log_message("Failed to create descriptor set layout.", apis::logging::log_levels::error);
            return false;
        }

        if (!create_graphics_pipeline(this->_device, extent, &this->_pipeline_layout, this->_render_pass, &this->_graphics_pipeline, &this->_descriptor_set_layout)) {
            this->_log_manager->log_message("Failed to create graphics pipeline.", apis::logging::log_levels::error);
            return false;
        }

        this->create_sample_resources();
        this->createDepthResources();

        if (!create_swap_chain_framebuffers(this->_swap_chain_framebuffers, this->_swap_chain_image_views, this->_render_pass, extent, this->_device)) {
            this->_log_manager->log_message("Failed to create swap chain framebuffers.", apis::logging::log_levels::error);
            return false;
        }

        if (!this->create_vertex_buffer(&this->_vertex_buffer,
                                        &this->_vertex_buffer_allocation,
                                        g_vertices)) {
            this->_log_manager->log_message("Failed to create vertex buffer.", apis::logging::log_levels::error);
            return false;
        }

        if (!this->create_vertex_buffer(&this->_model_vertex_buffer,
                                        &this->_model_vertex_buffer_allocation,
                                        this->_model_verticies)) {
            this->_log_manager->log_message("Failed to create vertex buffer.", apis::logging::log_levels::error);
            return false;
        }

        if (!this->create_index_buffer(&this->_index_buffer,
                                       &this->_index_buffer_allocation,
                                       g_indices)) {
            this->_log_manager->log_message("Failed to create index buffer.", apis::logging::log_levels::error);
            return false;
        }

        if (!this->create_index_buffer(&this->_model_index_buffer,
                                       &this->_model_index_buffer_allocation,
                                       this->_model_indices)) {
            this->_log_manager->log_message("Failed to create model index buffer.", apis::logging::log_levels::error);
            return false;
        }

        this->create_uniform_buffers();

        if (!this->create_descriptor_pool()) {
            this->_log_manager->log_message("Failed to create descriptor pool.", apis::logging::log_levels::error);
            return false;
        }

        if (!this->create_descriptor_sets()) {
            this->_log_manager->log_message("Failed to create descriptor sets.", apis::logging::log_levels::error);
            return false;
        }

        // create the command buffers
        this->_command_buffers.resize(this->_swap_chain_framebuffers.size());

        VkCommandBufferAllocateInfo command_buffer_allocate_info;
        memset(&command_buffer_allocate_info, 0, sizeof(command_buffer_allocate_info));
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.commandPool = this->_command_pool;
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = static_cast<uint32_t>(this->_command_buffers.size());

        if (vkAllocateCommandBuffers(this->_device, &command_buffer_allocate_info, this->_command_buffers.data()) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create command buffers.", apis::logging::log_levels::error);
            return false;
        }

        for (auto i {0u}; i < this->_command_buffers.size(); ++i) {
            VkCommandBufferBeginInfo begin_info;
            memset(&begin_info, 0, sizeof(begin_info));
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags = 0;
            begin_info.pInheritanceInfo = nullptr;

            if (vkBeginCommandBuffer(this->_command_buffers[i], &begin_info) != VK_SUCCESS) {
                this->_log_manager->log_message("Failed to begin command buffer: " + std::to_string(i), apis::logging::log_levels::error);
                return false;
            }

            VkRenderPassBeginInfo render_pass_info;
            memset(&render_pass_info, 0, sizeof(render_pass_info));
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass = this->_render_pass;
            render_pass_info.framebuffer = this->_swap_chain_framebuffers[i];
            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = this->_swap_chain_extent;

            std::array<VkClearValue, 2> clearValues{};
            // this must be in the same order as the attachments in the framebuffer & render pass
            clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
            clearValues[1].depthStencil = {1.0f, 0};

            render_pass_info.clearValueCount = 2;
            render_pass_info.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(this->_command_buffers[i],
                                 &render_pass_info,
                                 VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(this->_command_buffers[i],
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              this->_graphics_pipeline);

            std::array<VkBuffer, 2> vertex_buffers {this->_vertex_buffer, this->_model_vertex_buffer};
            VkDeviceSize offsets[] = {0};

            // the squares
            vkCmdBindVertexBuffers(this->_command_buffers[i],
                                   0,
                                   1,
                                   &vertex_buffers[0],
                                   offsets);

            vkCmdBindIndexBuffer(this->_command_buffers[i],
                                 this->_index_buffer,
                                 0,
                                 VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(this->_command_buffers[i],
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    this->_pipeline_layout,
                                    0,
                                    1,
                                    &this->_descriptor_sets[i],
                                    0,
                                    nullptr);

            vkCmdDrawIndexed(this->_command_buffers[i],
                             static_cast<uint32_t>(g_indices.size()),
                             1,
                             0,
                             0,
                             0);

            // the model
            vkCmdBindVertexBuffers(this->_command_buffers[i],
                                   0,
                                   1,
                                   &vertex_buffers[1],
                                   offsets);

            vkCmdBindIndexBuffer(this->_command_buffers[i],
                                 this->_model_index_buffer,
                                 0,
                                 VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(this->_command_buffers[i],
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    this->_pipeline_layout,
                                    0,
                                    1,
                                    &this->_descriptor_sets[i],
                                    0,
                                    nullptr);

            vkCmdDrawIndexed(this->_command_buffers[i],
                             static_cast<uint32_t>(this->_model_indices.size()),
                             1,
                             0,
                             0,
                             0);

            vkCmdEndRenderPass(this->_command_buffers[i]);

            if (vkEndCommandBuffer(this->_command_buffers[i]) != VK_SUCCESS) {
                this->_log_manager->log_message("Failed to end command buffer: " + std::to_string(i), apis::logging::log_levels::error);
                return false;
            }
        }

        return true;
    }

    void application_window::setup_vma() {
        VmaAllocatorCreateInfo allocatorInfo {};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_1;
        allocatorInfo.physicalDevice = this->_physical_device;
        allocatorInfo.device = this->_device;
        allocatorInfo.instance = this->_vulkan_instance;

        vmaCreateAllocator(&allocatorInfo, &this->_vma_allocator);
    }

    bool application_window::create(std::string_view title,
                                    uint32_t x, uint32_t y,
                                    uint32_t w, uint32_t h) noexcept {
        auto flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE |
                     SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

        this->_window = SDL_CreateWindow(std::string(title).c_str(),
                                         x, y,
                                         w, h,
                                         flags);

        if (!this->_window) {
            this->_log_manager->log_message("Failed to create SDL window.", apis::logging::log_levels::error);
            return false;
        }

        // application information
        VkApplicationInfo ai;
        memset(&ai, 0, sizeof(ai));
        ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ai.pApplicationName = "Project Bird Racing";
        ai.applicationVersion = VK_MAKE_API_VERSION(0, 0, 4, 0);
        ai.pEngineName = "PBR";
        ai.engineVersion = VK_MAKE_API_VERSION(0, 0, 4, 0);
        ai.apiVersion = VK_API_VERSION_1_1;

        // extensions
        auto extensions_count {0u};
        SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, nullptr);

        std::vector<const char*> extensions;
        extensions.resize(extensions_count);

        SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, extensions.data());

#ifdef DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        // validation layers
        std::vector<const char*> validation_layers {
            "VK_LAYER_KHRONOS_validation",
        };

        auto validation_layer_count {0u};
        vkEnumerateInstanceLayerProperties(&validation_layer_count, nullptr);

        std::vector<VkLayerProperties> available_validation_layers;
        available_validation_layers.resize(validation_layer_count);

        vkEnumerateInstanceLayerProperties(&validation_layer_count, available_validation_layers.data());

        std::vector<const char*> available_validation_layers_for_compare;
        for (const auto& l : available_validation_layers) {
            available_validation_layers_for_compare.push_back(l.layerName);
        }

        std::sort(validation_layers.begin(), validation_layers.end(),
                  [](auto& l, auto& r){ return std::string(l) < std::string(r); });
        std::sort(available_validation_layers_for_compare.begin(), available_validation_layers_for_compare.end(),
                  [](auto& l, auto& r){ return std::string(l) < std::string(r); });

        auto are_validation_layers_supported = std::includes(available_validation_layers_for_compare.begin(), available_validation_layers_for_compare.end(),
                                                             validation_layers.begin(), validation_layers.end(),
                                                             [](auto& available_layer, auto& layer) {
            return std::string(available_layer) < std::string(layer);
        });

        if (!are_validation_layers_supported) {
            this->_log_manager->log_message("Requested layers are not supported.", apis::logging::log_levels::error);
            return false;
        }

        // enable the best practices validation layer
        // from: https://vulkan.lunarg.com/doc/sdk/1.2.198.1/windows/best_practices.html
        VkValidationFeatureEnableEXT enables[] = {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT};
        VkValidationFeaturesEXT features = {};
        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features.enabledValidationFeatureCount = 1;
        features.pEnabledValidationFeatures = enables;

        // create the Vulkan instance
        VkInstanceCreateInfo ii;
        memset(&ii, 0, sizeof(ii));
        ii.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        ii.pApplicationInfo = &ai;
        ii.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        ii.ppEnabledExtensionNames = extensions.data();
#ifdef RELEASE
        ii.enabledLayerCount = 0u;
#else
        ii.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        ii.ppEnabledLayerNames = validation_layers.data();
#endif
        ii.pNext = &features;

        if (vkCreateInstance(&ii, nullptr, &this->_vulkan_instance) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create Vulkan instance.", apis::logging::log_levels::error);
            return false;
        }

        // set up the debug callback
        VkDebugUtilsMessengerCreateInfoEXT create_info;
        memset(&create_info, 0, sizeof(create_info));
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debugCallback;
        create_info.pUserData = static_cast<void*>(&this->_log_manager);

        if (CreateDebugUtilsMessengerEXT(this->_vulkan_instance, &create_info, nullptr, &this->_debug_messenger) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create debug messenger.", apis::logging::log_levels::error);
            return false;
        }

        // create window surface
        if (!SDL_Vulkan_CreateSurface(this->_window, this->_vulkan_instance, &this->_surface)) {
            this->_log_manager->log_message("Failed to create surface.", apis::logging::log_levels::error);
            return false;
        }

        // select the physical device to use
        auto physical_device_count {0u};
        vkEnumeratePhysicalDevices(this->_vulkan_instance, &physical_device_count, nullptr);
        if (physical_device_count == 0) {
            this->_log_manager->log_message("Failed to find GPUs with Vulkan support.", apis::logging::log_levels::error);
            return false;
        }

        std::vector<VkPhysicalDevice> physical_devices;
        physical_devices.resize(physical_device_count);
        vkEnumeratePhysicalDevices(this->_vulkan_instance, &physical_device_count, physical_devices.data());

        for (const auto& device : physical_devices) {
            if (is_physical_device_compatible(device, this->_surface)) {
                this->_physical_device = device;
                this->_msaa_samples = this->getMaxUsableSampleCount();
                break;
            }
        }

        if (this->_physical_device == VK_NULL_HANDLE) {
            this->_log_manager->log_message("Failed to find compatible device.", apis::logging::log_levels::error);
            return false;
        }

        // set up the logical device
        auto qfi = find_queue_families(this->_physical_device, this->_surface);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

        std::set<int> unique_queue_ids {
            *qfi.graphics_family_index,
            *qfi.present_family_index
        };

        for (const auto& queue_id : unique_queue_ids) {
            VkDeviceQueueCreateInfo queue_create_info;
            memset(&queue_create_info, 0, sizeof(queue_create_info));
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_id;
            queue_create_info.queueCount = 1;

            auto queue_priority = 1.0f;
            queue_create_info.pQueuePriorities = &queue_priority;

            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features;
        memset(&device_features, 0, sizeof(device_features));
        device_features.samplerAnisotropy = VK_TRUE;
        // aliases pixels within a polygon - will impact performance, so enable on higher graphics settings
        device_features.sampleRateShading = VK_TRUE;

        VkDeviceCreateInfo device_create_info;
        memset(&device_create_info, 0, sizeof(device_create_info));
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = queue_create_infos.data();
        device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pEnabledFeatures = &device_features;

        // do this check in real life:
        // vkCreateDevice: VK_KHR_portability_subset must be enabled because physical device VkPhysicalDevice 0x7fdc1b5ff9a0[] supports it The Vulkan spec states: If the VK_KHR_portability_subset extension is included in pProperties of vkEnumerateDeviceExtensionProperties, ppEnabledExtensions must include "VK_KHR_portability_subset" (https://vulkan.lunarg.com/doc/view/1.2.198.1/mac/1.2-extensions/vkspec.html#VUID-VkDeviceCreateInfo-pProperties-04451)
        std::vector<const char*> device_extensions {
            "VK_KHR_portability_subset",
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };
        device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();
#ifdef RELEASE
        device_create_info.enabledLayerCount = 0u;
#else
        device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        device_create_info.ppEnabledLayerNames = validation_layers.data();
#endif

        if (vkCreateDevice(this->_physical_device, &device_create_info, nullptr, &this->_device) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create logical device.", apis::logging::log_levels::error);
            return false;
        }

        this->setup_vma();

        vkGetDeviceQueue(this->_device, *qfi.graphics_family_index, 0, &this->_graphics_queue);
        vkGetDeviceQueue(this->_device, *qfi.present_family_index, 0, &this->_present_queue);

        // create the command pool
        VkCommandPoolCreateInfo pool_info;
        memset(&pool_info, 0, sizeof(pool_info));
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = *qfi.graphics_family_index;
        pool_info.flags = 0;

        if (vkCreateCommandPool(this->_device, &pool_info, nullptr, &this->_command_pool) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create command pool.", apis::logging::log_levels::error);
            return false;
        }

        if (!this->create_texture_image()) {
            this->_log_manager->log_message("Failed to create texture image.", apis::logging::log_levels::error);
            return false;
        }

        if (!this->createTextureImageView()) {
            this->_log_manager->log_message("Failed to create texture image view.", apis::logging::log_levels::error);
            return false;
        }

        this->createTextureSampler();

        this->load_model();

        if (!this->create_swap_chain()) {
            this->_log_manager->log_message("Failed to create swap chain.", apis::logging::log_levels::error);
            return false;
        }

        // create sync objects
        VkSemaphoreCreateInfo semaphore_create_info;
        memset(&semaphore_create_info, 0, sizeof(semaphore_create_info));
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info;
        memset(&fence_info, 0, sizeof(fence_info));
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        this->_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        this->_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        this->_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
        this->_images_in_flight.resize(this->_swap_chain_images.size(), VK_NULL_HANDLE);

        for (auto i {0u}; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (vkCreateSemaphore(this->_device, &semaphore_create_info, nullptr, &this->_image_available_semaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(this->_device, &semaphore_create_info, nullptr, &this->_render_finished_semaphores[i]) != VK_SUCCESS ||
                vkCreateFence(this->_device, &fence_info, nullptr, &this->_in_flight_fences[i]) != VK_SUCCESS) {
                this->_log_manager->log_message("Failed to create semaphores.", apis::logging::log_levels::error);
                return false;
            }
        }

        return true;
    }

    void application_window::shutdown() noexcept {
        if (this->_vulkan_instance) {
            // wait for any remaining async operations to complete before we tear everything down
            vkDeviceWaitIdle(this->_device);

            this->cleanup_swap_chain();

            for (auto i {0u}; i < MAX_FRAMES_IN_FLIGHT; ++i) {
                if (this->_image_available_semaphores[i]) {
                    vkDestroySemaphore(this->_device, this->_image_available_semaphores[i], nullptr);
                }

                if (this->_render_finished_semaphores[i]) {
                    vkDestroySemaphore(this->_device, this->_render_finished_semaphores[i], nullptr);
                }

                if (this->_in_flight_fences[i]) {
                    vkDestroyFence(this->_device, this->_in_flight_fences[i], nullptr);
                }
            }
            this->_image_available_semaphores.clear();
            this->_render_finished_semaphores.clear();
            this->_in_flight_fences.clear();

            if (this->_texture_sampler) {
                vkDestroySampler(this->_device, this->_texture_sampler, nullptr);
                this->_texture_sampler = nullptr;
            }

            if (this->_texture_image_view) {
                vkDestroyImageView(this->_device, this->_texture_image_view, nullptr);
                this->_texture_image_view = nullptr;
            }

            if (this->_texture_image) {
                vmaDestroyImage(this->_vma_allocator, this->_texture_image, _texture_image_allocation);
                this->_texture_image = nullptr;
            }

            if (this->_texture_image_view2) {
                vkDestroyImageView(this->_device, this->_texture_image_view2, nullptr);
                this->_texture_image_view2 = nullptr;
            }

            if (this->_texture_image2) {
                vmaDestroyImage(this->_vma_allocator, this->_texture_image2, _texture_image_allocation2);
                this->_texture_image2 = nullptr;
            }

            if (this->_texture_image_view3) {
                vkDestroyImageView(this->_device, this->_texture_image_view3, nullptr);
                this->_texture_image_view3 = nullptr;
            }

            if (this->_texture_image3) {
                vmaDestroyImage(this->_vma_allocator, this->_texture_image3, this->_texture_image_allocation3);
                this->_texture_image3 = nullptr;
            }

            if (this->_depth_image_view) {
                vkDestroyImageView(this->_device, this->_depth_image_view, nullptr);
                this->_depth_image_view = nullptr;
            }

            if (this->_depth_image) {
                vmaDestroyImage(this->_vma_allocator, this->_depth_image, _depth_image_allocation);
                this->_depth_image = nullptr;
            }

            if (this->_samples_image_view) {
                vkDestroyImageView(this->_device, this->_samples_image_view, nullptr);
                this->_samples_image_view = nullptr;
            }

            if (this->_samples_image) {
                vmaDestroyImage(this->_vma_allocator, this->_samples_image, _samples_image_allocation);
                this->_samples_image = nullptr;
            }

            if (this->_vertex_buffer) {
                vmaDestroyBuffer(this->_vma_allocator,
                                 this->_vertex_buffer,
                                 this->_vertex_buffer_allocation);
                this->_vertex_buffer = nullptr;
                this->_vertex_buffer_allocation = nullptr;
            }

            if (this->_index_buffer) {
                vmaDestroyBuffer(this->_vma_allocator,
                                 this->_index_buffer,
                                 this->_index_buffer_allocation);
                this->_index_buffer = nullptr;
                this->_index_buffer_allocation = nullptr;
            }

            if (this->_model_vertex_buffer) {
                vmaDestroyBuffer(this->_vma_allocator,
                                 this->_model_vertex_buffer,
                                 this->_model_vertex_buffer_allocation);
                this->_model_vertex_buffer = nullptr;
                this->_model_vertex_buffer_allocation = nullptr;
            }

            if (this->_model_index_buffer) {
                vmaDestroyBuffer(this->_vma_allocator,
                                 this->_model_index_buffer,
                                 this->_model_index_buffer_allocation);
                this->_model_index_buffer = nullptr;
                this->_model_index_buffer_allocation = nullptr;
            }

            if (this->_command_pool) {
                vkDestroyCommandPool(this->_device, this->_command_pool, nullptr);
                this->_command_pool = nullptr;
            }

            if (this->_descriptor_set_layout) {
                vkDestroyDescriptorSetLayout(this->_device, this->_descriptor_set_layout, nullptr);
                this->_descriptor_set_layout = nullptr;
            }

            if (this->_vma_allocator) {
                vmaDestroyAllocator(this->_vma_allocator);
                this->_vma_allocator = nullptr;
            }

            if (this->_device) {
                vkDestroyDevice(this->_device, nullptr);
                this->_device = nullptr;
            }

            if (this->_debug_messenger) {
                DestroyDebugUtilsMessengerEXT(this->_vulkan_instance, this->_debug_messenger, nullptr);
                this->_debug_messenger = nullptr;
            }

            if (this->_surface) {
                vkDestroySurfaceKHR(this->_vulkan_instance, this->_surface, nullptr);
                this->_surface = nullptr;
            }

            vkDestroyInstance(this->_vulkan_instance, nullptr);
            this->_vulkan_instance = nullptr;
        }

        if (this->_window) {
            SDL_DestroyWindow(this->_window);
            this->_window = nullptr;
        }
    }

    void application_window::update() {
        vkWaitForFences(this->_device, 1, &this->_in_flight_fences[this->_current_frame], VK_TRUE, UINT64_MAX);

        auto image_index {0u};
        auto result = vkAcquireNextImageKHR(this->_device, this->_swap_chain, UINT64_MAX, this->_image_available_semaphores[this->_current_frame], VK_NULL_HANDLE, &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            this->create_swap_chain();
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            this->_log_manager->log_message("Failed to acquire next image.", apis::logging::log_levels::error);
            return;
        }

        // Check if a previous frame is using this image (i.e. there is its fence to wait on)
        if (this->_images_in_flight[image_index] != VK_NULL_HANDLE) {
            vkWaitForFences(this->_device, 1, &this->_images_in_flight[image_index], VK_TRUE, UINT64_MAX);
        }
        // Mark the image as now being in use by this frame
        this->_images_in_flight[image_index] = this->_in_flight_fences[this->_current_frame];

        this->update_uniform_buffer(image_index);

        VkSubmitInfo submit_info;
        memset(&submit_info, 0, sizeof(submit_info));
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore wait_semaphores[] { this->_image_available_semaphores[this->_current_frame] };
        VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &this->_command_buffers[image_index];

        VkSemaphore signal_semaphores[] { this->_render_finished_semaphores[this->_current_frame] };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        vkResetFences(this->_device, 1, &this->_in_flight_fences[this->_current_frame]);

        if (vkQueueSubmit(this->_graphics_queue, 1, &submit_info, this->_in_flight_fences[this->_current_frame]) != VK_SUCCESS) {
            std::cerr << "Failed to submit queue.\n";
            return;
        }

        VkPresentInfoKHR present_info;
        memset(&present_info, 0, sizeof(present_info));
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swap_chains[] { this->_swap_chain };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swap_chains;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;

        result = vkQueuePresentKHR(this->_present_queue, &present_info);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->_signal_swap_chain_out_of_date) {
            this->create_swap_chain();
        } else if (result != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to acquire next image.", apis::logging::log_levels::error);
            return;
        }

        this->_current_frame = (this->_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}
