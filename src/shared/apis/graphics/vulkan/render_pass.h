#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/graphics/performance_settings.h"
#include "device.h"
#include "physical_device.h"
#include "image.h"

#include <memory>

namespace pbr::shared::apis::graphics::vulkan {
    /// A render pass used in a swap chain. By default, this will have one color attachment
    /// to present, one color resolver for MSAA and one depth attachment
    class render_pass {
    public:
        /// Constructs this render pass
        /// \param device The logical device
        /// \param physical_device The physical device
        /// \param image_format The image format of the attachments
        /// \param performance_settings The performance settings to use
        /// \param log_manager The log manager to use
        render_pass(const device& device,
                    const physical_device& physical_device,
                    VkFormat image_format,
                    performance_settings performance_settings,
                    std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this render pass
        ~render_pass();

        /// Returns the native handle to this render pass
        /// \returns The native handle to this render pass
        [[nodiscard]]
        VkRenderPass get_native_handle() const noexcept {
            return this->_render_pass;
        }

        /// Returns the format of the depth buffer
        /// \returns The format of the depth buffer
        [[nodiscard]]
        VkFormat get_depth_format() const noexcept {
            return this->_depth_format;
        }

        /// Returns the number of MSAA samples
        /// \returns The number of MSAA samples
        [[nodiscard]]
        VkSampleCountFlagBits get_msaa_samples() const noexcept {
            return this->_msaa_samples;
        }

    private:
        /// The logical device
        const device& _device;

        /// The render pass
        VkRenderPass _render_pass {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;

        /// The format of the depth buffer
        VkFormat _depth_format;

        /// The number of MSAA samples
        VkSampleCountFlagBits _msaa_samples;

        /// Returns the image format for the depth attachment
        /// \param physical_device The physical device to query
        /// \returns The image format for the depth attachment if found, else empty
        std::optional<VkFormat> get_depth_format(const physical_device& physical_device) const noexcept;
    };
}
