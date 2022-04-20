#pragma once

#include "shared/data/data_manager.h"
#include "shared/apis/logging/ilog_manager.h"
#include "resolution.h"

#include <cassert>
#include <memory>
#include <exception>
#include <vector>

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

        /// Returns the loaded resolutions
        /// \returns The loaded resolutions
        [[nodiscard]]
        std::vector<resolution> resolutions() const noexcept {
            return this->_resolutions;
        }

        /// Returns the default resolution
        [[nodiscard]]
        resolution default_resolution() const noexcept {
            return this->_resolutions[this->_default_resolution_index];
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

        /// Reads the resolutions from the config
        /// \param settings The settings to read from
        void read_resolutions(data::settings& settings) noexcept;

        /// The resolutions
        std::vector<resolution> _resolutions;

        /// The index of the default resolution
        uint32_t _default_resolution_index {0u};
    };
}
