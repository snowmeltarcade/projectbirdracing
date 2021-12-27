#pragma once

#include "device.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// Provides a view into a `VkImage`
    class image_view {
    public:
        /// Constructs this image view
        /// \param device The logical device
        /// \param image The image to create this view from
        /// \param format The format this view should take on
        /// \param aspect What aspect this view should have - color or depth for example
        /// \param mip_levels The number of mip map levels to generate
        /// \param log_manager The log manager to use
        image_view(device& device,
                   VkImage image,
                   VkFormat format,
                   VkImageAspectFlags aspect,
                   uint32_t mip_levels,
                   std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this image view
        ~image_view();

    private:
        /// The logical device
        device& _device;

        /// The image view
        VkImageView _view {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
