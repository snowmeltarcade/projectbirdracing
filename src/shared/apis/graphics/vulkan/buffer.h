#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "vma.h"

#include <memory>

namespace pbr::shared::apis::graphics::vulkan {
    /// Manages the lifetime of a `VkBuffer`
    class buffer {
    public:
        /// Creates this buffer
        /// \param vma The Vulkan Memory Allocator
        /// \param size The size of the buffer to create in bytes
        /// \param flags The usage flags
        /// \param memory_usage How this buffer's memory will be used
        /// \param log_manager The log manager to use
        buffer(const vma& vma,
               uint32_t size,
               VkBufferUsageFlags flags,
               VmaMemoryUsage memory_usage,
               std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destructs this command buffer
        ~buffer();

        /// Returns the native handle to this buffer
        /// \returns The native handle to this buffer
        [[nodiscard]]
        VkBuffer get_native_handle() const noexcept {
            return this->_buffer;
        }

    private:
        /// The Vulkan Memory Allocator
        const vma& _vma;

        /// The buffer
        VkBuffer _buffer {VK_NULL_HANDLE};

        /// The vma allocation
        VmaAllocation _allocation {VK_NULL_HANDLE};

        /// The log manager
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
