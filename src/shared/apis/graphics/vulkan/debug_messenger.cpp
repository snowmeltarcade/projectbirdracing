#include "debug_messenger.h"

#include <sstream>

namespace pbr::shared::apis::graphics::vulkan {
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger::get_create_info(
        std::shared_ptr<apis::logging::ilog_manager>& log_manager) noexcept {

        VkDebugUtilsMessengerCreateInfoEXT create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        create_info.pfnUserCallback = &debug_messenger::callback;

        create_info.pUserData = static_cast<void*>(&log_manager);

        return create_info;
    }

    apis::logging::log_levels to_log_level(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
        switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
                return apis::logging::log_levels::info;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
                return apis::logging::log_levels::info;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                return apis::logging::log_levels::warning;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                return apis::logging::log_levels::warning;
            }
            default: {
                // we are not using the version of Vulkan we think we are...
                return apis::logging::log_levels::fatal;
            }
        }
    }

    std::string to_string(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
        switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
                return "Verbose";
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
                return "Info";
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                return "Warning";
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                return "Error";
            }
            default: {
                return "Unknown: " + std::to_string(static_cast<uint32_t>(severity));
            }
        }
    }

    std::string to_string(VkDebugUtilsMessageTypeFlagsEXT type) {
        switch (type) {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
                return "General";
            }
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
                return "Validation";
            }
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
                return "Performance";
            }
            default: {
                return "Unknown: " + std::to_string(static_cast<uint32_t>(type));
            }
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger::callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                             VkDebugUtilsMessageTypeFlagsEXT type,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                             void* user_data) {
        auto logger = *static_cast<std::shared_ptr<apis::logging::ilog_manager>*>(user_data);

        std::stringstream ss;
        ss << to_string(severity)
            << " - "
            << to_string(type)
            << "> "
            << callback_data->pMessage
            << '\n';

        logger->log_message(ss.str(), to_log_level(severity), "Vulkan Messenger");

        return VK_FALSE;
    }
}
