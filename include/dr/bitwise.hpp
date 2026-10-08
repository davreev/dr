#pragma once

/*
    Assorted helper functions for bitwise manipulation
*/

#include <cassert>

#include <dr/num_traits.hpp>

namespace dr
{

/// Returns the number of ones in the binary representation of an unsigned integer
template <typename Num>
constexpr u8 bit_sum(Num x)
{
    static_assert(is_natural<Num>);

    u8 sum{};
    while (x != 0)
    {
        x &= x - 1;
        ++sum;
    }

    return sum;
}

/// Returns true if the given value is a power of 2
template <typename Num>
constexpr bool is_pow2(Num const x)
{
    static_assert(is_natural<Num>);
    return (x != 0) && ((x & (x - 1)) == 0);
}

/// Returns the nearest power of 2 that is greater than or equal to the given value
template <typename Num>
constexpr Num next_pow2(Num x)
{
    static_assert(is_natural<Num>);

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
template <typename Num>
constexpr Num prev_pow2(Num x)
{
    static_assert(is_natural<Num>);

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

template <typename Num>
constexpr void unit_square_corner(u8 const index, Num result[2])
{
    assert(index < 4);
    result[0] = Num(index & 1);
    result[1] = Num((index >> 1) & 1);
}

template <typename Num>
constexpr void unit_cube_corner(u8 const index, Num result[3])
{
    assert(index < 8);
    result[0] = Num(index & 1);
    result[1] = Num((index >> 1) & 1);
    result[2] = Num((index >> 2) & 1);
}

} // namespace dr