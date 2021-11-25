#pragma once

#include <new>

#ifndef RELEASE
/// Overrides the default `new` operator. This will keep track of the total number of allocated bytes.
/// This is thread safe.
/// \param size The number of bytes to allocate.
/// \return A pointer to the allocated memory.
void* operator new(size_t size);

/// Overrides the default `delete` operator. This will decrease the total number of allocated bytes.
/// This is thread safe. If `ptr` was not allocated with the overridden `new` above, the result is
/// undefined behavior.
/// \param ptr The pointer to delete.
void operator delete(void* ptr) noexcept;
#endif

namespace pbr::shared::memory {
    /// The header of an allocated memory block
    struct memory_block_header {
        /// The value that `key` should have
        static const size_t ID {0xABCD};

        /// The key identifying this block
        size_t key {0u};

        /// The size of the allocated block, including the size
        /// of this header
        size_t size {0u};
    };

    /// Returns the total number of allocated bytes. This is thread safe.
    /// \return The total number of allocated bytes.
    uint64_t get_number_of_allocated_bytes() noexcept;
}
