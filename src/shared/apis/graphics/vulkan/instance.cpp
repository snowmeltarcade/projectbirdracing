#include "instance.h"
#include "debug_messenger.h"

#include <cstdlib>
#include <iostream>
#include <cassert>

namespace pbr::shared::apis::graphics::vulkan {
    bool instance::initialize(SDL_Window* window,
                              application_information application_information) noexcept {
        assert((window != nullptr));

        this->set_environment_variables();

        this->_log_manager->log_message("Initializing Vulkan instance...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        this->_window = window;

        auto application_info = this->construct_application_info(application_information);

        auto extensions = this->get_extensions();

        auto validation_layers = this->get_validation_layers();
        if (!validation_layers) {
            this->_log_manager->log_message("Did not get required validation layers.",
                                            apis::logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        auto instance_create_info = this->construct_instance_create_info(
            application_info,
            extensions,
            *validation_layers);

#ifdef DEBUG
        std::array<VkValidationFeatureEnableEXT, 1> enables;
        VkValidationFeaturesEXT features {};
        this->enable_best_practices_validation_layer(instance_create_info, enables, features);
#endif

        auto debug_messenger_create_info = debug_messenger::get_create_info(
            this->_log_manager);
        features.pNext = &debug_messenger_create_info;

        if (vkCreateInstance(&instance_create_info, nullptr, &this->_instance) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create Vulkan instance.",
                                            apis::logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        this->_log_manager->log_message("Initialized Vulkan instance.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    void instance::set_environment_variables() const noexcept {
#if defined(DEBUG) && defined(REQUIRES_MOLTEN_VK)
        this->_log_manager->log_message("Setting `VK_LAYER_PATH` to " + std::string(VK_LAYER_PATH),
                                        apis::logging::log_levels::info,
                                        "Graphics");
        setenv("VK_LAYER_PATH", VK_LAYER_PATH, 0);

        this->_log_manager->log_message("Setting `VK_ICD_FILENAMES` to " + std::string(VK_ICD_FILENAMES),
                                        apis::logging::log_levels::info,
                                        "Graphics");
        setenv("VK_ICD_FILENAMES", VK_ICD_FILENAMES, 0);
#endif
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
        ai.apiVersion = VK_API_VERSION_1_1;

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

    std::optional<std::vector<const char*>> instance::get_validation_layers() const noexcept {
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
                                                                  const std::vector<const char*>& extensions,
                                                                  const std::vector<const char*>& validation_layers) const noexcept {
        VkInstanceCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &application_info;
        create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();
#ifdef RELEASE
        // we don't want any validation layers in release
        create_info.enabledLayerCount = 0u;
#else
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
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

        vkDestroyInstance(this->_instance, nullptr);
        this->_instance = nullptr;

        this->_log_manager->log_message("Shut down Vulkan instance.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }
}
