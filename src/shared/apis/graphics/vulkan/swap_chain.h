#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "device.h"
#include "physical_device.h"
#include "window_surface.h"
#include "shared/apis/windowing/iapplication_window.h"
#include "shared/apis/graphics/performance_settings.h"

#include <memory>

namespace pbr::shared::apis::graphics::vulkan {
    /// Manages the swap chain
    class swap_chain {
    public:
        /// Constructs this swap chain
        /// \param device The logical device
        /// \param physical_device The physical device
        /// \param surface The window surface
        /// \param performance_settings The performance settings to use
        /// \param application_window The application window to setup against
        /// \param old_swap_chain_handle A handle to the previous swap chain if the swap chain is being recreated
        /// \param log_manager The log manager to use
        swap_chain(device& device,
                   physical_device& physical_device,
                   window_surface& surface,
                   performance_settings performance_settings,
                   std::shared_ptr<windowing::iapplication_window> application_window,
                   VkSwapchainKHR old_swap_chain_handle,
                   std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this swap chain
        ~swap_chain();

        /// Returns the native handle to this swap chain
        /// \returns The native handle to this swap chain
        [[nodiscard]]
        VkSwapchainKHR get_native_handle() const noexcept {
            return this->_swap_chain;
        }

    private:
        /// The logical device
        device& _device;

        /// The swap chain
        VkSwapchainKHR _swap_chain {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
