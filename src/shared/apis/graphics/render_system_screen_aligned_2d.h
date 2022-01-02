#pragma once

#include "vulkan/device.h"
#include "vulkan/vma.h"
#include "vulkan/buffer.h"
#include "shared/apis/logging/ilog_manager.h"

#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace pbr::shared::apis::graphics {
    struct ubo {
        // used for color fading etc...
        alignas(4) glm::vec4 color;
    };

    /// Handles rendering 2d entities that are screen aligned. The top left of the screen
    /// will be treated as position, and the bottom right will be treated as 1,1.
    class render_system_screen_aligned_2d {
    public:
        render_system_screen_aligned_2d(const vulkan::device& device,
                                        const vulkan::vma& vma,
                                        uint32_t number_of_swap_chain_images,
                                        std::shared_ptr<logging::ilog_manager> log_manager);
        ~render_system_screen_aligned_2d();

    private:
        const vulkan::device& _device;

        const vulkan::vma& _vma;

        std::shared_ptr<logging::ilog_manager> _log_manager;

        std::vector<vulkan::buffer> _ubo_buffers;

        VkDescriptorSetLayout _descriptor_set_layout {VK_NULL_HANDLE};
        VkDescriptorPool _descriptor_pool {VK_NULL_HANDLE};
        std::vector<VkDescriptorSet> _descriptor_sets;
    };
}
