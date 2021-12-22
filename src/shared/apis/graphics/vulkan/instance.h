#pragma once

#include "shared/apis/graphics/application_information.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <string>
#include <cstdint>

namespace pbr::shared::apis::graphics::vulkan {
    /// A Vulkan instance
    class instance {
    public:
        /// Constructs this instance
        instance() = default;

        /// Destroys this instance
        ~instance() = default;

        instance(const instance&) = delete;
        instance(instance&&) = delete;

        /// Initializes this Vulkan instance
        /// \param window The SDL window to present to
        /// \param application_information Needed application information
        [[nodiscard]]
        bool initialize(SDL_Window* window,
                        application_information application_information);

    private:
        /// Constructs the `VkApplicationInfo` struct
        /// \param application_information Needed application information
        VkApplicationInfo construct_application_info(application_information application_information);

        /// The SDL window
        SDL_Window* _window {nullptr};

        /// A handle to this instance
        //class VkInstance _instance {VK_NULL_HANDLE};
    };
}
