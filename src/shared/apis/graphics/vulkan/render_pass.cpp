#include "render_pass.h"
#include "performance_settings.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Creates an attachment's description
    /// \returns The attachment's description
    VkAttachmentDescription create_attachment_description(VkFormat image_format,
                                                          VkSampleCountFlagBits msaa_samples,
                                                          VkImageLayout image_layout) {
        VkAttachmentDescription description {};
        description.format = image_format;
        description.samples = msaa_samples;
        description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        description.finalLayout = image_layout;

        return description;
    }

    /// Creates an attachment reference
    /// \param attachment_index The index of the attachment
    /// \param image_layout The image layout
    /// \returns The created attachment reference
    VkAttachmentReference create_attachment_reference(uint32_t attachment_index, VkImageLayout image_layout) {
        VkAttachmentReference attachment_ref {};
        attachment_ref.attachment = attachment_index;
        attachment_ref.layout = image_layout;

        return attachment_ref;
    }

    /// Creates a subpass description
    /// \param color_attachment_reference The color attachment reference
    /// \param color_resolver_attachment_reference The color resolver attachment reference
    /// \param depth_attachment_reference The depth attachment reference
    /// \returns The subpass description
    VkSubpassDescription create_subpass_description(const VkAttachmentReference& color_attachment_reference,
                                                    const VkAttachmentReference& color_resolver_attachment_reference,
                                                    const VkAttachmentReference& depth_attachment_reference) {
        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_reference;
        subpass.pDepthStencilAttachment = &depth_attachment_reference;
        subpass.pResolveAttachments = &color_resolver_attachment_reference;

        return subpass;
    }

    /// Creates a subpass dependency
    /// \returns The subpass dependency
    VkSubpassDependency create_subpass_dependency() {
        VkSubpassDependency dependency {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        return dependency;
    }

    /// Creates the render pass create info
    /// \param attachments An array to store the attachments
    /// \param color_attachment The color attachment
    /// \param color_resolver_attachment The color resolver attachment
    /// \param depth_attachment The depth attachment
    /// \param subpass_description The subpass description
    /// \param subpass_dependency The subpass dependency
    /// \returns The render pass create info
    VkRenderPassCreateInfo create_render_pass_create_info(std::array<VkAttachmentDescription, 3>& attachments,
                                                          const VkAttachmentDescription& color_attachment,
                                                          const VkAttachmentDescription& color_resolver_attachment,
                                                          const VkAttachmentDescription& depth_attachment,
                                                          const VkSubpassDescription& subpass_description,
                                                          const VkSubpassDependency& subpass_dependency) {
        attachments[0] = color_attachment;
        attachments[1] = color_resolver_attachment;
        attachments[2] = depth_attachment;

        VkRenderPassCreateInfo render_pass_create_info {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        render_pass_create_info.pAttachments = attachments.data();
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass_description;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &subpass_dependency;

        return render_pass_create_info;
    }

    render_pass::render_pass(const device& device,
                             const physical_device& physical_device,
                             VkFormat image_format,
                             performance_settings performance_settings,
                             std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
            _log_manager(log_manager) {
        this->_log_manager->log_message("Creating render pass...", logging::log_levels::info);

        this->_msaa_samples = graphics::get_msaa_samples(performance_settings,
                                                         physical_device.get_max_msaa_samples());

        // color
        auto color_attachment_create_info = create_attachment_description(image_format,
                                                                          this->_msaa_samples,
                                                                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        auto color_attachment_reference = create_attachment_reference(0,
                                                                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        // color resolver
        auto color_resolver_attachment_create_info = create_attachment_description(image_format,
                                                                                   VK_SAMPLE_COUNT_1_BIT,
                                                                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        auto color_resolver_attachment_reference = create_attachment_reference(1,
                                                                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        // depth
        auto depth_format = this->get_depth_format(physical_device);
        if (!depth_format) {
            FATAL_ERROR("Failed to get depth format.")
        }

        this->_depth_format = *depth_format;

        auto depth_attachment_create_info = create_attachment_description(this->_depth_format,
                                                                          this->_msaa_samples,
                                                                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        auto depth_attachment_reference = create_attachment_reference(2,
                                                                      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        // subpass
        auto subpass_description = create_subpass_description(color_attachment_reference,
                                                              color_resolver_attachment_reference,
                                                              depth_attachment_reference);

        auto subpass_dependency = create_subpass_dependency();

        // render pass
        std::array<VkAttachmentDescription, 3> attachments;
        auto create_info = create_render_pass_create_info(attachments,
                                                          color_attachment_create_info,
                                                          color_resolver_attachment_create_info,
                                                          depth_attachment_create_info,
                                                          subpass_description,
                                                          subpass_dependency);

        if (vkCreateRenderPass(device.get_native_handle(),
                               &create_info,
                               nullptr,
                               &this->_render_pass) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create render pass.")
        }

        this->_log_manager->log_message("Created render pass.", logging::log_levels::info);
    }

    std::optional<VkFormat> render_pass::get_depth_format(const physical_device& physical_device) const noexcept {
        auto format = physical_device.query_supported_image_tiling_format(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        return format;
    }

    render_pass::~render_pass() {
        if (this->_render_pass) {
            vkDestroyRenderPass(this->_device.get_native_handle(),
                                this->_render_pass,
                                nullptr);
            this->_render_pass = nullptr;
        }
    }
}
