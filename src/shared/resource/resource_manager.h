#pragma once

#include "shared/apis/logging/log_manager.h"
#include "shared/data/data_manager.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>

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
        resource_manager() = default;

        /// Destroys this resource manager
        virtual ~resource_manager() = default;

        /// Returns a resource
        /// \param name The name of the resource to get
        /// \returns The item to get. If not found, returns `nullptr`
        [[nodiscard]]
        std::shared_ptr<T> get(const std::string& name) noexcept {
            if (!this->_resources.contains(name)) {
                auto loaded_resource = this->load(name);
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
        }

    protected:
        /// This loads a resource
        /// \returns The loaded resource
        [[nodiscard]]
        virtual std::shared_ptr<T> load(const std::filesystem::path& path) noexcept = 0;

    private:
        /// The resources
        std::unordered_map<std::string, std::shared_ptr<T>> _resources;

        /// The filepaths from the names
        std::unordered_map<std::string, std::filesystem::path> _paths;

        /// Loads the paths from the resource descriptor file
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool load_paths() noexcept { return true; }
    };
}
