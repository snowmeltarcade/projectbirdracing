#pragma once

#include "device.h"
#include "vma.h"
#include "buffer.h"
#include "render_pass.h"
#include "command_buffer.h"
#include "command_pool.h"
#include "queue.h"
#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/graphics/renderable_entities.h"

#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace pbr::shared::apis::graphics {
    struct ubo {
        // used for color fading etc...
        alignas(4) glm::vec4 color;
    };

    struct vertex {
        glm::vec3 pos;
        glm::vec4 color;
    };

    /// Handles rendering 2d entities that are screen aligned. The top left of the screen
    /// will be treated as position 0,0, and the bottom right will be treated as 1,1.
    class render_system_screen_aligned_2d {
    public:
        render_system_screen_aligned_2d(const vulkan::device& device,
                                        const vulkan::vma& vma,
                                        const vulkan::render_pass& render_pass,
                                        const vulkan::command_pool& command_pool,
                                        const vulkan::queue& graphics_queue,
                                        VkExtent2D swap_chain_extent,
                                        uint32_t number_of_swap_chain_images,
                                        VkSampleCountFlagBits msaa_samples,
                                        std::shared_ptr<logging::ilog_manager> log_manager);
        ~render_system_screen_aligned_2d();

        void build_render_commands(vulkan::command_buffer& buffer, uint32_t image_index);

        void submit_renderable_entities(const renderable_entities& renderable_entities);

    private:
        const vulkan::device& _device;

        const vulkan::vma& _vma;

        const vulkan::command_pool& _command_pool;

        const vulkan::queue& _graphics_queue;

        std::shared_ptr<logging::ilog_manager> _log_manager;

        std::vector<vulkan::buffer> _ubo_buffers;

        std::vector<vertex> _vertices;
        std::unique_ptr<vulkan::buffer> _vertex_buffer;

        VkDescriptorSetLayout _descriptor_set_layout {VK_NULL_HANDLE};
        VkDescriptorPool _descriptor_pool {VK_NULL_HANDLE};
        std::vector<VkDescriptorSet> _descriptor_sets;

        VkPipelineLayout _pipeline_layout {VK_NULL_HANDLE};
        VkPipeline _graphics_pipeline {VK_NULL_HANDLE};

        void create_pipeline(VkExtent2D swap_chain_extent,
                             VkSampleCountFlagBits msaa_samples,
                             const vulkan::render_pass& render_pass);

        void create_vertex_buffer();
    };
}
