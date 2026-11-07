#include <utest.h>

#include <dr/bitwise.hpp>
#include <dr/num_types.hpp>

/*
    Compile-time checks
*/

namespace dr
{

static_assert(bit_sum(0b00000001u) == 1u);
static_assert(bit_sum(0b10000000u) == 1u);
static_assert(bit_sum(0b00001000u) == 1u);
static_assert(bit_sum(0b11110000u) == 4u);
static_assert(bit_sum(0b00001111u) == 4u);
static_assert(bit_sum(0b01010101u) == 4u);
static_assert(bit_sum(0b11111111u) == 8u);

static_assert(!is_pow2(0u));
static_assert(is_pow2(1u));
static_assert(is_pow2(2u));
static_assert(!is_pow2(3u));
static_assert(!is_pow2(7u));
static_assert(is_pow2(8u));
static_assert(!is_pow2(9u));
static_assert(!is_pow2(65535u));
static_assert(is_pow2(65536u));
static_assert(!is_pow2(65537u));

static_assert(next_pow2(0u) == 1);
static_assert(next_pow2(1u) == 1);
static_assert(next_pow2(2u) == 2);
static_assert(next_pow2(3u) == 4);
static_assert(next_pow2(4u) == 4);
static_assert(next_pow2(5u) == 8);

static_assert(prev_pow2(0u) == 0);
static_assert(prev_pow2(1u) == 1);
static_assert(prev_pow2(2u) == 2);
static_assert(prev_pow2(3u) == 2);
static_assert(prev_pow2(4u) == 4);
static_assert(prev_pow2(5u) == 4);

static_assert(aligned_size(0, 4) == 0);
static_assert(aligned_size(1, 4) == 4);
static_assert(aligned_size(4, 4) == 4);
static_assert(aligned_size(5, 4) == 8);
static_assert(aligned_size(9, 4) == 12);

/*
    Explicit instantiation of templates to catch compile errors
*/

template u8 bit_sum(u32);

template bool is_pow2(u32);

template u32 next_pow2(u32);

template u32 prev_pow2(u32);

template void unit_square_corner(u8, f64[2]);

template void unit_cube_corner(u8, f64[3]);

} // namespace dr
