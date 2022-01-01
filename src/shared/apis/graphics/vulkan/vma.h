#pragma once

#include "instance.h"
#include "physical_device.h"
#include "device.h"

#include <memory>
#include <vk_mem_alloc.h>

namespace pbr::shared::apis::graphics::vulkan {
    /// Manages the setup and lifetime of the Vulkan Memory Allocator
    class vma {
    public:
        /// Constructs this allocator
        /// \param instance The Vulkan instance
        /// \param physical_device The physical device
        /// \param device The logical device
        /// \param log_manager The log manager to use
        vma(const instance& instance,
            const physical_device& physical_device,
            const device& device,
            std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this allocator
        ~vma();

        /// Returns the native handle to this allocator
        /// \returns The native handle to this allocator
        [[nodiscard]]
        VmaAllocator get_native_handle() const noexcept {
            return this->_allocator;
        }

        /// Creates an allocation create info
        /// \param How the memory will be used
        /// \returns The allocation create info
        [[nodiscard]]
        static VmaAllocationCreateInfo create_allocation_create_info(VmaMemoryUsage usage) noexcept;

    private:
        /// The allocator
        VmaAllocator _allocator {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
