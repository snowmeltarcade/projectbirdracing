#pragma once

#include "device.h"
#include "vma.h"
#include "image_view.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// An image. This will also contain the image view
    class image {
    public:
        /// Constructs this image
        /// \param device The logical device
        /// \param vma The memory allocator
        /// \param image The image to create this view from
        /// \param width The image width in pixels
        /// \param height The image height in pixels
        /// \param mip_levels The number of mip map levels to generate
        /// \param samples_count The number of samples this image should have
        /// \param format The image format
        /// \param aspect What aspect this view should have - color or depth for example
        /// \param tiling How to tile this image
        /// \param usage How this image will be used
        /// \param memory_usage How the image memory will be used
        /// \param log_manager The log manager to use
        image(const device& device,
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
              std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this image
        ~image();

        /// Transitions this image from the old layout to the new layout
        /// \param old_layout The old image layout
        /// \param new_layout The new image layout
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool transition_layout(VkImageLayout old_layout, VkImageLayout new_layout) noexcept;

    private:
        /// The logical device
        const device& _device;

        /// The memory allocator
        const vma& _vma;

        /// The number of mipmap levels
        uint32_t _mip_levels {1u};

        /// The format of this image
        VkFormat _format;

        // The image view
        std::unique_ptr<image_view> _view;

        /// The image
        VkImage _image {VK_NULL_HANDLE};

        /// The memory allocation
        VmaAllocation _allocation {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
