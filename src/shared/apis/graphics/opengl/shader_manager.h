#pragma once

#include "shared/resource/resource_manager.h"
#include "shader.h"

namespace pbr::shared::apis::graphics::opengl {
    /// Manages the creation and deletion of shaders
    class shader_manager final : public resource::resource_manager<shader> {
    public:
        /// Creates this shader manager
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        /// \param list_path The path to the resource list from the `data` directory
        shader_manager(const std::shared_ptr<data::data_manager>& data_manager,
                       const std::shared_ptr<logging::ilog_manager>& log_manager,
                       const std::filesystem::path& list_path)
                       : resource::resource_manager<shader>(data_manager, log_manager, list_path),
                           _data_manager(data_manager),
                           _log_manager(log_manager) {
            assert((this->_data_manager));
            assert((this->_log_manager));
        }

        /// Destroys this shader manager
        ~shader_manager() override = default;

    protected:
        /// This loads a resource
        /// \param path The path os the resource
        /// \returns The loaded resource
        [[nodiscard]]
        std::shared_ptr<shader> load(const std::filesystem::path& path) noexcept override;

        /// The data manager
        std::shared_ptr<data::data_manager> _data_manager;

        /// The log manager
        std::shared_ptr<logging::ilog_manager> _log_manager;
    };
}
