#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "device.h"

#include <memory>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// Manages the lifetime of the command pool
    class command_pool {
    public:
        /// Constructs this command pool
        /// \param device The logical device
        /// \param queue_index The queue for this command pool
        /// \param log_manager The log manager to use
        command_pool(device& device,
                     uint32_t queue_index,
                     std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this command pool
        ~command_pool();

        /// Returns the native handle to this command pool
        /// \returns The native handle to this command pool
        [[nodiscard]]
        VkCommandPool get_native_handle() const noexcept {
            return this->_command_pool;
        }

    private:
        /// The logical device
        device& _device;

        /// The command pool
        VkCommandPool _command_pool {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
