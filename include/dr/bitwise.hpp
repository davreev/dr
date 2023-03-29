#pragma once

/*
    Assorted helper functions for bitwise manipulation
*/

#include <cassert>

#include <dr/basic_types.hpp>
#include <dr/math_traits.hpp>

namespace dr
{

/// Returns true if the given value is a power of 2
template <typename Nat>
constexpr bool is_pow2(Nat const x)
{
    static_assert(is_natural<Nat>);
    return (x != 0) && ((x & (x - 1)) == 0);
}

/// Returns the nearest power of 2 that is greater than or equal to the given value
template <typename Nat>
constexpr Nat next_pow2(Nat x)
{
    static_assert(is_natural<Nat>);

    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;

    if constexpr (sizeof(x) > 1)
        x |= x >> 8;

    if constexpr (sizeof(x) > 2)
        x |= x >> 16;

    if constexpr (sizeof(x) > 4)
        x |= x >> 32;

    return x + 1;
}

/// Returns the nearest power of 2 that is less than or equal to the given value
template <typename Nat>
constexpr Nat prev_pow2(Nat x)
{
    static_assert(is_natural<Nat>);

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;

    if constexpr (sizeof(x) > 1)
        x |= x >> 8;

    if constexpr (sizeof(x) > 2)
        x |= x >> 16;

    if constexpr (sizeof(x) > 4)
        x |= x >> 32;

    return x ^ (x >> 1);
}

template <typename Scalar>
void unit_square_vertex(u8 const index, Scalar& x, Scalar& y)
{
    static_assert(is_real<Scalar> || is_integer<Scalar> || is_natural<Scalar>);

    assert(index < 4);
    x = static_cast<Scalar>(index & 1);
    y = static_cast<Scalar>((index >> 1) & 1);
}

template <typename Scalar>
void unit_cube_vertex(u8 const index, Scalar& x, Scalar& y, Scalar& z)
{
    static_assert(is_real<Scalar> || is_integer<Scalar> || is_natural<Scalar>);

    assert(index < 8);
    x = static_cast<Scalar>(index & 1);
    y = static_cast<Scalar>((index >> 1) & 1);
    z = static_cast<Scalar>((index >> 2) & 1);
}

} // namespace dr