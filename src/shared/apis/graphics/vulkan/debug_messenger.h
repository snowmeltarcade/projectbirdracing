#pragma once

#include "shared/apis/logging/ilog_manager.h"

#include <memory>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// Handles Vulkan debug messages. This is a static class.
    class debug_messenger {
    public:
        debug_messenger() = delete;
        ~debug_messenger() = default;

        /// Gets the create info to use this messenger
        /// \param The log manager to log the messages to
        /// \returns The create info
        static VkDebugUtilsMessengerCreateInfoEXT get_create_info(
            std::shared_ptr<apis::logging::ilog_manager>& log_manager) noexcept;

    private:
        /// The callback Vulkan calls when a debug message is reported
        /// \param severity The severity of the message
        /// \param type The message type
        /// \param callback_data Any data associated with the message
        /// \param user_data Any set user data. In our case, this will be the `ilog_manager`
        static VKAPI_ATTR VkBool32 VKAPI_CALL callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                       VkDebugUtilsMessageTypeFlagsEXT type,
                                                       const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                       void* user_data);
    };
}
