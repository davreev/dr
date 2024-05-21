#pragma once

#include <dr/basic_types.hpp>
#include <dr/shim/pmr/memory_resource.hpp>

namespace dr
{
namespace impl
{

using AllocatorBase = std::pmr::polymorphic_allocator<std::byte>;

}

struct Allocator : impl::AllocatorBase
{
    // Expose constructors of base type for compatibility with pmr containers
    using impl::AllocatorBase::AllocatorBase;

    void* allocate(usize size, usize alignment) const
    {
        return resource()->allocate(size, alignment);
    }

    void deallocate(void* ptr, usize size, usize alignment) const
    {
        return resource()->deallocate(ptr, size, alignment);
    }

    template <typename T>
    T* allocate(usize const count = 1) const
    {
        return static_cast<T*>(allocate(count * sizeof(T), alignof(T)));
    }

    template <typename T>
    void deallocate(T* const ptr, usize const count = 1) const
    {
        deallocate(ptr, count * sizeof(T), alignof(T));
    }

    template <typename T, typename... Args>
    T* new_object(Args&&... args)
    {
        static_assert(!std::is_array_v<T>);
        T* const ptr = allocate<T>();
        construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    template <typename T>
    void delete_object(T* ptr) const
    {
        ptr->~T();
        deallocate(ptr);
    }
};

/// Enables allocator propagation on derived types
struct AllocatorAware
{
    using allocator_type = Allocator;
};

/// Returns true if T is allocator-aware
template <typename T>
inline constexpr bool is_allocator_aware = std::uses_allocator_v<std::decay_t<T>, Allocator>;

} // namespace dr