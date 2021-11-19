#include <atomic>
#include <cstdint>
#include <memory>

#include "basic_allocators.h"

/// Keeps track of the number of allocated bytes for the application as a whole, for all threads
static std::atomic_uint64_t NumberOfAllocatedBytes {0};

void* operator new(size_t size) {
    auto ptr = malloc(size);

    NumberOfAllocatedBytes += size;

    return ptr;
}

void operator delete(void* ptr, size_t size) noexcept {
    free(ptr);

    NumberOfAllocatedBytes -= size;
}

namespace pbr::shared::memory {
    uint64_t get_number_of_allocated_bytes() noexcept {
        return NumberOfAllocatedBytes;
    }
}
