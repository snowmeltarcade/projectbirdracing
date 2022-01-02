#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/graphics/application_information.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <cassert>
#include <string>
#include <cstdint>
#include <vector>
#include <optional>

namespace pbr::shared::apis::graphics::vulkan {
    /// A Vulkan instance
    class instance {
    public:
        /// Constructs this instance
        instance(std::shared_ptr<apis::logging::ilog_manager> log_manager)
            : _log_manager(log_manager)
        {
            assert((this->_log_manager));
        }

        /// Destroys this instance
        ~instance() {
            if (!this->shutdown()) {
                this->_log_manager->log_message("Failed to shutdown Vulkan instance.",
                                                apis::logging::log_levels::error,
                                                "Graphics");
            }
        }

        instance(const instance&) = delete;
        instance(instance&&) = delete;

        /// Initializes this Vulkan instance
        /// \param window The SDL window to present to
        /// \param application_information Needed application information
        [[nodiscard]]
        bool initialize(SDL_Window* window,
                        application_information application_information) noexcept;

        /// Returns the native Vulkan instance handle
        /// \returns The native Vulkan instance handle
        [[nodiscard]]
        const VkInstance& get_native_handle() const noexcept {
            return this->_instance;
        }

        /// Returns the version of Vulkan to use
        /// \returns The version of Vulkan to use
        [[nodiscard]]
        uint32_t get_api_version() const noexcept {
            return VK_API_VERSION_1_1;
        }

        /// Returns the validation layers this instance requested
        /// \returns The validation layers this instance requested
        [[nodiscard]]
        const std::vector<const char*>& get_validation_layers() const noexcept {
            return this->_validation_layers;
        }

    private:
        /// Constructs the `VkApplicationInfo` struct
        /// \param application_information Needed application information
        [[nodiscard]]
        VkApplicationInfo construct_application_info(application_information application_information) const noexcept;

        /// Returns available extensions
        /// \returns The available extensions
        [[nodiscard]]
        std::vector<const char*> get_extensions() const noexcept;

        /// Returns available validation layers
        /// \returns The available validation layers. If empty, the required validation layers are not supported
        [[nodiscard]]
        std::optional<std::vector<const char*>> query_validation_layers() const noexcept;

        /// Constructs the instance create info struct
        /// \param application_info The application info
        /// \param extensions The desired extensions
        [[nodiscard]]
        VkInstanceCreateInfo construct_instance_create_info(const VkApplicationInfo& application_info,
                                                            const std::vector<const char*>& extensions) const noexcept;

        /// Enabled the best practices validation layer on the passed instance create info
        /// See: https://vulkan.lunarg.com/doc/sdk/1.2.198.1/windows/best_practices.html
        /// We need to pass in `enabled` and `features` as they must be in scope when we create the instance
        /// \param info The create info to enable the best practices validation layer on
        /// \param enables An array to store the features to enable
        /// \param features The validation features struct
        void enable_best_practices_validation_layer(VkInstanceCreateInfo& info,
                                                    std::array<VkValidationFeatureEnableEXT, 1>& enables,
                                                    VkValidationFeaturesEXT& features) const noexcept;

        /// Shutdown this instance
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;

        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The SDL window
        SDL_Window* _window {nullptr};

        /// A handle to this instance
        VkInstance _instance {VK_NULL_HANDLE};

        /// The debug messenger
        VkDebugUtilsMessengerEXT _debug_messenger {VK_NULL_HANDLE};

        /// The validation layers requested
        std::vector<const char*> _validation_layers;
    };
}
