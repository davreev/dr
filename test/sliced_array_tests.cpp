#include <utest.h>

#include <dr/defer.hpp>
#include <dr/memory.hpp>
#include <dr/sliced_array.hpp>

UTEST(sliced_array, push_pop_back)
{
    using namespace dr;

    SlicedArray<f64> arr{};

    f64 const a[] = {0.0, 1.0, 2.0};
    f64 const b[] = {0.0, 1.0, 2.0, 3.0};
    f64 const c[] = {0.0, 1.0, 2.0, 3.0, 4.0};

    arr.push_back(as_span(a));
    ASSERT_EQ(1, arr.num_slices());
    ASSERT_EQ(3, arr.num_items());

    arr.push_back(as_span(b));
    ASSERT_EQ(2, arr.num_slices());
    ASSERT_EQ(7, arr.num_items());

    arr.push_back(as_span(c));
    ASSERT_EQ(3, arr.num_slices());
    ASSERT_EQ(12, arr.num_items());

    arr.pop_back();
    ASSERT_EQ(2, arr.num_slices());
    ASSERT_EQ(7, arr.num_items());

    arr.pop_back();
    ASSERT_EQ(1, arr.num_slices());
    ASSERT_EQ(3, arr.num_items());

    arr.pop_back();
    ASSERT_EQ(0, arr.num_slices());
    ASSERT_EQ(0, arr.num_items());
}

UTEST(sliced_array, indexer)
{
    using namespace dr;

    SlicedArray<f64> arr{};

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

UTEST(sliced_array, allocator_propagation)
{
    using namespace dr;

    // Restore default memory resource after test is complete
    auto def_mem = std::pmr::get_default_resource();
    auto _ = defer([=]() {
        std::pmr::set_default_resource(def_mem);
    });

    DebugMemoryResource mem[3]{};
    std::pmr::set_default_resource(&mem[0]);

    SlicedArray<f32> src{&mem[1]};
    src.reserve(5, 2);
    src.push_back(3, 1.0);
    src.push_back(2, 2.0);

    {
        // dst should use the given memory resource
        SlicedArray<f32> dst{src, &mem[2]};
        ASSERT_TRUE(dst.allocator().resource()->is_equal(mem[2]));
    }

    {
        // dst should use the current default memory resource
        SlicedArray<f32> dst{src};
        ASSERT_TRUE(dst.allocator().resource()->is_equal(mem[0]));
    }

    {
        // dst should use the same memory resource as src
        SlicedArray<f32> dst{std::move(src)};
        ASSERT_TRUE(dst.allocator().resource()->is_equal(mem[1]));
    }
}
