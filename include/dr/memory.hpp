#pragma once

/*
    Assorted helper functions and types related to memory management
*/

#include <memory>
#include <type_traits>

#include <dr/basic_types.hpp>
#include <dr/span.hpp>

#include <dr/shim/pmr/memory_resource.hpp>

namespace dr
{

/// Returns true if a pointer meets the alignment requirements of a specified type
template <typename T>
bool is_aligned(void const* const ptr)
{
    usize const addr = reinterpret_cast<usize>(ptr);
    return (addr & (alignof(T) - 1)) == 0;
}

/// Reinterprets an opaque pointer as a pointer to a specified type
template <typename T>
T* as(void* const ptr)
{
    assert(is_aligned<T>(ptr));
    return static_cast<T*>(ptr);
}

/// Reinterprets an opaque pointer as a pointer to a specified type
template <typename T>
T const* as(void const* const ptr)
{
    assert(is_aligned<T>(ptr));
    return static_cast<T const*>(ptr);
}

/// Reinterprets a pointer to one type as a pointer to another
template <typename Dst, typename Src>
Dst* as(Src* const ptr)
{
    if constexpr (alignof(Src) >= alignof(Dst))
        return reinterpret_cast<Dst*>(ptr);
    else
        return as<Dst>(static_cast<void*>(ptr));
}

/// Reinterprets a pointer to one type as a pointer to another
template <typename Dst, typename Src>
Dst const* as(Src const* const ptr)
{
    if constexpr (alignof(Src) >= alignof(Dst))
        return reinterpret_cast<Dst const*>(ptr);
    else
        return as<Dst>(static_cast<void const*>(ptr));
}

/// Reinterprets a span of one type as a span of another
template <typename Dst, typename Src>
Span<Dst> as(Span<Src> const& src)
{
    return {
        as<Dst>(src.data()),
        static_cast<isize>((src.size() * sizeof(Src)) / sizeof(Dst))};
}

/// Reinterprets a span of one type as a span of another
template <typename Dst, typename Src>
Span<Dst const> as(Span<Src const> const& src)
{
    return {
        as<Dst>(src.data()),
        static_cast<isize>((src.size() * sizeof(Src)) / sizeof(Dst))};
}

using AllocatorBase = std::pmr::polymorphic_allocator<std::byte>;

struct Allocator : AllocatorBase
{
    // Inherit all constructors of base type
    using AllocatorBase::AllocatorBase;

    void* allocate(usize size, usize alignment) const;

    void deallocate(void* ptr, usize size, usize alignment) const;

    template <typename T>
    T* allocate(usize const count) const
    {
        return static_cast<T*>(allocate(count * sizeof(T), alignof(T)));
    }

    template <typename T>
    void deallocate(T* const ptr, usize const count) const
    {
        deallocate(ptr, count * sizeof(T), alignof(T));
    }
};

/// Derived types will use the allocator of a parent container if compatible
struct AllocatorAware
{
    using allocator_type = Allocator;
};

/// Simple RAII-style heap allocation
struct ScopedAlloc
{
    ScopedAlloc(usize size, usize alignment, Allocator alloc = {});
    ~ScopedAlloc();

    ScopedAlloc(ScopedAlloc const&) = delete;
    ScopedAlloc& operator=(ScopedAlloc const&) = delete;

    Span<u8> data() const;

  private:
    void* data_;
    usize size_;
    usize alignment_;
    Allocator alloc_;
};

/// Deleter for allocator-backed unique pointers
struct DeleteUnique
{
    Allocator alloc{};

    template <typename T>
    void operator()(T* const ptr)
    {
        ptr->~T();
        alloc.deallocate(ptr, 1);
    }
};

/// Alias for an allocator-backed unique pointer
template <typename T>
using UniquePtr = std::unique_ptr<T, DeleteUnique>;

/// Creates an allocator-backed unique pointer
template <typename T, typename... Args>
UniquePtr<T> make_unique(Allocator const alloc, Args&&... args)
{
    static_assert(!std::is_array_v<T>);
    T* const ptr = alloc.allocate<T>(1);
    return {new (ptr) T{std::forward<Args>(args)...}, DeleteUnique{alloc}};
}

/// Memory resource for tracking allocation frequency and memory footprint
struct DebugMemoryResource : public std::pmr::memory_resource
{
    std::pmr::memory_resource* upstream;
    usize num_allocs{};
    usize num_deallocs{};
    usize bytes_allocated{};
    usize max_bytes_allocated{};

    DebugMemoryResource();
    DebugMemoryResource(std::pmr::memory_resource* upstream);
    ~DebugMemoryResource();

    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(std::pmr::memory_resource const& other) const noexcept override;
};

} // namespace dr