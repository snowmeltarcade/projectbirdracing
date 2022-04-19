#pragma once

#include "shared/data/data_manager.h"
#include "shared/apis/logging/ilog_manager.h"

#include <cassert>
#include <memory>
#include <exception>

namespace pbr::shared::apis::windowing {
    /// The windowing configuration settings
    class config final {
    public:
        /// Creates the config
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        /// \param config_path The path to the config file from the data path configured in the data manager
        config(std::shared_ptr<data::data_manager> data_manager,
               std::shared_ptr<logging::ilog_manager> log_manager,
               std::filesystem::path config_path = config::window_config_path) {
            assert((data_manager));

            if (!this->load(data_manager, log_manager, config_path)) {
                log_manager->log_message("Failed to load window config.",
                                         logging::log_levels::error,
                                         "Window");

                throw std::runtime_error("Invalid window config.");
            }
        }

    private:
        /// The path to the window config
        static const std::filesystem::path window_config_path;

        /// Loads the settings from the passed data manager
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        /// \param config_path The path to the config file from the data path configured in the data manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool load(const std::shared_ptr<data::data_manager>& data_manager,
                  const std::shared_ptr<logging::ilog_manager>& log_manager,
                  const std::filesystem::path& config_path) noexcept;
    };
}
