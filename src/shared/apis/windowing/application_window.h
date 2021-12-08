#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "iapplication_window.h"

#include <cassert>
#include <string>
#include <SDL.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::windowing {
    /// An application window can either be in windowed or full screen mode. This window will
    /// contain the render surface
    class application_window : public iapplication_window {
    public:
        /// Constructs this window
        /// \param log_manager The log manager to use
        /// \param title The window title
        /// \param w The width
        /// \param h The height
        application_window(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                           std::string_view title,
                           uint32_t w, uint32_t h)
                           : _log_manager(log_manager) {
            assert((this->_log_manager));

            if (!this->create(title,
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w, h)) {
                auto message = "Failed to create application window.";
                this->_log_manager->log_message(message, apis::logging::log_levels::fatal);
                throw std::logic_error(message);
            }
        }

        /// Destroys this window
        ~application_window() override {
            this->shutdown();
        }

    private:
        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The window instance
        SDL_Window* _window {nullptr};

        /// Creates this window
        /// \param title The window title
        /// \param x The x position
        /// \param y The y position
        /// \param w The width
        /// \param h The height
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create(std::string_view title,
                    uint32_t x, uint32_t y,
                    uint32_t w, uint32_t h) noexcept;

        /// Shuts down this window
        void shutdown() noexcept;

        VkInstance _vulkan_instance {nullptr};
        VkDebugUtilsMessengerEXT _debug_messenger {nullptr};
        VkSurfaceKHR _surface {VK_NULL_HANDLE};
        VkPhysicalDevice _physical_device {VK_NULL_HANDLE};
        VkDevice _device {VK_NULL_HANDLE};
        VkQueue _graphics_queue {VK_NULL_HANDLE};
        VkQueue _present_queue {VK_NULL_HANDLE};
    };
}
