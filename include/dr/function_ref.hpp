#pragma once

#include <cassert>
#include <type_traits>
#include <utility>

namespace dr
{

/// Dummy definition to allow partial specialization below
template <typename T>
struct FunctionRef;

/// Non-owning type-erased reference to an invocable object
template <typename Return, typename... Args>
struct FunctionRef<Return(Args...)>
{
    constexpr FunctionRef() = default;

    /// Creates an instance from a function object
    template <typename Fn, std::enable_if_t<std::is_invocable_r_v<Return, Fn, Args...>>* = nullptr>
    constexpr FunctionRef(Fn* fn)
    {
        if (fn != nullptr)
        {
            if constexpr (std::is_const_v<Fn>)
                ptr_ = {.obj = const_cast<void*>(static_cast<void const*>(fn))};
            else
                ptr_ = {.obj = fn};

            invoke_ = [](Ptr const& ptr, Args... args) -> Return {
                return (*static_cast<Fn*>(ptr.obj))(std::forward<Args>(args)...);
            };
        }
    }

    /// Creates an instance from a function pointer
    constexpr FunctionRef(Return (*fn)(Args...))
    {
        using Fn = decltype(fn);

        if (fn != nullptr)
        {
            ptr_ = {.fn = reinterpret_cast<void (*)()>(fn)};
            invoke_ = [](Ptr const& ptr, Args... args) -> Return {
                return (reinterpret_cast<Fn>(ptr.fn))(std::forward<Args>(args)...);
            };
        }
    }

    /// Invokes the referenced function
    constexpr Return operator()(Args... args) const
    {
        assert(invoke_);
        return invoke_(ptr_, std::forward<Args>(args)...);
    }

    /// Returns true if the instance refers to a valid function target
    constexpr bool is_valid() const { return invoke_ != nullptr; }
    constexpr explicit operator bool() const { return is_valid(); }

  private:
    union Ptr
    {
        void* obj;
        void (*fn)();
    };

    using InvokePtr = Return(Ptr const&, Args...);

    Ptr ptr_{};
    InvokePtr* invoke_{};
};

} // namespace dr