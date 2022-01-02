#pragma once

#include "command_pool.h"
#include "queue.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// A command buffer
    class command_buffer {
    public:
        /// Creates this command buffer
        /// \param device The logical device
        /// \param command_pool The command buffer to create from
        /// \param log_manager The log manager to use
        command_buffer(const device& device,
                       const command_pool& command_pool,
                       std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destructs this command buffer
        ~command_buffer();

        /// Returns the native handle to this command buffer
        /// \returns The native handle to this command buffer
        [[nodiscard]]
        VkCommandBuffer get_native_handle() const noexcept {
            return this->_buffer;
        }

        /// Sets up this buffer to accept commands to be executed just once
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool begin_one_time_usage() noexcept;

        /// Submits commands. Assumes this call was preceded by `begin_one_time_usage()`
        /// \param graphics_queue The graphics queue to submit these commands to
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool end_one_time_usage(const queue& graphics_queue) noexcept;

        /// Sets up this buffer to record for multiple executions
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool begin_record() noexcept;

        /// Ends the buffer recording
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool end_record() noexcept;

        /// Resets this command buffer
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool reset() noexcept;

    private:
        /// The logical device
        const device& _device;

        /// The command poll
        const command_pool& _command_pool;

        /// The command buffer
        VkCommandBuffer _buffer {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;

        /// Creates a command buffer allocate info struct
        /// \returns The command buffer allocate info struct
        VkCommandBufferAllocateInfo create_command_buffer_allocate_info() const noexcept;

        /// Creates a submit info struct
        /// \returns The submit info struct
        VkSubmitInfo create_submit_info() const noexcept;
    };
}
