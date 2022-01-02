#include "render_system_screen_aligned_2d.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics {
    render_system_screen_aligned_2d::render_system_screen_aligned_2d(const vulkan::device& device,
                                                                     const vulkan::vma& vma,
                                                                     uint32_t number_of_swap_chain_images,
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
    }

    render_system_screen_aligned_2d::~render_system_screen_aligned_2d() {
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
}
