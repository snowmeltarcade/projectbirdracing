#include "vma.h"

#define FATAL_ERROR(message) \
    this->_log_manager->log_message(message, apis::logging::log_levels::fatal, "Vulkan"); \
    throw std::runtime_error(message);

namespace pbr::shared::apis::graphics::vulkan {
    vma::vma(instance& instance,
             physical_device& physical_device,
             device& device,
             std::shared_ptr<logging::ilog_manager> log_manager)
         : _log_manager(log_manager) {
        this->_log_manager->log_message("Creating Vulkan Memory Allocator...",
                                        logging::log_levels::info,
                                        "Vulkan");

        VmaAllocatorCreateInfo allocatorInfo {};
        allocatorInfo.vulkanApiVersion = instance.get_api_version();
        allocatorInfo.instance = instance.get_native_handle();
        allocatorInfo.physicalDevice = physical_device.get_native_handle();
        allocatorInfo.device = device.get_native_handle();

        if (vmaCreateAllocator(&allocatorInfo, &this->_allocator) != VK_SUCCESS) {
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
