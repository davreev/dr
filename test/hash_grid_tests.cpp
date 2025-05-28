#include <utest.h>

#include <algorithm>

#include <dr/defer.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/geometry_types.hpp>
#include <dr/hash_grid.hpp>
#include <dr/math_types.hpp>
#include <dr/memory.hpp>

#include "test_utils.hpp"

UTEST(hash_grid, allocator_propagation)
{
    using namespace dr;

    // Restore default memory resource after test is complete
    auto def_mem = std::pmr::get_default_resource();
    auto _ = defer([=]() {
        std::pmr::set_default_resource(def_mem);
    });

    DebugMemoryResource mem[3]{};
    std::pmr::set_default_resource(&mem[0]);

    HashGrid2<f32> src{&mem[1]};
    src.insert({1.0f, 1.0f}, 0);
    src.insert({2.0f, 2.0f}, 1);

    {
        // dst should use the given memory resource
        HashGrid2<f32> grid_copy{src, &mem[2]};
        ASSERT_TRUE(grid_copy.allocator().resource()->is_equal(mem[2]));
    }

    {
        // dst should use the current default memory resource
        HashGrid2<f32> dst{src};
        ASSERT_TRUE(dst.allocator().resource()->is_equal(mem[0]));
    }

    {
        // dst should use the same memory resource as src
        HashGrid2<f32> grid_move{std::move(src)};
        ASSERT_TRUE(grid_move.allocator().resource()->is_equal(mem[1]));
    }
}

UTEST(hash_grid, insert_find)
{
    using namespace dr;

    struct TestCase
    {
        DynamicArray<Vec3<f64>> points;
        Interval3<f64> interval;
        DynamicArray<i32> result;
    };

    TestCase const test_cases[] = {
        {
            {
                {0.0, 0.0, 0.0},
            },
            {
                {0.0, 0.0, 0.0},
                {1.0, 1.0, 1.0},
            },
            {0},
        },
        {
            {
                {0.0, 0.0, 0.0},
                {0.5, 0.5, 0.5},
                {1.0, 1.0, 1.0},
            },
            {
                {0.0, 0.0, 0.0},
                {1.0, 1.0, 1.0},
            },
            {0, 1, 2},
        },
        {
            {
                {0.0, 0.0, 0.0},
                {2.0, 0.5, 0.5},
                {1.0, 1.0, 1.0},
            },
            {
                {0.0, 0.0, 0.0},
                {1.0, 1.0, 1.0},
            },
            {0, 2},
        },
    };

    for (auto const& [points, interval, result] : test_cases)
    {
        HashGrid3<f64> grid{};

        // Insert points
        i32 index = 0;
        for (auto const& p : points)
            grid.insert(p, index++);

        ASSERT_EQ(grid.size(), size(points));

        DynamicArray<i32> found_indices{};
        grid.find(interval, [&](i32 const i) {
            found_indices.push_back(i);
        });

        std::sort(found_indices.begin(), found_indices.end());

        ASSERT_TRUE(found_indices.size() == result.size());
        ASSERT_TRUE(all_equal(as_span(found_indices), as_span(result)));
    }
}
