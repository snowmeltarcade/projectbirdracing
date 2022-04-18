#include "vma.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    /// Creates an allocator create info struct
    /// \param instance The Vulkan instance
    /// \param physical_device The physical device to handle
    /// \param device The logical device
    /// \returns The allocator create info struct
    VmaAllocatorCreateInfo create_allocator_create_info(const instance& instance,
                                                        const physical_device& physical_device,
                                                        const device& device) noexcept {
        VmaAllocatorCreateInfo create_info {};
        create_info.vulkanApiVersion = instance.get_api_version();
        create_info.instance = instance.get_native_handle();
        create_info.physicalDevice = physical_device.get_native_handle();
        create_info.device = device.get_native_handle();

        return create_info;
    }

    vma::vma(const instance& instance,
             const physical_device& physical_device,
             const device& device,
             std::shared_ptr<logging::ilog_manager> log_manager)
         : _log_manager(log_manager) {
        this->_log_manager->log_message("Creating Vulkan Memory Allocator...",
                                        logging::log_levels::info,
                                        "Vulkan");

        auto create_info = create_allocator_create_info(instance,
                                                        physical_device,
                                                        device);

        if (vmaCreateAllocator(&create_info, &this->_allocator) != VK_SUCCESS) {
            FATAL_ERROR("Failed to create Vulkan Memory Allocator.")
        }

        this->_log_manager->log_message("Created Vulkan Memory Allocator.",
                                        logging::log_levels::info,
                                        "Vulkan");
    }

    vma::~vma() {
        if (this->_allocator) {
            vmaDestroyAllocator(this->_allocator);
            this->_allocator = nullptr;
        }
    }

    VmaAllocationCreateInfo vma::create_allocation_create_info(VmaMemoryUsage usage) noexcept {
        VmaAllocationCreateInfo create_info {};
        create_info.usage = usage;

        return create_info;
    }
}
