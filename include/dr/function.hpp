#pragma once

#include <cassert>
#include <type_traits>
#include <utility>

#include <dr/allocator.hpp>

namespace dr
{

/// Dummy definition to allow partial specialization below
template <typename T>
struct Function;

/// Type-erased allocator-aware function wrapper
template <typename Return, typename... Args>
struct Function<Return(Args...)> final : AllocatorAware
{
    Function(Allocator alloc = {}) : alloc_{alloc} {}

    /// Creates an instance from a function object
    template <
        typename Fn,
        typename DecayFn = std::decay_t<Fn>,
        std::enable_if_t<!std::is_same_v<DecayFn, Function>>* = nullptr>
    Function(Fn&& fn, Allocator alloc = {}) : alloc_{alloc}
    {
        static_assert(std::is_invocable_r_v<Return, DecayFn, Args...>);

        // Move function object to location given by allocator
        ptr_ = {.obj = alloc_.new_object<DecayFn>(std::forward<Fn>(fn))};
        vtable_ = {
            .invoke = [](Ptr const& ptr, Args... args) -> Return {
                assert(ptr.obj);
                return (*static_cast<DecayFn*>(ptr.obj))(std::forward<Args>(args)...);
            },
            .clone = [](Ptr const& ptr, Allocator alloc) -> Ptr {
                assert(ptr.obj);
                return {.obj = alloc.new_object<DecayFn>(*static_cast<DecayFn const*>(ptr.obj))};
            },
            .release = [](Ptr& ptr, Allocator alloc) -> void {
                assert(ptr.obj);
                alloc.delete_object(static_cast<DecayFn*>(ptr.obj));
                ptr.obj = nullptr;
            },
        };
    }

    /// Creates an instance from a function pointer
    Function(Return (*fn)(Args...), Allocator alloc = {}) : alloc_{alloc}
    {
        using Fn = decltype(fn);

        if (fn != nullptr)
        {
            ptr_ = {.fn = reinterpret_cast<void (*)()>(fn)};
            vtable_ = {
                .invoke = [](Ptr const& ptr, Args... args) -> Return {
                    assert(ptr.fn);
                    return (reinterpret_cast<Fn>(ptr.fn))(std::forward<Args>(args)...);
                },
                .clone = [](Ptr const& ptr, Allocator) -> Ptr {
                    assert(ptr.fn);
                    return ptr;
                },
                .release = [](Ptr& ptr, Allocator) -> void {
                    assert(ptr.fn);
                    ptr.fn = nullptr;
                },
            };
        }
    }

    Function(Function const& other, Allocator alloc = {}) : alloc_{alloc}
    {
        if (other.is_valid())
        {
            ptr_ = other.clone_ptr(alloc_);
            vtable_ = other.vtable_;
        }
    }

    Function(Function&& other) noexcept :
        ptr_{other.ptr_}, vtable_{other.vtable_}, alloc_{other.alloc_}
    {
        other.ptr_ = {};
        other.vtable_ = {};
    }

    ~Function()
    {
        if (is_valid())
            release_ptr();
    }

    Function& operator=(Function const& other)
    {
        if (is_valid())
            release_ptr();

        if (other.is_valid())
        {
            ptr_ = other.clone_ptr(alloc_);
            vtable_ = other.vtable_;
        }
        else
        {
            ptr_ = {};
            vtable_ = {};
        }

        return *this;
    }

    Function& operator=(Function&& other) noexcept
    {
        if (alloc_ == other.alloc_)
        {
            // If both use the same allocator, then can just steal the pointer as usual
            ptr_ = other.ptr_;
            vtable_ = other.vtable_;

            other.ptr_ = {};
            other.vtable_ = {};
        }
        else
        {
            // If they use different allocators, then fall back to a copy assign
            *this = other;
        }

        return *this;
    }

    /// Invokes the function
    Return operator()(Args... args) const
    {
        assert(vtable_.invoke);
        return vtable_.invoke(ptr_, std::forward<Args>(args)...);
    }

    /// Returns true if the instance has a valid function target
    bool is_valid() const { return vtable_.invoke != nullptr; }
    explicit operator bool() const { return is_valid(); }

  private:
    union Ptr
    {
        void* obj;
        void (*fn)();
    };

    using InvokePtr = Return(Ptr const&, Args...);
    using ClonePtr = Ptr(Ptr const&, Allocator);
    using ReleasePtr = void(Ptr&, Allocator);

    Ptr ptr_{};
    struct
    {
        InvokePtr* invoke{};
        ClonePtr* clone{};
        ReleasePtr* release{};
    } vtable_;
    Allocator alloc_{};

    Ptr clone_ptr(Allocator alloc) const
    {
        assert(vtable_.clone);
        return vtable_.clone(ptr_, alloc);
    }

    void release_ptr()
    {
        assert(vtable_.release);
        vtable_.release(ptr_, alloc_);
    }
};

} // namespace dr