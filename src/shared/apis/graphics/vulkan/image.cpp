#include "image.h"
#include "command_buffer.h"

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

    image::image(const device& device,
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
          _mip_levels(mip_levels),
          _format(format),
          _log_manager(log_manager) {

        auto create_info = create_view_create_info(width,
                                                   height,
                                                   this->_mip_levels,
                                                   this->_format,
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
                                                   this->_format,
                                                   aspect,
                                                   this->_mip_levels,
                                                   this->_log_manager);
    }

    image::~image() {
        this->_view.reset();

        if (this->_allocation && this->_image) {
            vmaDestroyImage(this->_vma.get_native_handle(),
                            this->_image,
                            this->_allocation);

            this->_image = nullptr;
            this->_allocation = nullptr;
        }
    }

    /// Returns `true` if `format` has a stencil component, else `false`
    /// \returns `true` if `format` has a stencil component, else `false`
    [[nodiscard]]
    bool has_stencil_component(VkFormat format) noexcept {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    bool image::transition_layout(VkImageLayout old_layout,
                                  VkImageLayout new_layout,
                                  const command_pool& command_pool,
                                  const queue& graphics_queue) noexcept {
        VkPipelineStageFlags source_stage;
        VkPipelineStageFlags destination_stage;

        auto barrier = this->create_memory_barrier(old_layout, new_layout);

        if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (has_stencil_component(this->_format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
            new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
                   new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            this->_log_manager->log_message("Unsupported layout transition.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        command_buffer buffer(this->_device, command_pool);
        buffer.begin_one_time_usage();

        vkCmdPipelineBarrier(
            buffer.get_native_handle(),
            source_stage, destination_stage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        buffer.end_one_time_usage(graphics_queue);

        return true;
    }

    VkImageMemoryBarrier image::create_memory_barrier(VkImageLayout old_layout,
                                                      VkImageLayout new_layout) const noexcept {
        VkImageMemoryBarrier barrier {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = this->_image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = this->_mip_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        return barrier;
    }
}
