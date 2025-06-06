#pragma once

/*
    Assorted helper functions and types related to memory management
*/

#include <memory>
#include <type_traits>

#include <dr/allocator.hpp>
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
    if constexpr(alignof(T) == 1)
    {
        return static_cast<T*>(ptr);
    }
    else
    {
        if (is_aligned<T>(ptr))
            return static_cast<T*>(ptr);
        else
            return nullptr;
    }
}

/// Interprets an opaque pointer as a pointer to T if it meets T's alignment requirements.
/// Otherwise, returns a nullptr.
template <typename T>
T const* as(void const* const ptr)
{
    if constexpr(alignof(T) == 1)
    {
        return static_cast<T const*>(ptr);
    }
    else
    {
        if (is_aligned<T>(ptr))
            return static_cast<T const*>(ptr);
        else
            return nullptr;
    }
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
template <typename Dst, typename Src>
constexpr Span<Dst> as(Span<Src> const& src)
{
    return {as<Dst>(src.data()), isize((src.size() * sizeof(Src)) / sizeof(Dst))};
}

/// Reinterprets a span of one type as a span of another
template <typename Dst, typename Src>
constexpr Span<Dst const> as(Span<Src const> const& src)
{
    return {as<Dst>(src.data()), isize((src.size() * sizeof(Src)) / sizeof(Dst))};
}

/// Views the given instance as a span of bytes
template <typename T>
Span<u8> as_bytes(T& item)
{
    return {as<u8>(&item), isize(sizeof(T))};
}

/// Views the given instance as a span of bytes
template <typename T>
Span<u8 const> as_bytes(T const& item)
{
    return {as<u8>(&item), isize(sizeof(T))};
}

/// Deleted to avoid creating a span over a temporary
template <typename T>
Span<u8 const> as_bytes(T const&& item) = delete;

/// Simple RAII-style heap allocation
template <usize alignment>
struct ScopedAlloc final
{
    ScopedAlloc(usize size, Allocator alloc = {}) :
        data_{alloc.allocate(size, alignment)}, size_{size}, alloc_{alloc}
    {
    }

    ~ScopedAlloc() { alloc_.deallocate(data_, size_, alignment); }

    ScopedAlloc(ScopedAlloc const&) = delete;
    ScopedAlloc& operator=(ScopedAlloc const&) = delete;

    Span<u8> data() { return {static_cast<u8*>(data_), isize(size_)}; }

    Span<u8 const> data() const
    {
        return {static_cast<u8 const*>(data_), isize(size_)};
    }

    template <typename T>
    Span<T> data_as()
    {
        static_assert(std::is_trivially_destructible_v<T>);
        static_assert(alignof(T) <= alignment);
        return {static_cast<T*>(data_), isize(size_ / sizeof(T))};
    }

    template <typename T>
    Span<T const> data_as() const
    {
        static_assert(std::is_trivially_destructible_v<T>);
        static_assert(alignof(T) <= alignment);
        return {static_cast<T const*>(data_), isize(size_ / sizeof(T))};
    }

  private:
    void* data_;
    usize size_;
    Allocator alloc_;
};

/// Deleter for allocator-backed unique pointers
struct DeleteUnique
{
    Allocator alloc{};

    template <typename T>
    void operator()(T* const ptr)
    {
        alloc.delete_object(ptr);
    }
};

/// Alias for an allocator-backed unique pointer
template <typename T>
using UniquePtr = std::unique_ptr<T, DeleteUnique>;

/// Creates an allocator-backed unique pointer
template <typename T, typename... Args>
UniquePtr<T> make_unique(Allocator alloc, Args&&... args)
{
    return {alloc.new_object<T>(std::forward<Args>(args)...), DeleteUnique{alloc}};
}

/// Memory resource for tracking allocation frequency and memory footprint
struct DebugMemoryResource final : public std::pmr::memory_resource
{
    std::pmr::memory_resource* upstream;
    usize num_allocs{};
    usize num_deallocs{};
    usize bytes_allocated{};
    usize max_bytes_allocated{};

    DebugMemoryResource() : upstream{std::pmr::get_default_resource()} {}

    DebugMemoryResource(std::pmr::memory_resource* const upstream) : upstream{upstream} {}

    ~DebugMemoryResource() { assert(bytes_allocated == 0); }

    void* do_allocate(std::size_t bytes, std::size_t alignment) override;

    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override;

    bool do_is_equal(std::pmr::memory_resource const& other) const noexcept override
    {
        return this == &other;
    };
};

} // namespace dr