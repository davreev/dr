#pragma once

#include <utility>

namespace dr
{

template <typename Func>
struct Defer
{
    template <typename Func_>
    Defer(Func_&& func) :
        func_(std::forward<Func_>(func))
    {
    }

    Defer(Defer const&) = delete;
    Defer& operator=(Defer const&) = delete;

    ~Defer() { func_(); }

  private:
    Func func_;
};

template <typename Func>
[[nodiscard]]
Defer<Func> defer(Func&& func)
{
    return {std::forward<Func>(func)};
}

} // namespace dr