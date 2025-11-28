#pragma once

/*
    Assorted helper functions for bitwise manipulation
*/

#include <cassert>

#include <dr/basic_traits.hpp>

namespace dr
{

/// Returns the number of ones in the binary representation of an unsigned integer
template <typename Nat>
constexpr u8 bit_sum(Nat x)
{
    static_assert(is_natural<Nat>);

    u8 sum{};
    while(x != 0)
    {
        x &= x - 1;
        ++sum;
    }

    return sum;
}

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
constexpr void unit_square_corner(u8 const index, Scalar result[2])
{
    static_assert(is_real<Scalar> || is_integer<Scalar> || is_natural<Scalar>);

    assert(index < 4);
    result[0] = Scalar(index & 1);
    result[1] = Scalar((index >> 1) & 1);
}

template <typename Scalar>
constexpr void unit_cube_corner(u8 const index, Scalar result[3])
{
    static_assert(is_real<Scalar> || is_integer<Scalar> || is_natural<Scalar>);

    assert(index < 8);
    result[0] = Scalar(index & 1);
    result[1] = Scalar((index >> 1) & 1);
    result[2] = Scalar((index >> 2) & 1);
}

} // namespace dr