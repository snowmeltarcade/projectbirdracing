#include "buffer.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Creates a buffer create info struct
    /// \param size The size of the buffer to create in bytes
    /// \param flags The usage flags
    /// \returns The buffer create info struct
    VkBufferCreateInfo create_buffer_create_info(uint32_t size, VkBufferUsageFlags flags) {
        VkBufferCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create_info.size = size;
        create_info.usage = flags;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        return create_info;
    }

    buffer::buffer(const vma& vma,
                   uint32_t size,
                   VkBufferUsageFlags flags,
                   VmaMemoryUsage memory_usage,
                   std::shared_ptr<logging::ilog_manager> log_manager)
        : _vma(vma),
          _log_manager(log_manager) {
        auto create_info = create_buffer_create_info(size, flags);

        auto allocation_create_info = vma::create_allocation_create_info(memory_usage);

        if (vmaCreateBuffer(this->_vma.get_native_handle(),
                            &create_info,
                            &allocation_create_info,
                            &this->_buffer,
                            &this->_allocation,
                            nullptr) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create buffer.")
        }
    }

    buffer::~buffer() {
        if (this->_buffer) {
            vmaDestroyBuffer(this->_vma.get_native_handle(),
                             this->_buffer,
                             this->_allocation);
            this->_buffer = nullptr;
            this->_allocation = nullptr;
        }
    }
}
