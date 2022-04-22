#pragma once

#include <memory>
#include <cassert>

namespace pbr::shared::resource {
    /// A reference counted resource item
    template <class T>
    class item {
    public:
        /// Creates this item
        item() = default;

        /// Destroys this item
        virtual ~item() = default;

        /// Increases the reference count
        void increase_reference() noexcept {
            ++this->_references;
        }

        /// Decreases the reference count
        /// \returns `true` if there are no references, else `false`
        [[nodiscard]]
        bool decrease_reference() noexcept {
            --this->_references;
            assert((this->_references >= 0u));

            return this->_references == 0;
        }

        /// Returns the current number of references
        /// \returns The current number of references
        [[nodiscard]]
        uint32_t references() const noexcept {
            return this->_references;
        }

    private:
        /// The number of references to this resource
        uint32_t _references {0u};

        /// The resource itself
        std::shared_ptr<T> _resource;
    };
}
