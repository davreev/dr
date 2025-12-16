#pragma once

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
    template <typename Src>
    constexpr FunctionRef(Src* src)
    {
        static_assert(std::is_invocable_r_v<Return, Src, Args...>);

        if (src != nullptr)
        {
            if constexpr (std::is_const_v<Src>)
                ptr_.obj = const_cast<void*>(static_cast<void const*>(src));
            else
                ptr_.obj = src;

            invoke_ = [](Ptr const& ptr, Args... args) -> Return {
                return (*static_cast<Src*>(ptr.obj))(std::forward<Args>(args)...);
            };
        }
    }

    /// Creates an instance from a function pointer
    constexpr FunctionRef(Return (*src)(Args...))
    {
        using Src = decltype(src);

        if (src != nullptr)
        {
            ptr_.fn = reinterpret_cast<void (*)()>(src);
            invoke_ = [](Ptr const& ptr, Args... args) -> Return {
                return (reinterpret_cast<Src>(ptr.fn))(std::forward<Args>(args)...);
            };
        }
    }

    /// Invokes the referenced function
    constexpr Return operator()(Args... args) const
    {
        return invoke_(ptr_, std::forward<Args>(args)...);
    }

    /// Returns true if the instance refers to a valid memory address
    constexpr bool is_valid() const { return invoke_ != nullptr; }
    constexpr explicit operator bool() const { return is_valid(); }

  private:
    union Ptr
    {
        void* obj;
        void (*fn)();
    };

    Ptr ptr_{};
    Return (*invoke_)(Ptr const&, Args...){};
};

} // namespace dr