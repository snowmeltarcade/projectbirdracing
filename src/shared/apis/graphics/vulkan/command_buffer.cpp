#include "command_buffer.h"

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
                                   const command_pool &command_pool)
        : _device(device),
            _command_pool(command_pool) {
        auto allocate_info = create_command_buffer_allocate_info();

        vkAllocateCommandBuffers(this->_device.get_native_handle(),
                                 &allocate_info,
                                 &this->_buffer);
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

    void command_buffer::begin_one_time_usage() noexcept {
        auto begin_info = create_begin_info();

        vkBeginCommandBuffer(this->_buffer, &begin_info);
    }

    void command_buffer::end_one_time_usage(const queue& graphics_queue) noexcept {
        vkEndCommandBuffer(this->_buffer);

        auto submit_info = this->create_submit_info();

        vkQueueSubmit(graphics_queue.get_native_handle(),
                      1,
                      &submit_info,
                      VK_NULL_HANDLE);

        vkQueueWaitIdle(graphics_queue.get_native_handle());
    }

    VkSubmitInfo command_buffer::create_submit_info() const noexcept {
        VkSubmitInfo submit_info {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &this->_buffer;

        return submit_info;
    }
}
