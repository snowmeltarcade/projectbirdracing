#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/data/data_manager.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <cassert>

namespace pbr::shared::resource {
    /// Manages resources that need allocating and deallocating. When a resource is
    /// requested, if it does not exist, it is loaded. When it is returned to the caller,
    /// a reference count to it is increased. When that resource is no longer needed,
    /// it is returned to this manager. A reference count to it is then decreased. When
    /// the reference count is equal to zero, it is deallocated. Resources are loaded
    /// using the data manager.
    template <class T>
    class resource_manager {
    public:
        /// Creates this resource manager
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        /// \param list_path The path to the resource list from the `data` directory
        resource_manager(const std::shared_ptr<data::data_manager>& data_manager,
                         const std::shared_ptr<apis::logging::ilog_manager>& log_manager,
                         const std::filesystem::path& list_path) {
            assert((data_manager));
            assert((log_manager));

            if (!this->load_list(data_manager, log_manager, list_path)) {
                log_manager->log_message("Failed to load config list with path: `" + list_path.generic_string(),
                                         apis::logging::log_levels::error,
                                         "Resource");
            }
        }

        /// Destroys this resource manager
        virtual ~resource_manager() = default;

        /// Returns a resource
        /// \param name The name of the resource to get
        /// \returns The item to get. If not found, returns `nullptr`
        [[nodiscard]]
        std::shared_ptr<T> get(const std::string& name) noexcept {
            if (!this->_resources.contains(name)) {
                auto path = this->_paths[name];
                auto loaded_resource = this->load(path);

                this->_resources[name] = loaded_resource;
            }

            return this->_resources[name];
        }

        /// Frees the passed resource. If all references to this resource as freed, this
        /// resource will be destructed - thus, only call this function when you are sure
        /// you no longer want to use the resource. If the resource is freed, the passed
        /// ptr will be set to `nullptr`.
        /// \param resource The resource to free
        void free(std::shared_ptr<T>& resource) noexcept {
            assert((resource));
            // TODO: Implement the rest of the resource manager in an appropriate PR
        }

    protected:
        /// This loads a resource
        /// \param path The path os the resource
        /// \returns The loaded resource
        [[nodiscard]]
        virtual std::shared_ptr<T> load(const std::filesystem::path& path) noexcept = 0;

    private:
        /// The resources
        std::unordered_map<std::string, std::shared_ptr<T>> _resources;

        /// The filepaths from the names
        std::unordered_map<std::string, std::filesystem::path> _paths;

        /// Loads the paths from the resource descriptor file
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        /// \param settings_path The path of the file to load
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool load_list(const std::shared_ptr<data::data_manager>& data_manager,
                       const std::shared_ptr<apis::logging::ilog_manager>& log_manager,
                       const std::filesystem::path& settings_path) noexcept {
            auto settings = data_manager->read_settings(settings_path);
            if (!settings) {
                log_manager->log_message("Failed to load settings at path: " + settings_path.generic_string(),
                                         apis::logging::log_levels::error,
                                         "Resource");
                return false;
            }

            auto resources = settings->get_as_settings_array("resources");
            if (!resources) {
                log_manager->log_message("Failed to find `resources` key for path: " + settings_path.generic_string(),
                                         apis::logging::log_levels::error,
                                         "Resource");
                return false;
            }

            for (auto& resource : *resources) {
                auto resource_name = resource.get("name");
                auto resource_path = resource.get("path");

                if (!resource_name || !resource_path) {
                    continue;
                }

                this->_paths[*resource_name] = *resource_path;
            }

            return true;
        }
    };
}
