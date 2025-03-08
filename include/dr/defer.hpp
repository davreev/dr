#pragma once

#include <utility>

namespace dr
{

template <typename Func>
struct Deferred final
{
    template <typename Func_>
    Deferred(Func_&& func) :
        func_(std::forward<Func_>(func))
    {
    }

    Deferred(Deferred const&) = delete;
    Deferred& operator=(Deferred const&) = delete;

    ~Deferred() { func_(); }

  private:
    Func func_;
};

template <typename Func>
[[nodiscard]]
Deferred<Func> defer(Func&& func)
{
    return {std::forward<Func>(func)};
}

} // namespace dr