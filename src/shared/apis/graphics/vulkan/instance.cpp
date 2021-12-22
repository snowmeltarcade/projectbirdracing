#include "instance.h"

#include <cassert>

namespace pbr::shared::apis::graphics::vulkan {
    bool instance::initialize(SDL_Window* window,
                              application_information application_information) {
    assert((window != nullptr));

    this->_window = window;

    auto application_info = this->construct_application_info(application_information);
    application_info.applicationVersion++;

    // extensions
    auto extensions_count {0u};
    SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, nullptr);

//        std::vector<const char*> extensions;
//        extensions.resize(extensions_count);
//
//        SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, extensions.data());
//
//#ifdef DEBUG
//        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//#endif
//
//        // validation layers
//        std::vector<const char*> validation_layers {
//            "VK_LAYER_KHRONOS_validation",
//        };
//
//        auto validation_layer_count {0u};
//        vkEnumerateInstanceLayerProperties(&validation_layer_count, nullptr);
//
//        std::vector<VkLayerProperties> available_validation_layers;
//        available_validation_layers.resize(validation_layer_count);
//
//        vkEnumerateInstanceLayerProperties(&validation_layer_count, available_validation_layers.data());
//
//        std::vector<const char*> available_validation_layers_for_compare;
//        for (const auto& l : available_validation_layers) {
//            available_validation_layers_for_compare.push_back(l.layerName);
//        }
//
//        std::sort(validation_layers.begin(), validation_layers.end(),
//                  [](auto& l, auto& r){ return std::string(l) < std::string(r); });
//        std::sort(available_validation_layers_for_compare.begin(), available_validation_layers_for_compare.end(),
//                  [](auto& l, auto& r){ return std::string(l) < std::string(r); });
//
//        auto are_validation_layers_supported = std::includes(available_validation_layers_for_compare.begin(), available_validation_layers_for_compare.end(),
//                                                             validation_layers.begin(), validation_layers.end(),
//                                                             [](auto& available_layer, auto& layer) {
//                                                                 return std::string(available_layer) < std::string(layer);
//                                                             });
//
//        if (!are_validation_layers_supported) {
//            this->_log_manager->log_message("Requested layers are not supported.", apis::logging::log_levels::error);
//            return false;
//        }
//
//        // enable the best practices validation layer
//        // from: https://vulkan.lunarg.com/doc/sdk/1.2.198.1/windows/best_practices.html
//        VkValidationFeatureEnableEXT enables[] = {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT};
//        VkValidationFeaturesEXT features = {};
//        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
//        features.enabledValidationFeatureCount = 1;
//        features.pEnabledValidationFeatures = enables;
//
//        // create the Vulkan instance
//        VkInstanceCreateInfo ii;
//        memset(&ii, 0, sizeof(ii));
//        ii.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//        ii.pApplicationInfo = &ai;
//        ii.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
//        ii.ppEnabledExtensionNames = extensions.data();
//#ifdef RELEASE
//        ii.enabledLayerCount = 0u;
//#else
//        ii.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
//        ii.ppEnabledLayerNames = validation_layers.data();
//#endif
//        ii.pNext = &features;
//
//        if (vkCreateInstance(&ii, nullptr, &this->_vulkan_instance) != VK_SUCCESS) {
//            this->_log_manager->log_message("Failed to create Vulkan instance.", apis::logging::log_levels::error);
//            return false;
//        }
    return true;
}

    VkApplicationInfo instance::construct_application_info(application_information application_information) {
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
}
