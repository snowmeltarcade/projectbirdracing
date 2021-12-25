#include "physical_device.h"

#include <set>

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Details about how a swap chain is supported
    struct swap_chain_support_details {
        /// The supported capabilities
        VkSurfaceCapabilitiesKHR capabilities;

        /// The supported surface formats
        std::vector<VkSurfaceFormatKHR> surface_formats;

        /// The supported present modes
        std::vector<VkPresentModeKHR> present_modes;
    };

    /// Returns the swap chain support details for the passed device and surface
    /// \param device The physical device to query
    /// \param surface The window surface
    /// \returns The swap chain support details
    swap_chain_support_details query_swap_chain_support(VkPhysicalDevice device, const window_surface& surface) {
        swap_chain_support_details details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,
                                                  surface.get_native_handle(),
                                                  &details.capabilities);

        auto format_count {0u};
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,
                                             surface.get_native_handle(),
                                             &format_count,
                                             nullptr);

        details.surface_formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,
                                             surface.get_native_handle(),
                                             &format_count,
                                             details.surface_formats.data());

        auto present_modes_count {0u};
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                  surface.get_native_handle(),
                                                  &present_modes_count,
                                                  nullptr);

        details.present_modes.resize(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                  surface.get_native_handle(),
                                                  &present_modes_count,
                                                  details.present_modes.data());

        return details;
    }

    /// Gets the queue family indexes for the passed physical device
    /// \param device The physical device to query
    /// \param surface The window surface
    /// \returns The queue family indexes
    physical_device::queue_family_indexes query_queue_family_indexes(const VkPhysicalDevice device,
                                                                     const window_surface& surface) {
        physical_device::queue_family_indexes indexes;

        auto queue_family_count {0u};
        vkGetPhysicalDeviceQueueFamilyProperties(device,
                                                 &queue_family_count,
                                                 nullptr);

        std::vector<VkQueueFamilyProperties> queue_families;
        queue_families.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device,
                                                 &queue_family_count,
                                                 queue_families.data());

        for (auto i {0u}; i < queue_families.size(); ++i) {
            VkBool32 surface_supported {false};
            vkGetPhysicalDeviceSurfaceSupportKHR(device,
                                                 i,
                                                 surface.get_native_handle(),
                                                 &surface_supported);

            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indexes.graphics_family_index = i;
            }

            if (surface_supported) {
                indexes.present_family_index = i;
            }
        }

        return indexes;
    }

    /// Determines if a physical device is usable
    /// \param device The physical device to check
    /// \param surface The window surface to render to
    bool is_physical_device_compatible(const VkPhysicalDevice device, const window_surface& surface) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        auto extensions_count {0u};
        vkEnumerateDeviceExtensionProperties(device,
                                             nullptr,
                                             &extensions_count,
                                             nullptr);

        // make sure the device supports the extensions we require
        std::vector<VkExtensionProperties> extensions;
        extensions.resize(extensions_count);
        vkEnumerateDeviceExtensionProperties(device,
                                             nullptr,
                                             &extensions_count,
                                             extensions.data());

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

        auto swap_chain_details = query_swap_chain_support(device, surface);
        if (swap_chain_details.surface_formats.empty() || swap_chain_details.present_modes.empty()) {
            return false;
        }

        auto qfi = query_queue_family_indexes(device, surface);
        if (!qfi.graphics_family_index || !qfi.present_family_index) {
            return false;
        }

        if (!features.samplerAnisotropy) {
            return false;
        }

        return true;
    }

    /// Returns the maximum supported MSAA samples for the passed device
    /// \param device The physical device to query
    /// \returns The maximum supported MSAA samples for the passed device
    VkSampleCountFlagBits query_max_msaa_samples(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties properties {};
        vkGetPhysicalDeviceProperties(device, &properties);

        VkSampleCountFlags counts = properties.limits.framebufferColorSampleCounts &
            properties.limits.framebufferDepthSampleCounts;

        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    physical_device::physical_device(instance& instance,
                                     window_surface& window_surface,
                                     std::shared_ptr<logging::ilog_manager> log_manager)
        : _instance(instance),
          _window_surface(window_surface),
          _log_manager(log_manager) {
        this->_log_manager->log_message("Creating physical device...", logging::log_levels::info);

        auto physical_device_count {0u};
        vkEnumeratePhysicalDevices(this->_instance.get_native_handle(),
                                   &physical_device_count,
                                   nullptr);
        if (physical_device_count == 0) {
            FATAL_ERROR("Failed to find GPUs with Vulkan support.");
        }

        std::vector<VkPhysicalDevice> physical_devices;
        physical_devices.resize(physical_device_count);
        vkEnumeratePhysicalDevices(this->_instance.get_native_handle(),
                                   &physical_device_count,
                                   physical_devices.data());

        for (const auto& device : physical_devices) {
            if (is_physical_device_compatible(device, this->_window_surface)) {
                this->_physical_device = device;
                break;
            }
        }

        if (this->_physical_device == VK_NULL_HANDLE) {
            FATAL_ERROR("Failed to find compatible physical graphics device.")
        }

        this->_queue_family_indexes = query_queue_family_indexes(this->_physical_device, this->_window_surface);
        this->_max_msaa_samples = query_max_msaa_samples(this->_physical_device);


//
//        // set up the logical device
//        auto qfi = find_queue_families(this->_physical_device, this->_surface);
//
//        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
//
//        std::set<int> unique_queue_ids {
//            *qfi.graphics_family_index,
//            *qfi.present_family_index
//        };
//
//        for (const auto& queue_id : unique_queue_ids) {
//            VkDeviceQueueCreateInfo queue_create_info;
//            memset(&queue_create_info, 0, sizeof(queue_create_info));
//            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//            queue_create_info.queueFamilyIndex = queue_id;
//            queue_create_info.queueCount = 1;
//
//            auto queue_priority = 1.0f;
//            queue_create_info.pQueuePriorities = &queue_priority;
//
//            queue_create_infos.push_back(queue_create_info);
//        }
//
//        VkPhysicalDeviceFeatures device_features;
//        memset(&device_features, 0, sizeof(device_features));
//        device_features.samplerAnisotropy = VK_TRUE;
//        // aliases pixels within a polygon - will impact performance, so enable on higher graphics settings
//        device_features.sampleRateShading = VK_TRUE;
//
//        VkDeviceCreateInfo device_create_info;
//        memset(&device_create_info, 0, sizeof(device_create_info));
//        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//        device_create_info.pQueueCreateInfos = queue_create_infos.data();
//        device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
//        device_create_info.pEnabledFeatures = &device_features;
//
//        // do this check in real life:
//        // vkCreateDevice: VK_KHR_portability_subset must be enabled because physical device VkPhysicalDevice 0x7fdc1b5ff9a0[] supports it The Vulkan spec states: If the VK_KHR_portability_subset extension is included in pProperties of vkEnumerateDeviceExtensionProperties, ppEnabledExtensions must include "VK_KHR_portability_subset" (https://vulkan.lunarg.com/doc/view/1.2.198.1/mac/1.2-extensions/vkspec.html#VUID-VkDeviceCreateInfo-pProperties-04451)
//        std::vector<const char*> device_extensions {
//            "VK_KHR_portability_subset",
//            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
//        };
//        device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
//        device_create_info.ppEnabledExtensionNames = device_extensions.data();
//#ifdef RELEASE
//        device_create_info.enabledLayerCount = 0u;
//#else
//        device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
//        device_create_info.ppEnabledLayerNames = validation_layers.data();
//#endif
//
//        if (vkCreateDevice(this->_physical_device, &device_create_info, nullptr, &this->_device) != VK_SUCCESS) {
//            this->_log_manager->log_message("Failed to create logical device.", apis::logging::log_levels::error);
//            return false;
//        }
//
//        this->setup_vma();
//
//        vkGetDeviceQueue(this->_device, *qfi.graphics_family_index, 0, &this->_graphics_queue);
//        vkGetDeviceQueue(this->_device, *qfi.present_family_index, 0, &this->_present_queue);

        this->_log_manager->log_message("Created physical device.", logging::log_levels::info);
    }

    physical_device::~physical_device() {

    }
}
