#include "semaphore.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Creates a semaphore create info struct
    /// \returns The semaphore create info struct
    VkSemaphoreCreateInfo create_semaphore_create_info() noexcept {
        VkSemaphoreCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        return create_info;
    }

    semaphore::semaphore(const device& device,
                         std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
            _log_manager(log_manager) {
        auto create_info = create_semaphore_create_info();

        if (vkCreateSemaphore(this->_device.get_native_handle(),
                              &create_info,
                              nullptr,
                              &this->_semaphore) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create a semaphore.")
        }
    }

    semaphore::~semaphore() {
        if (this->_semaphore) {
            vkDestroySemaphore(this->_device.get_native_handle(),
                               this->_semaphore,
                               nullptr);
            this->_semaphore = nullptr;
        }
    }
}
