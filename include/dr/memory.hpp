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

/*
    NOTE: When type punning via as<T>(), care must be taken to avoid violating the strict
    aliasing rule which states that two objects of different types cannot occupy the same location
    in memory. When strict aliasing is enabled (e.g. in Release builds), the compiler assumes this
    rule to be true and optimizes accordingly.

    Refs
    - https://cellperformance.beyond3d.com/articles/2006/06/understanding-strict-aliasing.html
    - https://gist.github.com/shafik/848ae25ee209f698763cffee272a58f8
*/

/// Interprets an opaque pointer as a pointer to T if it meets T's alignment requirements.
/// Otherwise, returns a nullptr.
template <typename T>
T* as(void* const ptr)
{
    if (is_aligned<T>(ptr))
        return static_cast<T*>(ptr);
    else
        return nullptr;
}

/// Interprets an opaque pointer as a pointer to T if it meets T's alignment requirements.
/// Otherwise, returns a nullptr.
template <typename T>
T const* as(void const* const ptr)
{
    if (is_aligned<T>(ptr))
        return static_cast<T const*>(ptr);
    else
        return nullptr;
}

/// Reinterprets a pointer to one type as a pointer to another
template <typename Dst, typename Src, std::enable_if_t<(alignof(Dst) <= alignof(Src))>* = nullptr>
constexpr Dst* as(Src* const ptr)
{
    return reinterpret_cast<Dst*>(ptr);
}

/// Reinterprets a pointer to one type as a pointer to another
template <typename Dst, typename Src, std::enable_if_t<(alignof(Dst) > alignof(Src))>* = nullptr>
Dst* as(Src* const ptr)
{
    return as<Dst>(static_cast<void*>(ptr));
}

/// Reinterprets a pointer to one type as a pointer to another
template <typename Dst, typename Src, std::enable_if_t<(alignof(Dst) <= alignof(Src))>* = nullptr>
constexpr Dst const* as(Src const* const ptr)
{
    return reinterpret_cast<Dst const*>(ptr);
}

/// Reinterprets a pointer to one type as a pointer to another
template <typename Dst, typename Src, std::enable_if_t<(alignof(Dst) > alignof(Src))>* = nullptr>
Dst const* as(Src const* const ptr)
{
    return as<Dst>(static_cast<void const*>(ptr));
}

/// Reinterprets a span of one type as a span of another
template <typename Dst, typename Src, std::enable_if_t<(alignof(Dst) <= alignof(Src))>* = nullptr>
constexpr Span<Dst> as(Span<Src> const& src)
{
    return {
        as<Dst>(src.data()),
        static_cast<isize>((src.size() * sizeof(Src)) / sizeof(Dst))};
}

/// Reinterprets a span of one type as a span of another
template <typename Dst, typename Src, std::enable_if_t<(alignof(Dst) > alignof(Src))>* = nullptr>
Span<Dst> as(Span<Src> const& src)
{
    return {
        as<Dst>(src.data()),
        static_cast<isize>((src.size() * sizeof(Src)) / sizeof(Dst))};
}

/// Reinterprets a span of one type as a span of another
template <typename Dst, typename Src, std::enable_if_t<(alignof(Dst) <= alignof(Src))>* = nullptr>
constexpr Span<Dst const> as(Span<Src const> const& src)
{
    return {
        as<Dst>(src.data()),
        static_cast<isize>((src.size() * sizeof(Src)) / sizeof(Dst))};
}

/// Reinterprets a span of one type as a span of another
template <typename Dst, typename Src, std::enable_if_t<(alignof(Dst) > alignof(Src))>* = nullptr>
Span<Dst const> as(Span<Src const> const& src)
{
    return {
        as<Dst>(src.data()),
        static_cast<isize>((src.size() * sizeof(Src)) / sizeof(Dst))};
}

using AllocatorBase = std::pmr::polymorphic_allocator<std::byte>;

struct Allocator : AllocatorBase
{
    // Expose constructors of base type for compatibility with pmr containers
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