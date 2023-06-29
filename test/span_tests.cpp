#include <utest.h>

#include <dr/container_utils.hpp>
#include <dr/span.hpp>

UTEST(span, is_valid)
{
    using namespace dr;

    {
        Span<i32 const> span{};
        ASSERT_FALSE(span.is_valid());
    }

    {
        i32 const arr[] = {0, 1, 2, 3, 4};
        Span<i32 const> span{arr, dr::size(arr)};
        ASSERT_TRUE(span.is_valid());
    }
}

UTEST(span, segment)
{
    using namespace dr;

    f64 const arr[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

    auto span = as_span(arr).segment(2, 3);
    ASSERT_EQ(3, span.size());

    ASSERT_EQ(2.0, span[0]);
    ASSERT_EQ(3.0, span[1]);
    ASSERT_EQ(4.0, span[2]);
}

UTEST(span, range)
{
    using namespace dr;

    f64 const arr[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

    auto span = as_span(arr).range(2, 5);
    ASSERT_EQ(3, span.size());

    ASSERT_EQ(2.0, span[0]);
    ASSERT_EQ(3.0, span[1]);
    ASSERT_EQ(4.0, span[2]);
}

UTEST(span, range_base_for)
{
    using namespace dr;

    i32 const arr[] = {0, 1, 2, 3, 4, 5};
    auto span = as_span(arr);
    i32 sum{};

    for (i32 const& x : span)
    {
        sum += x;
    }

    ASSERT_EQ(sum, 15);
}
