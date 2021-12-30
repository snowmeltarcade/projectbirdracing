#pragma once

#include "device.h"
#include "vma.h"
#include "image.h"
#include "render_pass.h"
#include "swap_chain.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// A framebuffer. This will implement the render pass object. The backing images
    /// are contained and managed in this class.
    class framebuffer {
    public:
        /// Constructs this framebuffer
        /// \param device The logical device
        /// \param vma The memory allocator
        /// \param render_pass The render pass to base this framebuffer on
        /// \param swap_chain The swap chain
        /// \param log_manager The log manager to use
        framebuffer(const device& device,
                    const vma& vma,
                    const render_pass& render_pass,
                    const swap_chain& swap_chain,
                    std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this framebuffer
        ~framebuffer();

    private:
        /// Creates the images to back this framebuffer
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create_images() noexcept;

        /// The logical device
        const device& _device;

        /// The memory allocator
        const vma& _vma;

        /// The render pass this framebuffer is based on
        const render_pass& _render_pass;

        /// The swap chain to use
        const swap_chain& _swap_chain;

        /// The framebuffer
        VkFramebuffer _framebuffer {VK_NULL_HANDLE};

        /// The color image used for presenting to the screen
        std::unique_ptr<image> _color_image;

        /// The MSAA samples image
        std::unique_ptr<image> _color_samples_image;

        /// The depth buffer image
        std::unique_ptr<image> _depth_image;

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
