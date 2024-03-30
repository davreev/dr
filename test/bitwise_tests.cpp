#include <utest/utest.h>

#include <dr/bitwise.hpp>

UTEST(bitwise, bit_sum)
{
    using namespace dr;

    struct TestCase
    {
        u8 value;
        u8 expect;
    };

    TestCase test_cases[]{
        {0b00000001, 1u},
        {0b10000000, 1u},
        {0b00001111, 4u},
        {0b11110000, 4u},
        {0b01010101, 4u},
        {0b11111111, 8u},
    };

    for (auto& [value, expect] : test_cases)
    {
        ASSERT_EQ(expect, bit_sum(value));
    }
}

UTEST(bitwise, is_pow2)
{
    using namespace dr;

    ASSERT_FALSE(is_pow2(0u));
    ASSERT_TRUE(is_pow2(1u));
    ASSERT_TRUE(is_pow2(2u));

    for (u8 i = 2; i < 63; ++i)
    {
        u64 const x = u64{1} << i;
        ASSERT_TRUE(is_pow2(x));
        ASSERT_FALSE(is_pow2(x - 1));
        ASSERT_FALSE(is_pow2(x + 1));
    }
}

UTEST(bitwise, next_pow2)
{
    using namespace dr;

    ASSERT_EQ(0u, next_pow2(0u));

    for (u8 i = 0; i < 63; ++i)
    {
        u64 const x = u64{1} << i;
        ASSERT_EQ(x, next_pow2(x));
        ASSERT_EQ(x << 1, next_pow2(x + 1));
    }
}

UTEST(bitwise, prev_pow2)
{
    using namespace dr;

    ASSERT_EQ(0u, prev_pow2(0u));

    for (u8 i = 0; i < 63; ++i)
    {
        u64 const x = u64{1} << i;
        ASSERT_EQ(x, prev_pow2(x));
        ASSERT_EQ(x >> 1, prev_pow2(x - 1));
    }
}
