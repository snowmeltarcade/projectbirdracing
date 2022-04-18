#include "image_view.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Creates the image view create info
    /// \param image The image to create this view from
    /// \param format The format this view should take on
    /// \param aspect What aspect this view should have - color or depth for example
    /// \param mip_levels The number of mip map levels to generate
    /// \returns The image view create info
    VkImageViewCreateInfo create_view_create_info(VkImage image,
                                                  VkFormat format,
                                                  VkImageAspectFlags aspect,
                                                  uint32_t mip_levels) {
        VkImageViewCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = image;
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = format;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = mip_levels;
        create_info.subresourceRange.layerCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.aspectMask = aspect;

        return create_info;
    }

    image_view::image_view(const device& device,
                           VkImage image,
                           VkFormat format,
                           VkImageAspectFlags aspect,
                           uint32_t mip_levels,
                           std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
          _log_manager(log_manager) {
        auto create_info = create_view_create_info(image, format, aspect, mip_levels);

        if (vkCreateImageView(this->_device.get_native_handle(),
                              &create_info,
                              nullptr,
                              &this->_view) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create image view.")
        }
    }

    image_view::~image_view() {
        if (this->_view) {
            vkDestroyImageView(this->_device.get_native_handle(), this->_view, nullptr);
            this->_view = nullptr;
        }
    }
}
