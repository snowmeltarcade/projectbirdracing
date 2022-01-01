#pragma once

#include "device.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// Manages the lifecycle of a `VkSemaphore`
    class semaphore {
    public:
        /// Constructs this semaphore
        /// \param device The logical device
        /// \param log_manager The log manager to use
        semaphore(const device& device,
              std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destructs this semaphore
        ~semaphore();

        /// Returns the native handle to this semaphore
        /// \returns The native handle to this semaphore
        [[nodiscard]]
        VkSemaphore get_native_handle() const noexcept {
            return this->_semaphore;
        }

    private:
        /// The logical device
        const device& _device;

        /// The semaphore
        VkSemaphore _semaphore {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
