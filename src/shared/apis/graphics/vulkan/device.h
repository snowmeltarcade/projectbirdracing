#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "physical_device.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <memory>

namespace pbr::shared::apis::graphics::vulkan {
    /// A logical device
    class device {
    public:
        /// Constructs this device
        /// \param instance The Vulkan instance
        /// \param physical_device The physical device
        /// \param log_manager The log manager to use
        device(instance& instance,
               physical_device& physical_device,
               std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this device
        ~device();

    private:
        /// The logical device
        VkDevice _device {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
