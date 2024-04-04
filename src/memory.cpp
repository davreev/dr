#include <dr/memory.hpp>

namespace dr
{

void* DebugMemoryResource::do_allocate(std::size_t const bytes, std::size_t const alignment)
{
    ++num_allocs;
    bytes_allocated += bytes;

    if (bytes_allocated > max_bytes_allocated)
        max_bytes_allocated = bytes_allocated;

    return upstream->allocate(bytes, alignment);
}

void DebugMemoryResource::do_deallocate(
    void* const ptr,
    std::size_t const bytes,
    std::size_t const alignment)
{
    assert(ptr != nullptr);
    assert(bytes_allocated >= bytes);

    ++num_deallocs;
    bytes_allocated -= bytes;
    upstream->deallocate(ptr, bytes, alignment);
}

} // namespace dr