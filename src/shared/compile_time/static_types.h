#include <type_traits>

namespace pbr::shared::compile_time {
    /// Allows a `static_assert` to fail whilst being dependent
    /// on a type
    /// \tparam T The type to depend on
    template <typename T>
    struct always_false : std::false_type {};
}