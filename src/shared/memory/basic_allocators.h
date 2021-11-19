#pragma once

#include <cstddef>

/// Overrides the default `new` operator. This will keep track of the total number of allocated bytes.
/// This is thread safe.
/// \param size The number of bytes to allocate.
/// \return A pointer to the allocated memory.
void* operator new(size_t size);

/// Overrides the default `delete` operator. This will decrease the total number of allocated bytes.
/// This is thread safe.
/// \param ptr The pointer to delete.
/// \param size The size of the memory pointed to by `ptr` in bytes.
void operator delete(void* ptr, size_t size) noexcept;

namespace pbr::shared::memory {
    /// Returns the total number of allocated bytes. This is thread safe.
    /// \return The total number of allocated bytes.
    uint64_t get_number_of_allocated_bytes() noexcept;
}
