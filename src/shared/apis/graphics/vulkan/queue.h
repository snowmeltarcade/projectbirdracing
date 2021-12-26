#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "device.h"

#include <memory>

namespace pbr::shared::apis::graphics::vulkan {
    /// A queue - e.g. graphics, presentation etc...
    class queue {
    public:
        /// Constructs this queue
        /// \param device The logical device
        /// \param index The queue family index
        /// \param log_manager The log manager to use
        queue(device& device,
              uint32_t index,
              std::shared_ptr<logging::ilog_manager> log_manager);

        /// Destroys this physical device
        ~queue();

        /// Returns the native handle to this queue
        /// \returns The native handle to this queue
        [[nodiscard]]
        VkQueue get_native_handle() const noexcept {
            return this->_queue;
        }

    private:
        /// The queue
        VkQueue _queue {VK_NULL_HANDLE};

        /// The log manager to use
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
