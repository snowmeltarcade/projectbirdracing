#pragma once

#include "shared/data/data_manager.h"
#include "shared/apis/logging/ilog_manager.h"

#include <cassert>
#include <memory>

namespace pbr::shared::apis::graphics {
    /// The graphics configuration settings
    class config {
    public:
        /// Creates the config
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        config(std::shared_ptr<data::data_manager> data_manager,
               std::shared_ptr<logging::ilog_manager> log_manager) {
            assert((data_manager));

            if (!this->load(data_manager, log_manager)) {
                log_manager->log_message("Failed to load graphics settings.",
                                         logging::log_levels::error,
                                         "Graphics");
            }
        }

    private:
        /// Loads the settings from the passed data manager
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool load(const std::shared_ptr<data::data_manager>& data_manager,
                  const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept;
    };
}
