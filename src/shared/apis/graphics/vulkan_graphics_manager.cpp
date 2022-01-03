#include "vulkan_graphics_manager.h"
#include "shared/apis/windowing/application_window.h"

#include <cstdlib>
#include <SDL_vulkan.h>

using namespace pbr::shared::apis;

namespace pbr::shared::apis::graphics {
    bool vulkan_graphics_manager::load_api(const std::filesystem::path& executable_path) noexcept {
        this->_log_manager->log_message("Loading the graphics API...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        this->set_environment_variables(executable_path);

        if (SDL_VideoInit(nullptr) < 0) {
            this->_log_manager->log_message("Failed to init SDL with error:",
                                            apis::logging::log_levels::error,
                                            "Graphics");
            this->_log_manager->log_message(SDL_GetError(),
                                            apis::logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        this->_log_manager->log_message("Loaded the graphics API.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    void vulkan_graphics_manager::set_environment_variables(const std::filesystem::path& executable_path) const noexcept {
#if defined(DEBUG) && defined(REQUIRES_MOLTEN_VK)
        this->_log_manager->log_message("Setting `VK_LAYER_PATH` to " + std::string(VK_LAYER_PATH),
                                        apis::logging::log_levels::info,
                                        "Graphics");
        setenv("VK_LAYER_PATH", VK_LAYER_PATH, 0);
#endif

        auto icd_path = executable_path / VK_ICD_FILENAMES;
        this->_log_manager->log_message("Setting `VK_ICD_FILENAMES` to " + std::string(icd_path),
                                        apis::logging::log_levels::info,
                                        "Graphics");
        setenv("VK_ICD_FILENAMES", icd_path.c_str(), 0);
    }

    bool vulkan_graphics_manager::initialize() noexcept {
        this->_log_manager->log_message("Initializing the graphics manager...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        auto application_window = std::dynamic_pointer_cast<windowing::application_window>(
            this->_window_manager->get_main_application_window());
        assert((application_window));

        if (!this->_instance.initialize(application_window->get_native_handle(),
                                        this->_application_information)) {
            this->_log_manager->log_message("Failed to initialize Vulkan instance.",
                                            logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        this->_window_surface = std::make_unique<vulkan::window_surface>(this->_instance,
                                                                         application_window,
                                                                         this->_log_manager);

        this->_physical_device = std::make_unique<vulkan::physical_device>(this->_instance,
                                                                           *this->_window_surface,
                                                                           this->_log_manager);

        this->_device = std::make_unique<vulkan::device>(this->_instance,
                                                         *this->_physical_device,
                                                         this->_log_manager);

        this->_vma = std::make_unique<vulkan::vma>(this->_instance,
                                                   *this->_physical_device,
                                                   *this->_device,
                                                   this->_log_manager);

        auto queue_indexes = this->_physical_device->get_queue_family_indexes();

        this->_graphics_queue = std::make_unique<vulkan::queue>(*this->_device,
                                                                *queue_indexes.graphics_family_index,
                                                                this->_log_manager);

        this->_present_queue = std::make_unique<vulkan::queue>(*this->_device,
                                                               *queue_indexes.present_family_index,
                                                               this->_log_manager);

        this->_command_pool = std::make_unique<vulkan::command_pool>(*this->_device,
                                                                     *queue_indexes.graphics_family_index,
                                                                     this->_log_manager);

        if (!this->refresh_resources()) {
            this->_log_manager->log_message("Failed to refresh resources.",
                                            apis::logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        auto number_of_swap_chain_views = this->_swap_chain->get_image_views().size();

        this->_command_buffers.reserve(number_of_swap_chain_views);
        for (auto i {0u}; i < number_of_swap_chain_views; ++i) {
            this->_command_buffers.emplace_back(*this->_device,
                                                *this->_command_pool,
                                                this->_log_manager);
        }

        // this requires a valid swap chain, which is crated in `refresh_resources()`
        if (!this->create_swap_chain_synchronization_objects()) {
            this->_log_manager->log_message("Failed to create swap chain synchronization objects.",
                                            apis::logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        this->_log_manager->log_message("Initialized the graphics manager.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    bool vulkan_graphics_manager::refresh_resources() noexcept {
        // wait until the driver has finished using everything
        vkDeviceWaitIdle(this->_device->get_native_handle());

        VkSwapchainKHR old_swap_chain_handle = {VK_NULL_HANDLE};

        if (this->_swap_chain) {
            old_swap_chain_handle = this->_swap_chain->get_native_handle();
            this->_swap_chain.reset();
        }

        this->cleanup_resources();

        this->_swap_chain = std::make_unique<vulkan::swap_chain>(*this->_device,
                                                                 *this->_physical_device,
                                                                 *this->_window_surface,
                                                                 this->_performance_settings,
                                                                 this->_window_manager->get_main_application_window(),
                                                                 old_swap_chain_handle,
                                                                 this->_log_manager);

        this->_render_pass = std::make_unique<vulkan::render_pass>(*this->_device,
                                                                   *this->_physical_device,
                                                                   this->_swap_chain->get_image_format(),
                                                                   this->_performance_settings,
                                                                   this->_log_manager);

        this->_framebuffer = std::make_unique<vulkan::framebuffer>(*this->_device,
                                                                   *this->_vma,
                                                                   *this->_render_pass,
                                                                   *this->_swap_chain,
                                                                   *this->_command_pool,
                                                                   *this->_graphics_queue,
                                                                   this->_log_manager);

        this->_render_system_screen_aligned_2d = std::make_unique<render_system_screen_aligned_2d>(*this->_device,
                                                                                                   *this->_vma,
                                                                                                   *this->_render_pass,
                                                                                                   *this->_command_pool,
                                                                                                   *this->_graphics_queue,
                                                                                                   this->_swap_chain->get_extent(),
                                                                                                   this->_swap_chain->get_image_views().size(),
                                                                                                   this->_render_pass->get_msaa_samples(),
                                                                                                   this->_log_manager);

        return true;
    }

    bool vulkan_graphics_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the graphics API...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        vkDeviceWaitIdle(this->_device->get_native_handle());

        this->cleanup_resources();

        this->_command_buffers.clear();

        this->_command_pool.reset();

        this->_vma.reset();

        this->_image_available_semaphores.clear();
        this->_render_finished_semaphores.clear();
        this->_in_flight_fences.clear();

        this->_device.reset();

        this->_physical_device.reset();

        this->_window_surface.reset();

        SDL_VideoQuit();

        this->_log_manager->log_message("Shut down the graphics API.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    void vulkan_graphics_manager::cleanup_resources() noexcept {
        this->_log_manager->log_message("Cleaning up graphics resources...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        this->_framebuffer.reset();

        this->_render_pass.reset();

        this->_swap_chain.reset();

        this->_render_system_screen_aligned_2d.reset();

        this->_log_manager->log_message("Cleaned up graphics resources.",
                                        apis::logging::log_levels::info,
                                        "Graphics");
    }

    bool vulkan_graphics_manager::create_swap_chain_synchronization_objects() noexcept {
        auto max_frames = this->_performance_settings.max_frames_in_flight;
        this->_image_available_semaphores.reserve(max_frames);
        this->_render_finished_semaphores.reserve(max_frames);
        this->_in_flight_fences.reserve(max_frames);

        for (auto i {0u}; i < max_frames; ++i) {
            this->_image_available_semaphores.emplace_back(*this->_device, this->_log_manager);
            this->_render_finished_semaphores.emplace_back(*this->_device, this->_log_manager);
            this->_in_flight_fences.emplace_back(*this->_device, this->_log_manager);
        }

        auto swap_chain_images_count = this->_swap_chain->get_image_views().size();
        this->_images_in_flight.resize(swap_chain_images_count, VK_NULL_HANDLE);

        return true;
    }

    void vulkan_graphics_manager::submit_renderable_entities(renderable_entities renderable_entities) noexcept {
        std::scoped_lock<std::mutex> lock(this->_submit_renderable_entities_mutex);

        this->_renderable_entities = renderable_entities;
    }

    void vulkan_graphics_manager::submit_frame_for_render() noexcept {
        // wait until any blocking actions to finish, so we can start a render request
        auto in_flight_fence = this->_in_flight_fences[this->_current_frame].get_native_handle();

        vkWaitForFences(this->_device->get_native_handle(),
                        1,
                        &in_flight_fence,
                        VK_TRUE,
                        UINT64_MAX);

        // get the next image to render into
        auto image_index {0u};
        auto result = vkAcquireNextImageKHR(this->_device->get_native_handle(),
                                            this->_swap_chain->get_native_handle(),
                                            UINT64_MAX,
                                            this->_image_available_semaphores[this->_current_frame].get_native_handle(),
                                            VK_NULL_HANDLE,
                                            &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            if (!this->refresh_resources()) {
                this->_log_manager->log_message("Failed to refresh resources.",
                                                apis::logging::log_levels::error,
                                                "Vulkan");
                return;
            }
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            this->_log_manager->log_message("Failed to acquire next image.",
                                            apis::logging::log_levels::error,
                                            "Vulkan");
            return;
        }

        // check if a previous frame is using this image (i.e. there is its fence to wait on)
        if (this->_images_in_flight[image_index] != VK_NULL_HANDLE) {
            vkWaitForFences(this->_device->get_native_handle(),
                            1,
                            &this->_images_in_flight[image_index],
                            VK_TRUE,
                            UINT64_MAX);
        }

        // mark the image as now being in use by this frame
        this->_images_in_flight[image_index] = this->_in_flight_fences[this->_current_frame].get_native_handle();

        VkSemaphore wait_semaphores[] {
            this->_image_available_semaphores[this->_current_frame].get_native_handle(),
        };

        VkPipelineStageFlags wait_stages[] {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        if (!this->create_render_entities_command_buffers(image_index)) {
            this->_log_manager->log_message("Failed to create render entities command buffers.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return;
        }

        auto command_buffer_to_submit = this->_command_buffers[image_index].get_native_handle();

        // submit the frame
        VkSubmitInfo submit_info;
        memset(&submit_info, 0, sizeof(submit_info));
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer_to_submit;

        VkSemaphore signal_semaphores[] {
            this->_render_finished_semaphores[this->_current_frame].get_native_handle(),
        };

        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        auto fence_to_reset = this->_in_flight_fences[this->_current_frame].get_native_handle();

        vkResetFences(this->_device->get_native_handle(),
                      1,
                      &fence_to_reset);

        // submit to the graphics queue
        if (vkQueueSubmit(this->_graphics_queue->get_native_handle(),
                          1,
                          &submit_info,
                          this->_in_flight_fences[this->_current_frame].get_native_handle()) != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to submit to graphics queue.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return;
        }

        VkPresentInfoKHR present_info {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swap_chains[] {
            this->_swap_chain->get_native_handle(),
        };

        present_info.swapchainCount = 1;
        present_info.pSwapchains = swap_chains;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;

        result = vkQueuePresentKHR(this->_present_queue->get_native_handle(),
                                   &present_info);

        if (result == VK_ERROR_OUT_OF_DATE_KHR ||
            result == VK_SUBOPTIMAL_KHR ||
            this->_signal_swap_chain_out_of_date) {
            if (!this->refresh_resources()) {
                this->_log_manager->log_message("Failed to refresh resources.",
                                                apis::logging::log_levels::error,
                                                "Vulkan");
                return;
            }
        } else if (result != VK_SUCCESS) {
            this->_log_manager->log_message("Failed to present queue.",
                                            apis::logging::log_levels::error,
                                            "Vulkan");
            return;
        }

        // advance to the next frame to render
        this->_current_frame = (this->_current_frame + 1) % this->_performance_settings.max_frames_in_flight;
    }

    bool vulkan_graphics_manager::create_render_entities_command_buffers(uint32_t image_index) noexcept {
        // copy the data to avoid keeping the lock longer than needed
        renderable_entities renderable_entities;
        {
            std::scoped_lock<std::mutex> lock(this->_submit_renderable_entities_mutex);
            renderable_entities = this->_renderable_entities;
        }

        auto& buffer = this->_command_buffers[image_index];

        if (!buffer.begin_record()) {
            this->_log_manager->log_message("Failed to begin command buffer record.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        // this is to stop a warning from being logged about the framebuffer image being in the wrong format
        // when actual rendering code starts, this can be removed
        VkRenderPassBeginInfo render_pass_info {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = this->_render_pass->get_native_handle();
        render_pass_info.framebuffer = this->_framebuffer->get_native_handle(image_index);
        render_pass_info.renderArea.offset = { 0, 0 };
        render_pass_info.renderArea.extent = this->_swap_chain->get_extent();

        std::array<VkClearValue, 3> clearValues {};
        // this must be in the same order as the attachments in the framebuffer & render pass
        clearValues[0].color = {{ 1.0f, 1.0f, 0.0f, 1.0f }};
        clearValues[1].color = {{ 1.0f, 1.0f, 0.0f, 1.0f }};
        clearValues[2].depthStencil = { 1.0f, 0 };

        render_pass_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
        render_pass_info.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(buffer.get_native_handle(),
                             &render_pass_info,
                             VK_SUBPASS_CONTENTS_INLINE);

        this->_render_system_screen_aligned_2d->build_render_commands(buffer, image_index);

        vkCmdEndRenderPass(buffer.get_native_handle());

        if (!buffer.end_record()) {
            this->_log_manager->log_message("Failed to end command buffer record.",
                                            logging::log_levels::error,
                                            "Vulkan");
            return false;
        }

        return true;
    }
}
