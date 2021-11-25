#include <atomic>
#include <cstdint>
#include <tuple>

#include "basic_allocators.h"

/// Keeps track of the number of allocated bytes for the application as a whole, for all threads
static std::atomic_uint64_t NumberOfAllocatedBytes {0};

/// Allocates a block of memory with the header
/// \param size The size of the block to allocate, not including the header size
/// \returns A pointer to the usable block of memory and the total allocated size, including the size of the header
std::pair<void*, size_t> allocate_with_header(size_t size) {
    auto total_size = size + sizeof(pbr::shared::memory::memory_block_header);

    auto ptr = malloc(total_size);

    auto header_ptr = static_cast<pbr::shared::memory::memory_block_header*>(ptr);
    header_ptr[0].key = pbr::shared::memory::memory_block_header::ID;
    header_ptr[0].size = total_size;

    return { static_cast<void*>(&header_ptr[1]), total_size };
}

/// Returns the starting address of the block of memory to free. If the memory was
/// allocated by us, it returns the address of the header, else it returns the passed
/// address
/// \param ptr The pointer to check
/// \returns The pointer to free and the size of the allocated block of memory. If the memory was
/// not allocated by us, `0` is returned.
std::pair<void*, size_t> get_ptr_to_free(void* ptr) {
    auto header_ptr = static_cast<pbr::shared::memory::memory_block_header*>(ptr);
    auto header = &header_ptr[-1];

    if (header->key != pbr::shared::memory::memory_block_header::ID) {
        return { ptr, 0 };
    }

    return { static_cast<void*>(header), header->size };
}

void* operator new(size_t size) {
    auto [ptr, total_size] = allocate_with_header(size);

    NumberOfAllocatedBytes += total_size;

    return ptr;
}

void operator delete(void* ptr) noexcept {
    auto [ptr_to_free, size] = get_ptr_to_free(ptr);

    free(ptr_to_free);

    NumberOfAllocatedBytes -= size;
}

namespace pbr::shared::memory {
    uint64_t get_number_of_allocated_bytes() noexcept {
        return NumberOfAllocatedBytes;
    }
}
