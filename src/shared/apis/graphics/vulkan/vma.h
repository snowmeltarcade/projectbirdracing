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
        vma(instance& instance,
            physical_device& physical_device,
            device& device,
            std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this allocator
        ~vma();

    private:
        /// The allocator
        VmaAllocator _allocator {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
