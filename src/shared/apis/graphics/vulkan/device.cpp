#include "device.h"

#include <set>

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Creates a list of queue create information
    /// \param indexes The indexes of the queues
    /// \param queue_priority The priority of the queues - all queues will share the same priority for the time being
    /// \returns The queue create information
    std::vector<VkDeviceQueueCreateInfo> create_queue_create_infos(const physical_device::queue_family_indexes& indexes,
                                                                   const float& queue_priority) {
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

        // the queues could have the same id, so remove any duplicates
        std::set<int> unique_queue_ids {
            *indexes.graphics_family_index,
            *indexes.present_family_index,
        };

        for (const auto& queue_id : unique_queue_ids) {
            VkDeviceQueueCreateInfo queue_create_info {};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_id;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queue_priority;

            queue_create_infos.push_back(queue_create_info);
        }

        return queue_create_infos;
    }

    /// Creates the desired physical device features
    /// \returns The desired features
    VkPhysicalDeviceFeatures create_physical_device_features() {
        VkPhysicalDeviceFeatures device_features {};
        device_features.samplerAnisotropy = VK_TRUE;
        // aliases pixels within a polygon - will impact performance, so enable on higher graphics settings
        device_features.sampleRateShading = VK_TRUE;

        return device_features;
    }

    /// Creates the device create info
    /// \param device_features The desired physical device features
    /// \returns The device created info
    VkDeviceCreateInfo create_device_create_info(const VkPhysicalDeviceFeatures& device_features,
                                                 const std::vector<VkDeviceQueueCreateInfo>& queue_create_infos) {
        VkDeviceCreateInfo device_create_info {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = queue_create_infos.data();
        device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pEnabledFeatures = &device_features;

        return device_create_info;
    }

    device::device(instance& instance,
                   physical_device& physical_device,
                   std::shared_ptr<logging::ilog_manager> log_manager)
        : _log_manager(log_manager) {
        this->_log_manager->log_message("Creating the logical device...", logging::log_levels::info);

        auto queue_indexes = physical_device.get_queue_family_indexes();

        auto queue_priority = 1.0f;
        auto queue_create_infos = create_queue_create_infos(queue_indexes, queue_priority);

        auto device_features = create_physical_device_features();

        auto device_create_info = create_device_create_info(device_features, queue_create_infos);

        // do this check in real life:
        // vkCreateDevice: VK_KHR_portability_subset must be enabled because physical device
        // VkPhysicalDevice 0x7fdc1b5ff9a0[] supports it The Vulkan spec states: If the VK_KHR_portability_subset
        // extension is included in pProperties of vkEnumerateDeviceExtensionProperties, ppEnabledExtensions must
        // include "VK_KHR_portability_subset"
        // (https://vulkan.lunarg.com/doc/view/1.2.198.1/mac/1.2-extensions/vkspec.html#VUID-VkDeviceCreateInfo-pProperties-04451)
        std::vector<const char*> device_extensions {
            "VK_KHR_portability_subset",
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();
#ifdef RELEASE
        device_create_info.enabledLayerCount = 0u;
#else
        auto validation_layers = instance.get_validation_layers();
        device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        device_create_info.ppEnabledLayerNames = validation_layers.data();
#endif

        if (vkCreateDevice(physical_device.get_native_handle(), &device_create_info, nullptr, &this->_device) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create logical device.")
        }

//        this->setup_vma();
//
//        vkGetDeviceQueue(this->_device, *qfi.graphics_family_index, 0, &this->_graphics_queue);
//        vkGetDeviceQueue(this->_device, *qfi.present_family_index, 0, &this->_present_queue);

        this->_log_manager->log_message("Created the logical device.", logging::log_levels::info);
    }

    device::~device() {
        if (this->_device) {
            vkDestroyDevice(this->_device, nullptr);
            this->_device = nullptr;
        }
    }
}
