#include <utest.h>

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <dr/container_traits.hpp>
#include <dr/container_utils.hpp>

UTEST(container_utils, carray_to_span)
{
    using namespace dr;
    
    {
        f64 arr[] = {0.0, 1.0, 2.0, 3.0, 4.0};
        Span<f64> span = as_span(arr);
        ASSERT_EQ(5, span.size());
        ASSERT_TRUE(std::equal(begin(span), end(span), arr));
    }

    {
        f64 const arr[] = {0.0, 1.0, 2.0, 3.0, 4.0};
        Span<f64 const> const span = as_span(arr);
        ASSERT_EQ(5, span.size());
        ASSERT_TRUE(std::equal(begin(span), end(span), arr));
    }
}

UTEST(container_traits, is_contiguous)
{
    using namespace dr;
    
    ASSERT_TRUE(is_contiguous<std::vector<int>>);
    ASSERT_TRUE(is_contiguous<std::vector<float>>);
    ASSERT_FALSE(is_contiguous<std::vector<bool>>);
    
    {
        using Array = std::array<int, 100>;
        ASSERT_TRUE(is_contiguous<Array>);
    }

    {
        using Array = std::array<int, 100>;
        ASSERT_TRUE(is_contiguous<Array>);
    }

    {
        using Array = std::array<int, 100>;
        ASSERT_TRUE(is_contiguous<Array>);
    }

    ASSERT_TRUE(is_contiguous<std::string>);

    {
        using Map = std::unordered_map<int, int>;
        ASSERT_FALSE(is_contiguous<Map>);
    }

    ASSERT_FALSE(is_contiguous<std::unordered_set<int>>);
}
