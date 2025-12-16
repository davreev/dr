#pragma once

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
    /// Creates an instance from a function object
    template <
        typename Fn,
        typename DecayFn = std::decay_t<Fn>,
        std::enable_if_t<!std::is_same_v<DecayFn, Function>>* = nullptr>
    Function(Fn&& fn, Allocator alloc = {}) : alloc_{alloc}
    {
        static_assert(std::is_invocable_r_v<Return, DecayFn, Args...>);

        // Move fn to location provided by the given allocator
        ptr_.obj = alloc_.new_object<DecayFn>(std::forward<Fn>(fn));

        invoke_ = [](Ptr const& ptr, Args... args) -> Return {
            return ((*static_cast<DecayFn*>(ptr.obj))(std::forward<Args>(args)...));
        };

        delete_ = [](Ptr const& ptr, Allocator alloc) {
            if (ptr.obj != nullptr)
                alloc.delete_object(static_cast<DecayFn*>(ptr.obj));
        };

        clone_ = [](Ptr const& ptr, Allocator alloc) -> Ptr {
            Ptr result{};
            result.obj = alloc.new_object<DecayFn>(*static_cast<DecayFn const*>(ptr.obj));
            return result;
        };
    }

    /// Creates an instance from a function pointer
    Function(Return (*fn)(Args...), Allocator alloc = {}) : alloc_{alloc}
    {
        using Fn = decltype(fn);

        if (fn != nullptr)
        {
            ptr_.fn = reinterpret_cast<void (*)()>(fn);
            invoke_ = [](Ptr const& ptr, Args... args) -> Return {
                return (reinterpret_cast<Fn>(ptr.fn))(std::forward<Args>(args)...);
            };
        }

        delete_ = [](Ptr const&, Allocator) { /*no-op*/ };
        clone_ = [](Ptr const& ptr, Allocator) -> Ptr { return ptr; };
    }

    Function(Function const& other, Allocator alloc = {}) :
        ptr_{other.clone_ptr(alloc)},
        invoke_{other.invoke_},
        clone_{other.clone_},
        delete_{other.delete_},
        alloc_{alloc}
    {
    }

    Function(Function&& other) noexcept :
        ptr_{other.ptr_},
        invoke_{other.invoke_},
        clone_{other.clone_},
        delete_{other.delete_},
        alloc_{other.alloc_}
    {
        other.ptr_ = {};
    }

    ~Function() { delete_ptr(); }

    Function& operator=(Function const& other)
    {
        delete_ptr();

        ptr_ = other.clone_ptr(alloc_);
        invoke_ = other.invoke_;
        clone_ = other.clone_;
        delete_ = other.delete_;

        return *this;
    }

    Function& operator=(Function&& other) noexcept
    {
        if (alloc_ == other.alloc_)
        {
            // If both use the same allocator, then we can just steal the pointer as usual
            ptr_ = other.ptr_;
            invoke_ = other.invoke_;
            clone_ = other.clone_;
            delete_ = other.delete_;

            other.ptr_ = {};
        }
        else
        {
            // If they use different allocators, then we have to perform a copy assign
            *this = other;
        }

        return *this;
    }

    /// Invokes the function
    constexpr Return operator()(Args... args) const
    {
        return invoke_(ptr_, std::forward<Args>(args)...);
    }

  private:
    union Ptr
    {
        void* obj;
        void (*fn)();
    };

    Ptr ptr_{};
    Return (*invoke_)(Ptr const&, Args...){};
    Ptr (*clone_)(Ptr const&, Allocator){};
    void (*delete_)(Ptr const&, Allocator){};
    Allocator alloc_{};

    Ptr clone_ptr(Allocator alloc) const { return clone_(ptr_, alloc); }
    void delete_ptr() const { delete_(ptr_, alloc_); }
};

} // namespace dr