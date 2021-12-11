#include "application_window.h"

#include <algorithm>
#include <SDL_vulkan.h>
#include <iostream>
#include <optional>
#include <set>
#include <fstream>

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

    struct swap_chain_support_details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> surface_formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    swap_chain_support_details query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface) {
        swap_chain_support_details details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        auto format_count {0u};
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

        details.surface_formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.surface_formats.data());

        auto present_modes_count {0u};
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_modes_count, nullptr);

        details.present_modes.resize(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_modes_count, details.present_modes.data());

        return details;
    }

    VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
        for (const auto& format : formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }

        return formats[0];
    }

    VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& modes) {
        for (const auto& mode : modes) {
            // triple buffering
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return mode;
            }
        }

        // this will always be available
        // is better for mobile devices (its basically vsync mode)
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D select_swap_chain_extent(const VkSurfaceCapabilitiesKHR& capabilities, SDL_Window* window) {
        // use the size Vulkan tells us to use
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            // looks like we can choose
            int width;
            int height;
            SDL_Vulkan_GetDrawableSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    bool is_physical_device_compatible(const VkPhysicalDevice device, VkSurfaceKHR surface) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        auto extensions_count {0u};
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, nullptr);

        // make sure the device supports the extensions we require
        std::vector<VkExtensionProperties> extensions;
        extensions.resize(extensions_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, extensions.data());

        std::set<std::string> required_extensions {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        // any extensions that are left in this set are not supported
        for (const auto& extension : extensions) {
            required_extensions.erase(std::string(extension.extensionName));
        }

        if (!required_extensions.empty()) {
            return false;
        }

        auto qfi = find_queue_families(device, surface);
        if (!qfi.graphics_family_index || !qfi.present_family_index) {
            return false;
        }

        auto swap_chain_details = query_swap_chain_support(device, surface);
        if (swap_chain_details.surface_formats.empty() || swap_chain_details.present_modes.empty()) {
            return false;
        }

        return true;
    }

    std::vector<char> read_all_bytes(const std::filesystem::path& path) {
        std::vector<char> bytes;

        std::ifstream fs(path, std::ios::ate | std::ios::binary);
        if (!fs.is_open()) {
            std::cerr << "Failed to open file: " << path << '\n';
            return {};
        }

        auto file_size = fs.tellg();
        bytes.resize(file_size);

        fs.seekg(0);
        fs.read(bytes.data(), file_size);

        fs.close();

        return bytes;
    }

    VkShaderModule create_shader_module(VkDevice device, const std::vector<char>& bytes) {
        VkShaderModuleCreateInfo create_info;
        memset(&create_info, 0, sizeof(create_info));
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = bytes.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(bytes.data());

        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
            std::cerr << "Failed to create shader module.\n";
            return VK_NULL_HANDLE;
        }

        return shader_module;
    }

    bool create_graphics_pipeline(VkDevice device) {
        auto vertex_shader_bytes = read_all_bytes("../../../data/vertex.vert.spv");
        auto fragment_shader_bytes = read_all_bytes("../../../data/fragment.frag.spv");

        auto vertex_shader = create_shader_module(device, vertex_shader_bytes);
        auto fragment_shader = create_shader_module(device, fragment_shader_bytes);

        VkPipelineShaderStageCreateInfo vertex_shader_stage_info;
        memset(&vertex_shader_stage_info, 0, sizeof(vertex_shader_stage_info));
        vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_shader_stage_info.module = vertex_shader;
        vertex_shader_stage_info.pName = "main";
        vertex_shader_stage_info.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo fragment_shader_stage_info;
        memset(&fragment_shader_stage_info, 0, sizeof(fragment_shader_stage_info));
        fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragment_shader_stage_info.module = fragment_shader;
        fragment_shader_stage_info.pName = "main";
        fragment_shader_stage_info.pSpecializationInfo = nullptr;

//        VkPipelineShaderStageCreateInfo shader_stages[] = {
//            vertex_shader_stage_info,
//            fragment_shader_stage_info,
//        };

        vkDestroyShaderModule(device, vertex_shader, nullptr);
        vkDestroyShaderModule(device, fragment_shader, nullptr);

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

        // application information
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

        // set up the debug callback
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
        std::vector<const char*> device_extensions {
            "VK_KHR_portability_subset",
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };
        device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();
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

        // create the swap chain
        auto swap_chain_details = query_swap_chain_support(this->_physical_device, this->_surface);
        auto surface_format = choose_surface_format(swap_chain_details.surface_formats);
        auto present_mode = choose_present_mode(swap_chain_details.present_modes);
        auto extent = select_swap_chain_extent(swap_chain_details.capabilities, this->_window);

        auto image_count = swap_chain_details.capabilities.minImageCount + 1;
        if (swap_chain_details.capabilities.maxImageCount > 0 && image_count > swap_chain_details.capabilities.maxImageCount) {
            image_count = swap_chain_details.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swap_chain_create_info;
        memset(&swap_chain_create_info, 0, sizeof(swap_chain_create_info));
        swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swap_chain_create_info.surface = this->_surface;
        swap_chain_create_info.minImageCount = image_count;
        swap_chain_create_info.imageFormat = surface_format.format;
        swap_chain_create_info.imageColorSpace = surface_format.colorSpace;
        swap_chain_create_info.imageExtent = extent;
        swap_chain_create_info.imageArrayLayers = 1;
        swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queue_family_indexes[] = { static_cast<uint32_t>(*qfi.graphics_family_index),
                                            static_cast<uint32_t>(*qfi.present_family_index) };

        if (qfi.graphics_family_index == qfi.present_family_index) {
            swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swap_chain_create_info.queueFamilyIndexCount = 0;
            swap_chain_create_info.pQueueFamilyIndices = nullptr;
        } else {
            swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swap_chain_create_info.queueFamilyIndexCount = 2;
            swap_chain_create_info.pQueueFamilyIndices = queue_family_indexes;
        }

        // maybe useful for when a mobile device changes orientation?
        swap_chain_create_info.preTransform = swap_chain_details.capabilities.currentTransform;

        swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swap_chain_create_info.presentMode = present_mode;
        swap_chain_create_info.clipped = VK_TRUE;
        swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(this->_device, &swap_chain_create_info, nullptr, &this->_swap_chain) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to create swap chain.", apis::logging::log_levels::error);
            return false;
        }

        auto swap_chain_image_count {0u};
        vkGetSwapchainImagesKHR(this->_device, this->_swap_chain, &swap_chain_image_count, nullptr);

        this->_swap_chain_images.resize(swap_chain_image_count);
        vkGetSwapchainImagesKHR(this->_device, this->_swap_chain, &swap_chain_image_count, this->_swap_chain_images.data());

        this->_swap_chain_format = swap_chain_create_info.imageFormat;
        this->_swap_chain_extent = swap_chain_create_info.imageExtent;

        this->_swap_chain_image_views.resize(this->_swap_chain_images.size());

        for (auto i {0u}; i < this->_swap_chain_images.size(); ++i)  {
            VkImageViewCreateInfo image_view_create_info;
            memset(&image_view_create_info, 0, sizeof(image_view_create_info));
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.image = _swap_chain_images[i];
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = this->_swap_chain_format;
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(this->_device, &image_view_create_info, nullptr, &_swap_chain_image_views[i]) != VK_SUCCESS) {
                this->_log_manager->log_message("Failed to create image view: " + std::to_string(i), apis::logging::log_levels::error);
                return false;
            }
        }

        if (!create_graphics_pipeline(this->_device)) {
            this->_log_manager->log_message("Failed to create graphics pipeline.", apis::logging::log_levels::error);
            return false;
        }

        return true;
    }

    void application_window::shutdown() noexcept {
        if (this->_vulkan_instance) {
            for (auto& view : this->_swap_chain_image_views) {
                vkDestroyImageView(this->_device, view, nullptr);
            }
            this->_swap_chain_image_views.clear();

            if (this->_swap_chain) {
                vkDestroySwapchainKHR(this->_device, this->_swap_chain, nullptr);
                this->_swap_chain = nullptr;
            }

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
