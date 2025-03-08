#pragma once

#include <chrono>

#include <fmt/core.h>

namespace dr
{

template <typename Duration>
struct ScopedTimer final
{
    ScopedTimer(char const* const context, std::FILE* const out = stdout) :
        context_{context},
        out_{out},
        start_{Clock::now()}
    {
    }

    ~ScopedTimer()
    {
        auto const elapsed = std::chrono::duration_cast<Duration>(Clock::now() - start_);
        fmt::print(out_, "{}: {} {}\n", context_, elapsed.count(), suffix());
    }

  private:
    using Clock = std::chrono::high_resolution_clock;

    char const* context_;
    std::FILE* out_;
    Clock::time_point start_;

    static constexpr char const* suffix() { return ""; }
};

using NanoScopedTimer = ScopedTimer<std::chrono::nanoseconds>;
using MicroScopedTimer = ScopedTimer<std::chrono::microseconds>;
using MilliScopedTimer = ScopedTimer<std::chrono::milliseconds>;

template <>
constexpr char const* NanoScopedTimer::suffix() { return "ns"; }

template <>
constexpr char const* MicroScopedTimer::suffix() { return "us"; }

template <>
constexpr char const* MilliScopedTimer::suffix() { return "ms"; }

} // namespace dr