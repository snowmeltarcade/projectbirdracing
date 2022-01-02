#include "fence.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Creates a fence create info struct
    /// \returns The fence create info struct
    VkFenceCreateInfo create_fence_create_info() noexcept {
        VkFenceCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        return create_info;
    }

    fence::fence(const device& device,
                 std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
            _log_manager(log_manager) {
        auto create_info = create_fence_create_info();

        if (vkCreateFence(this->_device.get_native_handle(),
                          &create_info,
                          nullptr,
                          &this->_fence) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create a fence.")
        }
    }

    fence::~fence() {
        if (this->_fence) {
            vkDestroyFence(this->_device.get_native_handle(),
                           this->_fence,
                           nullptr);
            this->_fence = nullptr;
        }
    }
}
