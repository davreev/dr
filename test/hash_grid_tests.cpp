#include <utest.h>

#include <algorithm>

#include <dr/dynamic_array.hpp>
#include <dr/geometry_types.hpp>
#include <dr/hash_grid.hpp>
#include <dr/math_types.hpp>
#include <dr/memory.hpp>

#include "test_utils.hpp"

UTEST(hash_grid, allocator_propagation)
{
    using namespace dr;

    DebugMemoryResource mem{};

    HashGrid2<f32> grid{&mem};
    grid.insert({1.0f, 1.0f}, 0);
    grid.insert({2.0f, 2.0f}, 1);

    {
        // This copy should use the same memory resource
        HashGrid2<f32> grid_copy{grid, &mem};
        ASSERT_TRUE(grid_copy.allocator().resource()->is_equal(mem));
    }

    {
        // This copy should use the current default memory resource
        HashGrid2<f32> grid_copy{grid};
        ASSERT_TRUE(grid_copy.allocator().resource()->is_equal(*std::pmr::get_default_resource()));
    }

    {
        // This copy should use the current default memory resource
        HashGrid2<f32> grid_move{std::move(grid)};
        ASSERT_TRUE(grid_move.allocator().resource()->is_equal(mem));
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
        grid.find(interval, [&](i32 const i) { found_indices.push_back(i); });

        std::sort(found_indices.begin(), found_indices.end());

        ASSERT_TRUE(found_indices.size() == result.size());
        ASSERT_TRUE(all_equal(as_span(found_indices), as_span(result)));
    }
}
