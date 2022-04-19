#include "command_pool.h"
#include "command_buffer.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    VkCommandPoolCreateInfo create_command_pool_create_info(uint32_t queue_index) noexcept {
        VkCommandPoolCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        create_info.queueFamilyIndex = queue_index;
        create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        return create_info;
    }

    command_pool::command_pool(const device& device,
                               uint32_t queue_index,
                               std::shared_ptr<logging::ilog_manager> log_manager)
       : _device(device),
         _log_manager(log_manager) {
        this->_log_manager->log_message("Creating the command pool for queue with index: " + std::to_string(queue_index),
                                        logging::log_levels::info,
                                        "Vulkan");

        auto create_info = create_command_pool_create_info(queue_index);

        if (vkCreateCommandPool(device.get_native_handle(),
                                &create_info,
                                nullptr,
                                &this->_command_pool) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create command pool.")
        }

        this->_log_manager->log_message("Created the command pool for queue with index: " + std::to_string(queue_index),
                                        logging::log_levels::info,
                                        "Vulkan");
    }

    command_pool::~command_pool() {
        if (this->_command_pool) {
            vkDestroyCommandPool(this->_device.get_native_handle(),
                                 this->_command_pool,
                                 nullptr);
            this->_command_pool = nullptr;
        }
    }
}
