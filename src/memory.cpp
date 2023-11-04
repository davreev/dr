#include <dr/memory.hpp>

namespace dr
{

DebugMemoryResource::DebugMemoryResource() :
    upstream{std::pmr::get_default_resource()}
{
}

DebugMemoryResource::DebugMemoryResource(std::pmr::memory_resource* const upstream) :
    upstream{upstream}
{
}

DebugMemoryResource::~DebugMemoryResource()
{
    assert(bytes_allocated == 0);
}

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

bool DebugMemoryResource::do_is_equal(std::pmr::memory_resource const& other) const noexcept
{
    return this == &other;
};

} // namespace dr