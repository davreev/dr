#pragma once

#include <algorithm>
#include <cerrno>
#include <cstdio>

#include <dr/math.hpp>
#include <dr/span.hpp>

namespace dr
{

template <typename T, typename U>
bool equal(Span<T> const& a, Span<U> const& b)
{
    return (a.size() == b.size()) ? std::equal(begin(a), end(a), begin(b)) : false;
}

template <typename Real>
bool near_equal(Span<Real const> const& a, Span<Real const> const& b, Real const abs_tol)
{
    if (a.size() != b.size())
        return false;

    for (isize i = 0; i < a.size(); ++i)
    {
        if (!near_equal(a[i], b[i], abs_tol))
            return false;
    }

    return true;
}

/// Workaround for MSVC deprecation warning on std::fopen
inline std::FILE* safe_fopen(char const* const filename, char const* const mode)
{
#if defined(_MSC_VER)
    std::FILE* f = nullptr;
    errno = fopen_s(&f, filename, mode);
    return f;
#else
    return std::fopen(filename, mode);
#endif
}

} // namespace dr