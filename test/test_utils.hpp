#pragma once

#include <algorithm>

#include <dr/math.hpp>
#include <dr/span.hpp>

namespace dr
{

template <typename T, typename U>
bool all_equal(Span<T> const& a, Span<U> const& b)
{
    return (a.size() == b.size()) ? std::equal(begin(a), end(a), begin(b)) : false;
}

template <typename Real>
bool all_near_equal(Span<Real const> const& a, Span<Real const> const& b, Real const abs_tol)
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

} // namespace dr