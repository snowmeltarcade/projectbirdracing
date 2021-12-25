#pragma once

#include "instance.h"
#include "window_surface.h"

#include <memory>

namespace pbr::shared::apis::graphics::vulkan {
    /// Manages creating and destroying the physical graphics device
    class physical_device {
    public:
        /// Holds the indexes of the queue families for a physical device
        struct queue_family_indexes {
            /// The graphics queue index, if present
            std::optional<int> graphics_family_index;

            /// The present family index, if present
            std::optional<int> present_family_index;
        };

        /// Constructs this physical device
        /// \param instance The Vulkan instance
        /// \param window_surface The window surface
        /// \param log_manager The log manager to use
        physical_device(instance& instance,
                        window_surface& window_surface,
                        std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this surface
        ~physical_device();

        /// Returns the queue family indexes for this physical device
        /// \returns The queue family indexes for this physical device
        [[nodiscard]]
        queue_family_indexes get_queue_family_indexes() const noexcept {
            return this->_queue_family_indexes;
        }

        /// Returns the maximum MSAA samples supported by this physical device
        /// \returns The maximum MSAA samples supported by this physical device
        [[nodiscard]]
        VkSampleCountFlagBits get_max_msaa_samples() const noexcept {
            return this->_max_msaa_samples;
        }

    private:
        /// The Vulkan instance
        instance& _instance;

        /// The window surface
        window_surface& _window_surface;

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;

        /// The physical graphics device
        VkPhysicalDevice _physical_device {VK_NULL_HANDLE};

        /// The queue family indexes for this device
        queue_family_indexes _queue_family_indexes;

        /// The maximum MSAA samples for this device
        VkSampleCountFlagBits _max_msaa_samples {VK_SAMPLE_COUNT_1_BIT};
    };
}
