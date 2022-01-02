#include "framebuffer.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    VkFramebufferCreateInfo framebuffer::create_framebuffer_create_info(std::array<VkImageView, 3>& attachments,
                                                                        const image_view& view) const noexcept {
        // this order matches the order set out in the render pass
        attachments[0] = this->_color_samples_image->get_view_native_handle();
        attachments[1] = view.get_native_handle();
        attachments[2] = this->_depth_image->get_view_native_handle();

        VkFramebufferCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = this->_render_pass.get_native_handle();
        create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        create_info.pAttachments = attachments.data();
        create_info.width = this->_swap_chain.get_extent().width;
        create_info.height = this->_swap_chain.get_extent().height;
        create_info.layers = 1;

        return create_info;
    }

    framebuffer::framebuffer(const device& device,
                             const vma& vma,
                             const render_pass& render_pass,
                             const swap_chain& swap_chain,
                             const command_pool& command_pool,
                             const queue& graphics_queue,
                             std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
            _vma(vma),
            _render_pass(render_pass),
            _swap_chain(swap_chain),
            _command_pool(command_pool),
            _graphics_queue(graphics_queue),
            _log_manager(log_manager) {
        this->_log_manager->log_message("Creating framebuffer...",
                                        logging::log_levels::info,
                                        "Vulkan");

        if (!this->create_images()) {
            FATAL_ERROR("Failed to create images.")
        }

        for (const auto& swap_chain_image_view : this->_swap_chain.get_image_views()) {
            std::array<VkImageView, 3> attachments;
            auto create_info = this->create_framebuffer_create_info(attachments,
                                                                    swap_chain_image_view);

            VkFramebuffer buffer {VK_NULL_HANDLE};

            if (vkCreateFramebuffer(this->_device.get_native_handle(),
                                    &create_info,
                                    nullptr,
                                    &buffer) != VK_SUCCESS) {
                FATAL_ERROR("Failed to create frame buffer.")
            }

            this->_framebuffers.push_back(buffer);
        }

        this->_log_manager->log_message("Created framebuffer.",
                                        logging::log_levels::info,
                                        "Vulkan");
    }

    framebuffer::~framebuffer() {
        this->_depth_image.reset();

        this->_color_samples_image.reset();

        for (const auto& framebuffer : this->_framebuffers) {
            vkDestroyFramebuffer(this->_device.get_native_handle(),
                                 framebuffer,
                                 nullptr);
        }

        this->_framebuffers.clear();
    }

    bool framebuffer::create_images() noexcept {
        if (!this->create_depth_image()) {
            this->_log_manager->log_message("Failed to create depth image.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        if (!this->create_color_samples_image()) {
            this->_log_manager->log_message("Failed to create color samples image.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        return true;
    }

    bool framebuffer::create_depth_image() noexcept {
        this->_depth_image = std::make_unique<image>(this->_device,
                                                     this->_vma,
                                                     this->_swap_chain.get_extent().width,
                                                     this->_swap_chain.get_extent().height,
                                                     1,
                                                     this->_render_pass.get_msaa_samples(),
                                                     this->_render_pass.get_depth_format(),
                                                     VK_IMAGE_ASPECT_DEPTH_BIT,
                                                     VK_IMAGE_TILING_OPTIMAL,
                                                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                     VMA_MEMORY_USAGE_GPU_ONLY,
                                                     this->_log_manager);

        if (!this->_depth_image->transition_layout(VK_IMAGE_LAYOUT_UNDEFINED,
                                                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                                   this->_command_pool,
                                                   this->_graphics_queue)) {
            this->_log_manager->log_message("Failed to transition depth image.",
                                            logging::log_levels::error,
                                            "Vulkan");

            return false;
        }

        return true;
    }

    bool framebuffer::create_color_samples_image() noexcept {
        this->_color_samples_image = std::make_unique<image>(this->_device,
                                                             this->_vma,
                                                             this->_swap_chain.get_extent().width,
                                                             this->_swap_chain.get_extent().height,
                                                             1,
                                                             this->_render_pass.get_msaa_samples(),
                                                             this->_swap_chain.get_image_format(),
                                                             VK_IMAGE_ASPECT_COLOR_BIT,
                                                             VK_IMAGE_TILING_OPTIMAL,
                                                             VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                             VMA_MEMORY_USAGE_GPU_ONLY,
                                                             this->_log_manager);

        return true;
    }
}
