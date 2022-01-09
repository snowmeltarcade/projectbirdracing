#pragma once

#include "device.h"
#include "vma.h"
#include "image.h"
#include "render_pass.h"
#include "swap_chain.h"
#include "command_pool.h"
#include "queue.h"

#include <array>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// A framebuffer. This will implement the render pass object. The backing images
    /// are contained and managed in this class. Internally, this will contain one framebuffer
    /// per swap chain image
    class framebuffer {
    public:
        /// Constructs this framebuffer
        /// \param device The logical device
        /// \param vma The memory allocator
        /// \param render_pass The render pass to base this framebuffer on
        /// \param swap_chain The swap chain
        /// \param command_pool The command pool
        /// \param graphics_queue The graphics queue to submit image transition commands to
        /// \param log_manager The log manager to use
        framebuffer(const device& device,
                    const vma& vma,
                    const render_pass& render_pass,
                    const swap_chain& swap_chain,
                    const command_pool& command_pool,
                    const queue& graphics_queue,
                    std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this framebuffer
        ~framebuffer();

        /// Returns the native handle to this framebuffer
        /// \param index The index of the framebuffer to return
        /// \returns The native handle to this framebuffer
        [[nodiscard]]
        VkFramebuffer get_native_handle(uint32_t index) const noexcept {
            assert((index < this->_framebuffers.size()));
            return this->_framebuffers[index];
        }

    private:
        /// The logical device
        const device& _device;

        /// The memory allocator
        const vma& _vma;

        /// The render pass this framebuffer is based on
        const render_pass& _render_pass;

        /// The swap chain to use
        const swap_chain& _swap_chain;

        /// The command pool
        const command_pool& _command_pool;

        /// The graphics queue
        const queue& _graphics_queue;

        /// The framebuffer
        std::vector<VkFramebuffer> _framebuffers;

        /// The MSAA samples image
        std::unique_ptr<image> _color_samples_image;

        /// The depth buffer image
        std::unique_ptr<image> _depth_image;

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;

        /// Creates the images to back this framebuffer
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create_images() noexcept;

        /// Creates the depth image
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create_depth_image() noexcept;

        /// Creates the color samples image
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create_color_samples_image() noexcept;

        /// Creates a framebuffer create info struct
        /// \param attachments Array to store the attachments
        /// \param view The swap chain image view to create this framebuffer for
        /// \returns The framebuffer create info struct
        VkFramebufferCreateInfo create_framebuffer_create_info(std::array<VkImageView, 3>& attachments,
                                                               const image_view& view) const noexcept;
    };
}
