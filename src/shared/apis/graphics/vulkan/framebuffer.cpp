#include "framebuffer.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Creates a framebuffer create info struct
    /// \param render_pass The render pass to base this framebuffer on
    /// \param swap_chain_extent The size (extent) of the swap chain images
    /// \returns The framebuffer create info struct
    VkFramebufferCreateInfo create_framebuffer_create_info(const render_pass& render_pass,
                                                           VkExtent2D swap_chain_extent) {
        VkFramebufferCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = render_pass.get_native_handle();
        create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        create_info.pAttachments = attachments.data();
        create_info.width = swap_chain_extent.width;
        create_info.height = swap_chain_extent.height;
        create_info.layers = 1;
    }

    framebuffer::framebuffer(const device& device,
                             const vma& vma,
                             const render_pass& render_pass,
                             const swap_chain& swap_chain,
                std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
            _vma(vma),
            _log_manager(log_manager) {
        this->_log_manager->log_message("Creating framebuffer...",
                                        logging::log_levels::info,
                                        "Vulkan");

        auto create_info = create_framebuffer_create_info(render_pass,
                                                          swap_chain.get_extent());

        if (!this->create_images()) {
            FATAL_ERROR("Failed to create images.")
        }

        if (vkCreateFramebuffer(this->_device.get_native_handle(),
                                &create_info,
                                nullptr,
                                &this->_framebuffer) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create frame buffer.")
        }

        this->_log_manager->log_message("Created framebuffer.",
                                        logging::log_levels::info,
                                        "Vulkan");
    }

    framebuffer::~framebuffer() {
        if (this->_framebuffer) {
            vkDestroyFramebuffer(this->_device.get_native_handle(),
                                 this->_framebuffer,
                                 nullptr);
            this->_framebuffer = nullptr;
        }
    }

    bool framebuffer::create_images() noexcept {
        VkFormat depthFormat = this->findDepthFormat();

        createImage(this->_swap_chain_extent.width,
                    this->_swap_chain_extent.height,
                    1,
                    this->_msaa_samples,
                    depthFormat,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                    VMA_MEMORY_USAGE_GPU_ONLY,
                    &this->_depth_image,
                    &this->_depth_image_allocation);

        this->_depth_image_view = createImageView(this->_depth_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

        transitionImageLayout(this->_depth_image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

        return true;
    }
}
