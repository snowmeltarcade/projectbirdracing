#pragma once

#include "device.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// Manages the lifecycle of a `VkFence`
    class fence {
    public:
        /// Constructs this fence
        /// \param device The logical device
        /// \param log_manager The log manager to use
        fence(const device& device,
              std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destructs this fence
        ~fence();

        /// Returns the native handle to this fence
        /// \returns The native handle to this fence
        [[nodiscard]]
        VkFence get_native_handle() const noexcept {
            return this->_fence;
        }

    private:
        /// The logical device
        const device& _device;

        /// The fence
        VkFence _fence {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
