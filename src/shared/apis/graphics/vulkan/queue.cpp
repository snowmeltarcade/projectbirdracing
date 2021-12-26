#include "queue.h"

namespace pbr::shared::apis::graphics::vulkan {
    queue::queue(device& device,
                 uint32_t index,
                 std::shared_ptr<logging::ilog_manager> log_manager)
        : _log_manager(log_manager) {
        this->_log_manager->log_message("Creating a queue with family index: " + std::to_string(index),
                                        logging::log_levels::info);

        vkGetDeviceQueue(device.get_native_handle(),
                         index,
                         0,
                         &this->_queue);

        this->_log_manager->log_message("Created a queue with family index: " + std::to_string(index),
                                        logging::log_levels::info);
    }

    queue::~queue() {
    }
}
