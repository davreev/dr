#include <utest.h>

#include <dr/jagged_array.hpp>

#include <dr/shim/pmr/memory_resource.hpp>

UTEST(jagged_array, push_pop_back)
{
    using namespace dr;

    std::pmr::memory_resource* mem = std::pmr::get_default_resource();
    JaggedArray<f64> arr{mem};

    f64 const a[] = {0.0, 1.0, 2.0};
    f64 const b[] = {0.0, 1.0, 2.0, 3.0};
    f64 const c[] = {0.0, 1.0, 2.0, 3.0, 4.0};

    arr.push_back(as_span(a));
    ASSERT_EQ(1, arr.num_blocks());
    ASSERT_EQ(3, arr.num_items());

    arr.push_back(as_span(b));
    ASSERT_EQ(2, arr.num_blocks());
    ASSERT_EQ(7, arr.num_items());

    arr.push_back(as_span(c));
    ASSERT_EQ(3, arr.num_blocks());
    ASSERT_EQ(12, arr.num_items());

    arr.pop_back();
    ASSERT_EQ(2, arr.num_blocks());
    ASSERT_EQ(7, arr.num_items());

    arr.pop_back();
    ASSERT_EQ(1, arr.num_blocks());
    ASSERT_EQ(3, arr.num_items());

    arr.pop_back();
    ASSERT_EQ(0, arr.num_blocks());
    ASSERT_EQ(0, arr.num_items());
}

UTEST(jagged_array, indexer)
{
    using namespace dr;

    std::pmr::memory_resource* mem = std::pmr::get_default_resource();
    JaggedArray<f64> arr{mem};

    f64 const a[] = {0.0, 1.0, 2.0};
    f64 const b[] = {0.0, 1.0, 2.0, 3.0};

    arr.push_back(as_span(a));
    arr.push_back(as_span(b));

    {
        Span<f64 const> const block = arr[0];
        ASSERT_TRUE(std::equal(begin(block), end(block), a));
    }

    {
        Span<f64 const> const block = arr[1];
        ASSERT_TRUE(std::equal(begin(block), end(block), b));
    }
}

UTEST(jagged_array, allocator_propagation)
{
    using namespace dr;

    DebugMemoryResource mem{};

    JaggedArray<f32> arr{&mem};
    arr.reserve(5, 2);
    arr.push_back(3, 1.0);
    arr.push_back(2, 2.0);

    {
        // This copy should use the same memory resource
        JaggedArray<f32> arr_copy{arr, &mem};
        ASSERT_TRUE(arr_copy.allocator().resource()->is_equal(mem));
    }

    {
        // This copy should use the current default memory resource
        JaggedArray<f32> arr_copy{arr};
        ASSERT_TRUE(arr_copy.allocator().resource()->is_equal(*std::pmr::get_default_resource()));
    }

    {
        // Move should use the same memory resource
        JaggedArray<f32> arr_move{std::move(arr)};
        ASSERT_TRUE(arr_move.allocator().resource()->is_equal(mem));
    }
}
