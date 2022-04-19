#include "command_buffer.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    VkCommandBufferAllocateInfo command_buffer::create_command_buffer_allocate_info() const noexcept {
        VkCommandBufferAllocateInfo allocate_info {};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate_info.commandPool = this->_command_pool.get_native_handle();
        allocate_info.commandBufferCount = 1;

        return allocate_info;
    }

    command_buffer::command_buffer(const device& device,
                                   const command_pool &command_pool,
                                   std::shared_ptr<logging::ilog_manager> log_manager)
        : _device(device),
            _command_pool(command_pool),
            _log_manager(log_manager) {
        auto allocate_info = create_command_buffer_allocate_info();

        if (vkAllocateCommandBuffers(this->_device.get_native_handle(),
                                     &allocate_info,
                                     &this->_buffer) != VK_SUCCESS) {
            FATAL_ERROR("Failed to allocate command buffer.")
        }
    }

    command_buffer::~command_buffer() {
        if (this->_buffer) {
            vkFreeCommandBuffers(this->_device.get_native_handle(),
                                 this->_command_pool.get_native_handle(),
                                 1,
                                 &this->_buffer);
            this->_buffer = nullptr;
        }
    }

    /// Creates a buffer begin info struct
    /// \returns The buffer begin info struct
    VkCommandBufferBeginInfo create_begin_info() noexcept {
        VkCommandBufferBeginInfo begin_info {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        return begin_info;
    }

    bool command_buffer::begin_one_time_usage() noexcept {
        auto begin_info = create_begin_info();

        if (vkBeginCommandBuffer(this->_buffer, &begin_info) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to begin command buffer one time usage.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        return true;
    }

    bool command_buffer::end_one_time_usage(const queue& graphics_queue) noexcept {
        if (vkEndCommandBuffer(this->_buffer) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to end command buffer one time usage.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        auto submit_info = this->create_submit_info();

        if (vkQueueSubmit(graphics_queue.get_native_handle(),
                          1,
                          &submit_info,
                          VK_NULL_HANDLE) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to submit command buffer to queue.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        if (vkQueueWaitIdle(graphics_queue.get_native_handle()) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to wait for graphics queue to become idle.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        return true;
    }

    bool command_buffer::begin_record() noexcept {
        auto begin_info = create_begin_info();

        if (vkBeginCommandBuffer(this->_buffer, &begin_info) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to begin command buffer record.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        return true;
    }

    bool command_buffer::end_record() noexcept {
        if (vkEndCommandBuffer(this->_buffer) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to end command buffer record.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        return true;
    }

    bool command_buffer::reset() noexcept {
        if (!vkResetCommandBuffer(this->_buffer, 0)) {
            this->_log_manager->log_message("Failed to reset command buffer.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        return true;
    }

    VkSubmitInfo command_buffer::create_submit_info() const noexcept {
        VkSubmitInfo submit_info {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &this->_buffer;

        return submit_info;
    }
}
