#include "image.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Creates the image create info
    /// \param width The image width in pixels
    /// \param height The image height in pixels
    /// \param mip_levels The number of mip map levels to generate
    /// \param samples_count The number of samples this image should have
    /// \param format The image format
    /// \param tiling How to tile this image
    /// \param usage How this image will be used
    /// \returns The image view create info
    VkImageCreateInfo create_view_create_info(uint32_t width,
                                              uint32_t height,
                                              uint32_t mip_levels,
                                              VkFormat format,
                                              VkImageTiling tiling,
                                              VkImageUsageFlags usage,
                                              VkSampleCountFlagBits samples_count) {
        VkImageCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        create_info.imageType = VK_IMAGE_TYPE_2D;
        create_info.extent.width = width;
        create_info.extent.height = height;
        create_info.extent.depth = 1;
        create_info.mipLevels = mip_levels;
        create_info.arrayLayers = 1;
        create_info.format = format;
        create_info.tiling = tiling;
        create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        create_info.usage = usage;
        create_info.samples = samples_count;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        return create_info;
    }

    image::image(device& device,
                 const vma& vma,
                 uint32_t width,
                 uint32_t height,
                 uint32_t mip_levels,
                 VkSampleCountFlagBits samples_count,
                 VkFormat format,
                 VkImageAspectFlags aspect,
                 VkImageTiling tiling,
                 VkImageUsageFlags usage,
                 VmaMemoryUsage memory_usage,
                 std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
          _vma(vma),
          _log_manager(log_manager) {

        auto create_info = create_view_create_info(width,
                                                   height,
                                                   mip_levels,
                                                   format,
                                                   tiling,usage,
                                                   samples_count);

        auto allocation_create_info = vma::create_allocation_create_info(memory_usage);

        if (vmaCreateImage(vma.get_native_handle(),
                           &create_info,
                           &allocation_create_info,
                           &this->_image,
                           &this->_allocation,
                           nullptr) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create image.")
        }

        this->_view = std::make_unique<image_view>(this->_device,
                                                   this->_image,
                                                   format,
                                                   aspect,
                                                   mip_levels,
                                                   this->_log_manager);
    }

    image::~image() {
        this->_view.release();

        if (this->_allocation && this->_image) {
            vmaDestroyImage(this->_vma.get_native_handle(),
                            this->_image,
                            this->_allocation);

            this->_image = nullptr;
            this->_allocation = nullptr;
        }
    }
}
