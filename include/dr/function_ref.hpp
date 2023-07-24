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
    template <typename Src>
    constexpr FunctionRef(Src* src)
    {
        ptr_.obj = src;
        set_invoke_obj<Src>();
    }

    template <typename Src>
    constexpr FunctionRef(Src const* src)
    {
        ptr_.obj = const_cast<Src*>(src);
        set_invoke_obj<Src const>();
    }

    constexpr FunctionRef(Return (*src)(Args...))
    {
        ptr_.fn = reinterpret_cast<void (*)()>(src);
        set_invoke_fn<decltype(src)>();
    }

    constexpr Return operator()(Args&&... args) const
    {
        return invoke_(ptr_, std::forward<Args>(args)...);
    }

  private:
    union Ptr
    {
        void* obj;
        void (*fn)();
    } ptr_;
    Return (*invoke_)(Ptr const&, Args&&...);

    template <typename Src>
    constexpr void set_invoke_obj()
    {
        static_assert(std::is_invocable_r_v<Return, Src, Args...>);

        // Invokes as function object
        invoke_ = [](Ptr const& ptr, Args&&... args) -> Return {
            return (*static_cast<Src*>(ptr.obj))(std::forward<Args>(args)...);
        };
    }

    template <typename Src>
    constexpr void set_invoke_fn()
    {
        // Invokes as function ptr
        invoke_ = [](Ptr const& ptr, Args&&... args) -> Return {
            return (reinterpret_cast<Src>(ptr.fn))(std::forward<Args>(args)...);
        };
    }
};

} // namespace dr