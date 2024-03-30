#include <utest/utest.h>

#include <dr/math_types.hpp>
#include <dr/meta.hpp>

UTEST(traits, includes_type)
{
    using namespace dr;

    struct Foo
    {
    };

    struct Bar
    {
    };

    constexpr TypePack<i32, f32, Foo> types{};

    static_assert(types.includes<i32>());
    static_assert(types.includes<f32>());
    static_assert(types.includes<Foo>());

    static_assert(!types.includes<u32>());
    static_assert(!types.includes<f64>());
    static_assert(!types.includes<Bar>());

    ASSERT_TRUE(true);
}

UTEST(traits, join_types)
{
    using namespace dr;

    constexpr TypePack<i8, i16> types_a{};
    constexpr TypePack<i32, i64> types_b{};
    constexpr auto types = types_a.join(types_b);

    static_assert(types.includes<i8>());
    static_assert(types.includes<i16>());
    static_assert(types.includes<i32>());
    static_assert(types.includes<i64>());

    ASSERT_TRUE(true);
}

UTEST(traits, join_values)
{
    using namespace dr;

    constexpr auto vals_a = ValuePack<int, 0, 1>{};
    constexpr auto vals_b = ValuePack<int, 2, 3>{};
    constexpr auto vals = vals_a.join(vals_b);

    static_assert(vals.includes<0>());
    static_assert(vals.includes<1>());
    static_assert(vals.includes<2>());
    static_assert(vals.includes<3>());

    constexpr auto arr = vals.array();
    static_assert(arr.size() == 4);

    static_assert(arr[0] == 0);
    static_assert(arr[1] == 1);
    static_assert(arr[2] == 2);
    static_assert(arr[3] == 3);

    ASSERT_TRUE(true);
}
