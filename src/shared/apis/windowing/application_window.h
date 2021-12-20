#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "iapplication_window.h"

#include <cassert>
#include <string>
#include <SDL.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace pbr::shared::apis::windowing {
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
        glm::vec2 texCoord2;

        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }

        static VkVertexInputBindingDescription get_binding_description() {
            VkVertexInputBindingDescription binding_description{};
            binding_description.binding = 0;
            binding_description.stride = sizeof(Vertex);
            binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return binding_description;
        }

        static std::array<VkVertexInputAttributeDescription, 4> get_attribute_descriptions() {
            std::array<VkVertexInputAttributeDescription, 4> attribute_descriptions{};

            attribute_descriptions[0].binding = 0;
            attribute_descriptions[0].location = 0;
            attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attribute_descriptions[0].offset = offsetof(Vertex, pos);

            attribute_descriptions[1].binding = 0;
            attribute_descriptions[1].location = 1;
            attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attribute_descriptions[1].offset = offsetof(Vertex, color);

            attribute_descriptions[2].binding = 0;
            attribute_descriptions[2].location = 2;
            attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attribute_descriptions[2].offset = offsetof(Vertex, texCoord);

            attribute_descriptions[3].binding = 0;
            attribute_descriptions[3].location = 3;
            attribute_descriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
            attribute_descriptions[3].offset = offsetof(Vertex, texCoord2);

            return attribute_descriptions;
        }
    };

    /// An application window can either be in windowed or full screen mode. This window will
    /// contain the render surface
    class application_window : public iapplication_window {
    public:
        /// Constructs this window
        /// \param log_manager The log manager to use
        /// \param title The window title
        /// \param w The width
        /// \param h The height
        application_window(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                           std::string_view title,
                           uint32_t w, uint32_t h)
                           : _log_manager(log_manager) {
            assert((this->_log_manager));

            if (!this->create(title,
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w, h)) {
                auto message = "Failed to create application window.";
                this->_log_manager->log_message(message, apis::logging::log_levels::fatal);
                throw std::logic_error(message);
            }
        }

        /// Destroys this window
        ~application_window() override {
            this->shutdown();
        }

        void update() override;

        void size_changed() override {
            this->_signal_swap_chain_out_of_date = true;
        }

    private:
        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The window instance
        SDL_Window* _window {nullptr};

        /// Creates this window
        /// \param title The window title
        /// \param x The x position
        /// \param y The y position
        /// \param w The width
        /// \param h The height
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create(std::string_view title,
                    uint32_t x, uint32_t y,
                    uint32_t w, uint32_t h) noexcept;

        /// Shuts down this window
        void shutdown() noexcept;

        void cleanup_swap_chain();
        bool create_swap_chain();

        void create_uniform_buffers();
        bool create_descriptor_pool();
        bool create_descriptor_sets();

        bool create_texture_image();
        void createImage(uint32_t width,
                         uint32_t height,
                         uint32_t mip_levels,
                         VkSampleCountFlagBits samples_count,
                         VkFormat format,
                         VkImageTiling tiling,
                         VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties,
                         VkImage* image,
                         VkDeviceMemory* imageMemory);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mip_levels);
        bool create_vertex_buffer(VkDevice device,
                                  VkPhysicalDevice physical_device,
                                  VkBuffer* vertex_buffer,
                                  VkDeviceMemory* vertex_buffer_memory,
                                  const std::vector<Vertex>& vertices);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        bool create_index_buffer(VkDevice device,
                                 VkPhysicalDevice physical_device,
                                 VkBuffer* index_buffer,
                                 VkDeviceMemory* index_buffer_memory,
                                 const std::vector<uint32_t>& indices);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels);
        bool createTextureImageView();
        void createTextureSampler();
        void createDepthResources();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat findDepthFormat();
        bool create_render_pass(VkFormat swap_chain_image_format, VkDevice device, VkRenderPass* render_pass);
        bool create_swap_chain_framebuffers(std::vector<VkFramebuffer>& swap_chain_framebuffers,
                                            const std::vector<VkImageView>& swap_chain_image_views,
                                            VkRenderPass render_pass, VkExtent2D swap_chain_extent,
                                            VkDevice device);
        void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        VkSampleCountFlagBits getMaxUsableSampleCount();
        void create_sample_resources();
        bool create_graphics_pipeline(VkDevice device,
                                      VkExtent2D swap_chain_extent,
                                      VkPipelineLayout* pipeline_layout,
                                      VkRenderPass render_pass,
                                      VkPipeline* graphics_pipeline,
                                      VkDescriptorSetLayout* descriptor_set_layout);

        void load_image(std::string path, VkImage* image, VkDeviceMemory* image_memory, uint32_t& mip_levels);

        void load_model();

        void update_uniform_buffer(uint32_t current_image_index);

        VkInstance _vulkan_instance {nullptr};
        VkDebugUtilsMessengerEXT _debug_messenger {nullptr};
        VkSurfaceKHR _surface {VK_NULL_HANDLE};
        VkPhysicalDevice _physical_device {VK_NULL_HANDLE};
        VkDevice _device {VK_NULL_HANDLE};
        VkQueue _graphics_queue {VK_NULL_HANDLE};
        VkQueue _present_queue {VK_NULL_HANDLE};
        VkSwapchainKHR _swap_chain {VK_NULL_HANDLE};
        std::vector<VkImage> _swap_chain_images;
        std::vector<VkImageView> _swap_chain_image_views;
        VkFormat _swap_chain_format {VkFormat::VK_FORMAT_UNDEFINED};
        VkExtent2D _swap_chain_extent;
        VkRenderPass _render_pass {VK_NULL_HANDLE};
        VkDescriptorSetLayout _descriptor_set_layout {VK_NULL_HANDLE};
        VkPipelineLayout _pipeline_layout {VK_NULL_HANDLE};
        VkPipeline _graphics_pipeline {VK_NULL_HANDLE};
        std::vector<VkFramebuffer> _swap_chain_framebuffers;
        VkCommandPool _command_pool {VK_NULL_HANDLE};
        std::vector<VkCommandBuffer> _command_buffers;
        std::vector<VkSemaphore> _image_available_semaphores;
        std::vector<VkSemaphore> _render_finished_semaphores;
        std::vector<VkFence> _in_flight_fences;
        std::vector<VkFence> _images_in_flight;
        uint32_t _current_frame {0u};
        bool _signal_swap_chain_out_of_date {false};
        VkBuffer _vertex_buffer {VK_NULL_HANDLE};
        VkDeviceMemory _vertex_buffer_memory {VK_NULL_HANDLE};
        VkBuffer _index_buffer {VK_NULL_HANDLE};
        VkDeviceMemory _index_buffer_memory {VK_NULL_HANDLE};
        std::vector<VkBuffer> _uniform_buffers;
        std::vector<VkDeviceMemory> _uniform_buffers_memory;
        VkDescriptorPool _descriptor_pool {VK_NULL_HANDLE};
        std::vector<VkDescriptorSet> _descriptor_sets;

        uint32_t _mip_levels1 {1};
        VkImage _texture_image {VK_NULL_HANDLE};
        VkDeviceMemory _texture_image_memory {VK_NULL_HANDLE};
        VkImageView _texture_image_view {VK_NULL_HANDLE};

        uint32_t _mip_levels2 {1};
        VkImage _texture_image2 {VK_NULL_HANDLE};
        VkDeviceMemory _texture_image_memory2 {VK_NULL_HANDLE};
        VkImageView _texture_image_view2 {VK_NULL_HANDLE};
        VkSampler _texture_sampler {VK_NULL_HANDLE};
        VkImage _depth_image {VK_NULL_HANDLE};
        VkDeviceMemory _depth_image_memory {VK_NULL_HANDLE};
        VkImageView _depth_image_view {VK_NULL_HANDLE};

        uint32_t _mip_levels3 {1};
        VkImage _texture_image3 {VK_NULL_HANDLE};
        VkDeviceMemory _texture_image_memory3 {VK_NULL_HANDLE};
        VkImageView _texture_image_view3 {VK_NULL_HANDLE};

        std::vector<Vertex> _model_verticies;
        std::vector<uint32_t> _model_indices;
        VkBuffer _model_vertex_buffer {VK_NULL_HANDLE};
        VkDeviceMemory _model_vertex_buffer_memory {VK_NULL_HANDLE};
        VkBuffer _model_index_buffer {VK_NULL_HANDLE};
        VkDeviceMemory _model_index_buffer_memory {VK_NULL_HANDLE};

        // the max value wants to be set by the user/graphics level or something like that...
        VkSampleCountFlagBits _msaa_samples = VK_SAMPLE_COUNT_1_BIT;

        VkImage _samples_image {VK_NULL_HANDLE};
        VkDeviceMemory _samples_image_memory {VK_NULL_HANDLE};
        VkImageView _samples_image_view {VK_NULL_HANDLE};
    };
}

namespace std {
    template<> struct hash<pbr::shared::apis::windowing::Vertex> {
        size_t operator()(pbr::shared::apis::windowing::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}
