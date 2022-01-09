#include "instance.h"
#include "debug_messenger.h"

#include <cassert>
#include <array>
#include <algorithm>

namespace pbr::shared::apis::graphics::vulkan {
    /// Create the debug messenger for the instance. This is needed because
    /// the `vkCreateDebugUtilsMessengerEXT` function is not exposed by default
    /// \param instance The Vulkan instance
    /// \param create_info The create info
    /// \param allocator An optional allocator
    /// \param debug_messenger The debug messenger to create
    VkResult create_debug_utils_messenger_ext(VkInstance instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                              const VkAllocationCallbacks* allocator,
                                              VkDebugUtilsMessengerEXT* debug_messenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, create_info, allocator, debug_messenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    /// Destroys the passed debug messenger. This is needed because the `vkDestroyDebugUtilsMessengerEXT`
    /// function is not natively exposed.
    /// \param instance The Vulkan instance
    /// \param debug_messenger The debug messenger to destroy
    /// \param allocator An optional allocator
    void destroy_debug_utils_messenger_ext(VkInstance instance,
                                           VkDebugUtilsMessengerEXT debug_messenger,
                                           const VkAllocationCallbacks* allocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debug_messenger, allocator);
        }
    }

    bool instance::initialize(SDL_Window* window,
                              application_information application_information) noexcept {
        assert((window != nullptr));

        this->_log_manager->log_message("Initializing Vulkan instance...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        this->_window = window;

        auto application_info = this->construct_application_info(application_information);

        auto extensions = this->get_extensions();

        auto validation_layers = this->query_validation_layers();
        if (!validation_layers) {
            this->_log_manager->log_message("Did not get required validation layers.",
                                            apis::logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        this->_validation_layers = *validation_layers;

        auto instance_create_info = this->construct_instance_create_info(
            application_info,
            extensions);

#ifdef DEBUG
        std::array<VkValidationFeatureEnableEXT, 1> enables;
        VkValidationFeaturesEXT features {};
        this->enable_best_practices_validation_layer(instance_create_info, enables, features);

        auto debug_messenger_create_info = debug_messenger::get_create_info(
            this->_log_manager);
        features.pNext = &debug_messenger_create_info;
#endif

        if (vkCreateInstance(&instance_create_info, nullptr, &this->_instance) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create Vulkan instance.",
                                            apis::logging::log_levels::error,
                                            "Graphics");
            return false;
        }

#ifdef DEBUG
        if (create_debug_utils_messenger_ext(this->_instance,
                                             &debug_messenger_create_info,
                                             nullptr,
                                             &this->_debug_messenger) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create debug messenger.",
                                            apis::logging::log_levels::error,
                                            "Vulkan");
            return false;
        }
#endif

        this->_log_manager->log_message("Initialized Vulkan instance.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    VkApplicationInfo instance::construct_application_info(application_information application_information) const noexcept {
        VkApplicationInfo ai {};
        ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ai.pApplicationName = std::string(application_information._application_name).c_str();
        ai.applicationVersion = VK_MAKE_API_VERSION(application_information._version_major,
                                                    application_information._version_minor,
                                                    application_information._version_build, 0);
        ai.pEngineName = std::string(application_information._engine_name).c_str();
        ai.engineVersion = ai.applicationVersion;
        ai.apiVersion = this->get_api_version();

        return ai;
    }

    std::vector<const char*> instance::get_extensions() const noexcept {
        auto extensions_count {0u};
        SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, nullptr);

        std::vector<const char*> extensions;
        extensions.resize(extensions_count);

        SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, extensions.data());

#ifdef DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        return extensions;
    }

    std::optional<std::vector<const char*>> instance::query_validation_layers() const noexcept {
        auto validation_layer_count {0u};
        vkEnumerateInstanceLayerProperties(&validation_layer_count, nullptr);

        std::vector<VkLayerProperties> available_validation_layers;
        available_validation_layers.resize(validation_layer_count);

        vkEnumerateInstanceLayerProperties(&validation_layer_count, available_validation_layers.data());

        std::vector<const char*> available_validation_layers_for_compare;
        for (const auto& l : available_validation_layers) {
            available_validation_layers_for_compare.push_back(l.layerName);
        }

        std::vector<const char*> desired_validation_layers {
#ifdef DEBUG
            // this layer is only used during debug
            "VK_LAYER_KHRONOS_validation",
#endif
        };

        if (desired_validation_layers.size() > 0) {
            std::sort(desired_validation_layers.begin(), desired_validation_layers.end(),
                      [](auto &l, auto &r) { return std::string(l) < std::string(r); });

            std::sort(available_validation_layers_for_compare.begin(), available_validation_layers_for_compare.end(),
                      [](auto &l, auto &r) { return std::string(l) < std::string(r); });

            auto are_desired_validation_layers_supported = std::includes(
                available_validation_layers_for_compare.begin(), available_validation_layers_for_compare.end(),
                desired_validation_layers.begin(), desired_validation_layers.end(),
                [](auto &available_layer, auto &layer) {
                    return std::string(available_layer) < std::string(layer);
                });

            if (!are_desired_validation_layers_supported) {
                this->_log_manager->log_message("Requested layers are not supported.",
                                                apis::logging::log_levels::error,
                                                "Graphics");
                return {};
            }
        }

        return desired_validation_layers;
    }

    VkInstanceCreateInfo instance::construct_instance_create_info(const VkApplicationInfo& application_info,
                                                                  const std::vector<const char*>& extensions) const noexcept {
        VkInstanceCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &application_info;
        create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();
#ifdef RELEASE
        // we don't want any validation layers in release
        create_info.enabledLayerCount = 0u;
#else
        create_info.enabledLayerCount = static_cast<uint32_t>(this->_validation_layers.size());
        create_info.ppEnabledLayerNames = this->_validation_layers.data();
#endif

        return create_info;
    }

    void instance::enable_best_practices_validation_layer(VkInstanceCreateInfo& info,
                                                          std::array<VkValidationFeatureEnableEXT, 1>& enables,
                                                          VkValidationFeaturesEXT& features) const noexcept {
        enables[0] = VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT;

        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features.enabledValidationFeatureCount = enables.size();
        features.pEnabledValidationFeatures = enables.data();

        info.pNext = &features;
    }

    bool instance::shutdown() noexcept {
        if (!this->_instance) {
            return true;
        }

        this->_log_manager->log_message("Shutting down Vulkan instance...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        if (this->_debug_messenger) {
            destroy_debug_utils_messenger_ext(this->_instance, this->_debug_messenger, nullptr);
            this->_debug_messenger = nullptr;
        }

        vkDestroyInstance(this->_instance, nullptr);
        this->_instance = nullptr;

        this->_log_manager->log_message("Shut down Vulkan instance.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }
}
