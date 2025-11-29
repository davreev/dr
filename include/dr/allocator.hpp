#pragma once

#include <memory_resource>

#include <dr/basic_types.hpp>

namespace dr
{
namespace impl
{

using AllocatorBase = std::pmr::polymorphic_allocator<std::byte>;

}

/// Represents a non-owning reference to a memory resource or "arena" from which memory can be
/// allocated
struct Allocator : impl::AllocatorBase
{
    using Traits = std::allocator_traits<impl::AllocatorBase>;

    // Expose constructors of base type for compatibility with pmr containers
    using impl::AllocatorBase::AllocatorBase;

#if __cplusplus == 202002L
    // C++20

    template <typename T>
    void destroy(T* ptr)
    {
        Traits::destroy(*this, ptr);
    }

#else
    // C++17

    void* allocate_bytes(usize size, usize alignment) const
    {
        return resource()->allocate(size, alignment);
    }

    void deallocate_bytes(void* ptr, usize size, usize alignment) const
    {
        return resource()->deallocate(ptr, size, alignment);
    }

    template <typename T>
    T* allocate_object(usize const count = 1) const
    {
        return static_cast<T*>(allocate_bytes(count * sizeof(T), alignof(T)));
    }

    template <typename T>
    void deallocate_object(T* const ptr, usize const count = 1) const
    {
        deallocate_bytes(ptr, count * sizeof(T), alignof(T));
    }

    template <typename T, typename... Args>
    T* new_object(Args&&... args)
    {
        static_assert(!std::is_array_v<T>);
        T* const ptr = allocate_object<T>();
        construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    template <typename T>
    void delete_object(T* ptr)
    {
        static_assert(!std::is_array_v<T>);
        destroy(ptr);
        deallocate_object(ptr);
    }

#endif
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