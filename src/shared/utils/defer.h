#pragma once

#include <functional>
#include <atomic>
#include <cstdint>

namespace pbr::shared::utils {
    /// Runs a functor when destructed. This is designed to mimic golang's `defer` keyword
    class defer {
    public:
        /// The type of the destruct functor
        using on_destruct_functor_type = std::function<void(void)>;

        /// Constructs this defer object
        explicit defer(on_destruct_functor_type on_destruct)
            : _on_destruct { on_destruct } {
        }

        /// Destructs this object and calls the defer object, if it
        /// hasn't already been called
        ~defer() {
            if (!this->_has_destructed) {
                this->_on_destruct();
            }

            this->_has_destructed = true;
        }

    private:
        /// Has the destructor already been called?
        std::atomic_bool _has_destructed {false};

        /// The functor to call on destruct
        on_destruct_functor_type _on_destruct;
    };
}
