#pragma once

#include "instance.h"
#include "shared/apis/windowing/application_window.h"
#include "shared/apis/logging/ilog_manager.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <memory>

namespace pbr::shared::apis::graphics::vulkan {
    /// A renderable surface created from a window
    class window_surface {
    public:
        /// Constructs this surface
        /// \param instance The Vulkan instance
        /// \param window The window to create the surface from
        /// \param log_manager The log manager to use
        window_surface(instance& instance,
                       std::shared_ptr<windowing::application_window> window,
                       std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this surface
        ~window_surface();

    private:
        /// The Vulkan instance
        instance& _instance;

        /// The surface
        VkSurfaceKHR _surface {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
