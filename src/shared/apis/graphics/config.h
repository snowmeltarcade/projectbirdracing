#pragma once

#include "shared/data/data_manager.h"
#include "shared/apis/logging/ilog_manager.h"
#include "apis.h"

#include <cassert>
#include <memory>
#include <exception>

namespace pbr::shared::apis::graphics {
    /// The graphics configuration settings
    class config {
    public:
        /// Creates the config
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        /// \param config_path The path to the config file from the data path configured in the data manager
        config(std::shared_ptr<data::data_manager> data_manager,
               std::shared_ptr<logging::ilog_manager> log_manager,
               std::filesystem::path config_path = config::graphics_config_path) {
            assert((data_manager));

            if (!this->load(data_manager, log_manager, config_path)) {
                log_manager->log_message("Failed to load graphics config.",
                                         logging::log_levels::error,
                                         "Graphics");

                throw std::runtime_error("Invalid graphics config.");
            }
        }

        /// Returns the graphics api to use
        /// \returns The graphics api to use
        apis api() const noexcept {
            return this->_api;
        }

        /// Sets the graphics api to use
        /// \param The api to set
        void set_api(apis api) noexcept {
            this->_api = api;
        }

    private:
        /// The path to the graphics config
        static const std::filesystem::path graphics_config_path;

        /// Loads the settings from the passed data manager
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        /// \param config_path The path to the config file from the data path configured in the data manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool load(const std::shared_ptr<data::data_manager>& data_manager,
                  const std::shared_ptr<logging::ilog_manager>& log_manager,
                  const std::filesystem::path& config_path) noexcept;

        /// The graphics api to use
        apis _api;
    };
}
