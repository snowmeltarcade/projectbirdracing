#include "application_window.h"

#include <algorithm>
#include <SDL_vulkan.h>
#include <iostream>
#include <optional>
#include <set>

namespace pbr::shared::apis::windowing {
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
        VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        // see: https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Validation_layers
        // under the heading 'Message Callback'

        auto logger = *static_cast<std::shared_ptr<apis::logging::ilog_manager>*>(pUserData);
        logger->log_message(pCallbackData->pMessage, apis::logging::log_levels::error);
        //std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    struct queue_family_indexes {
        std::optional<int> graphics_family_index;
        std::optional<int> present_family_index;
    };

    queue_family_indexes find_queue_families(const VkPhysicalDevice device, VkSurfaceKHR surface) {
        queue_family_indexes indexes;

        auto queue_family_count {0u};
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families;
        queue_families.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

        for (auto i {0u}; i < queue_families.size(); ++i) {
            VkBool32 surface_supported {false};
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surface_supported);

            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indexes.graphics_family_index = i;
            }

            if (surface_supported) {
                indexes.present_family_index = i;
            }
        }

        return indexes;
    }

    bool is_physical_device_compatible(const VkPhysicalDevice device, VkSurfaceKHR surface) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        auto qfi = find_queue_families(device, surface);
        if (!qfi.graphics_family_index || !qfi.present_family_index) {
            return false;
        }

        return true;
    }

    bool application_window::create(std::string_view title,
                                    uint32_t x, uint32_t y,
                                    uint32_t w, uint32_t h) noexcept {
        auto flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE |
                     SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

        this->_window = SDL_CreateWindow(std::string(title).c_str(),
                                         x, y,
                                         w, h,
                                         flags);

        if (!this->_window) {
            this->_log_manager->log_message("Failed to create SDL window.", apis::logging::log_levels::error);
            return false;
        }

        // application infomation
        VkApplicationInfo ai;
        memset(&ai, 0, sizeof(ai));
        ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ai.pApplicationName = "Project Bird Racing";
        ai.applicationVersion = VK_MAKE_API_VERSION(0, 0, 4, 0);
        ai.pEngineName = "PBR";
        ai.engineVersion = VK_MAKE_API_VERSION(0, 0, 4, 0);
        ai.apiVersion = VK_API_VERSION_1_1;

        // extensions
        auto extensions_count {0u};
        SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, nullptr);

        std::vector<const char*> extensions;
        extensions.resize(extensions_count);

        SDL_Vulkan_GetInstanceExtensions(this->_window, &extensions_count, extensions.data());

#ifdef DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        // validation layers
        std::vector<const char*> validation_layers {
            "VK_LAYER_KHRONOS_validation",
        };

        auto validation_layer_count {0u};
        vkEnumerateInstanceLayerProperties(&validation_layer_count, nullptr);

        std::vector<VkLayerProperties> available_validation_layers;
        available_validation_layers.resize(validation_layer_count);

        vkEnumerateInstanceLayerProperties(&validation_layer_count, available_validation_layers.data());

        std::vector<const char*> available_validation_layers_for_compare;
        for (const auto& l : available_validation_layers) {
            available_validation_layers_for_compare.push_back(l.layerName);
        }

        std::sort(validation_layers.begin(), validation_layers.end(),
                  [](auto& l, auto& r){ return std::string(l) < std::string(r); });
        std::sort(available_validation_layers_for_compare.begin(), available_validation_layers_for_compare.end(),
                  [](auto& l, auto& r){ return std::string(l) < std::string(r); });

        auto are_validation_layers_supported = std::includes(available_validation_layers_for_compare.begin(), available_validation_layers_for_compare.end(),
                                                             validation_layers.begin(), validation_layers.end(),
                                                             [](auto& available_layer, auto& layer) {
            return std::string(available_layer) < std::string(layer);
        });

        if (!are_validation_layers_supported) {
            this->_log_manager->log_message("Requested layers are not supported.", apis::logging::log_levels::error);
            return false;
        }

        // enable the best practices validation layer
        // from: https://vulkan.lunarg.com/doc/sdk/1.2.198.1/windows/best_practices.html
        VkValidationFeatureEnableEXT enables[] = {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT};
        VkValidationFeaturesEXT features = {};
        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features.enabledValidationFeatureCount = 1;
        features.pEnabledValidationFeatures = enables;

        // create the Vulkan instance
        VkInstanceCreateInfo ii;
        memset(&ii, 0, sizeof(ii));
        ii.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        ii.pApplicationInfo = &ai;
        ii.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        ii.ppEnabledExtensionNames = extensions.data();
#ifdef RELEASE
        ii.enabledLayerCount = 0u;
#else
        ii.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        ii.ppEnabledLayerNames = validation_layers.data();
#endif
        ii.pNext = &features;

        if (vkCreateInstance(&ii, nullptr, &this->_vulkan_instance) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create Vulkan instance.", apis::logging::log_levels::error);
            return false;
        }

        // setup the debug callback
        VkDebugUtilsMessengerCreateInfoEXT create_info;
        memset(&create_info, 0, sizeof(create_info));
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debugCallback;
        create_info.pUserData = static_cast<void*>(&this->_log_manager);

        if (CreateDebugUtilsMessengerEXT(this->_vulkan_instance, &create_info, nullptr, &this->_debug_messenger) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create debug messenger.", apis::logging::log_levels::error);
            return false;
        }

        // create window surface
        if (!SDL_Vulkan_CreateSurface(this->_window, this->_vulkan_instance, &this->_surface)) {
            this->_log_manager->log_message("Failed to create surface.", apis::logging::log_levels::error);
            return false;
        }

        // select the physical device to use
        auto physical_device_count {0u};
        vkEnumeratePhysicalDevices(this->_vulkan_instance, &physical_device_count, nullptr);
        if (physical_device_count == 0) {
            this->_log_manager->log_message("Failed to find GPUs with Vulkan support.", apis::logging::log_levels::error);
            return false;
        }

        std::vector<VkPhysicalDevice> physical_devices;
        physical_devices.resize(physical_device_count);
        vkEnumeratePhysicalDevices(this->_vulkan_instance, &physical_device_count, physical_devices.data());

        for (const auto& device : physical_devices) {
            if (is_physical_device_compatible(device, this->_surface)) {
                this->_physical_device = device;
                break;
            }
        }

        if (this->_physical_device == VK_NULL_HANDLE) {
            this->_log_manager->log_message("Failed to find compatible device.", apis::logging::log_levels::error);
            return false;
        }

        // set up the logical device
        auto qfi = find_queue_families(this->_physical_device, this->_surface);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

        std::set<int> unique_queue_ids {
            *qfi.graphics_family_index,
            *qfi.present_family_index
        };

        for (const auto& queue_id : unique_queue_ids) {
            VkDeviceQueueCreateInfo queue_create_info;
            memset(&queue_create_info, 0, sizeof(queue_create_info));
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_id;
            queue_create_info.queueCount = 1;

            auto queue_priority = 1.0f;
            queue_create_info.pQueuePriorities = &queue_priority;

            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features;
        memset(&device_features, 0, sizeof(device_features));

        VkDeviceCreateInfo device_create_info;
        memset(&device_create_info, 0, sizeof(device_create_info));
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = queue_create_infos.data();
        device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pEnabledFeatures = &device_features;

        // do this check in real life:
        // vkCreateDevice: VK_KHR_portability_subset must be enabled because physical device VkPhysicalDevice 0x7fdc1b5ff9a0[] supports it The Vulkan spec states: If the VK_KHR_portability_subset extension is included in pProperties of vkEnumerateDeviceExtensionProperties, ppEnabledExtensions must include "VK_KHR_portability_subset" (https://vulkan.lunarg.com/doc/view/1.2.198.1/mac/1.2-extensions/vkspec.html#VUID-VkDeviceCreateInfo-pProperties-04451)
        const char* device_extension = "VK_KHR_portability_subset";
        device_create_info.enabledExtensionCount = 1;
        device_create_info.ppEnabledExtensionNames = &device_extension;
#ifdef RELEASE
        device_create_info.enabledLayerCount = 0u;
#else
        device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        device_create_info.ppEnabledLayerNames = validation_layers.data();
#endif

        if (vkCreateDevice(this->_physical_device, &device_create_info, nullptr, &this->_device) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create logical device.", apis::logging::log_levels::error);
            return false;
        }

        vkGetDeviceQueue(this->_device, *qfi.graphics_family_index, 0, &this->_graphics_queue);
        vkGetDeviceQueue(this->_device, *qfi.present_family_index, 0, &this->_present_queue);

        return true;
    }

    void application_window::shutdown() noexcept {
        if (this->_vulkan_instance) {
            if (this->_device) {
                vkDestroyDevice(this->_device, nullptr);
                this->_device = nullptr;
            }

            if (this->_debug_messenger) {
                DestroyDebugUtilsMessengerEXT(this->_vulkan_instance, this->_debug_messenger, nullptr);
                this->_debug_messenger = nullptr;
            }

            if (this->_surface) {
                vkDestroySurfaceKHR(this->_vulkan_instance, this->_surface, nullptr);
                this->_surface = nullptr;
            }

            vkDestroyInstance(this->_vulkan_instance, nullptr);
            this->_vulkan_instance = nullptr;
        }

        if (this->_window) {
            SDL_DestroyWindow(this->_window);
            this->_window = nullptr;
        }
    }
}
