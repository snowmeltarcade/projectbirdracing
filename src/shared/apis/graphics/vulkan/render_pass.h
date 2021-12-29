#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/graphics/performance_settings.h"
#include "device.h"
#include "physical_device.h"

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
        render_pass(device& device,
                    physical_device& physical_device,
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

    private:
        /// The logical device
        device& _device;

        /// The render pass
        VkRenderPass _render_pass {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;

        /// Returns the image format for the depth attachment
        /// \param physical_device The physical device to query
        /// \returns The image format for the depth attachment
        VkFormat get_depth_format(physical_device& physical_device);
    };
}
