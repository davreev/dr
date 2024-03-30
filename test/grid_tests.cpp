#include <utest/utest.h>

#include <dr/grid.hpp>
#include <dr/math_ctors.hpp>

UTEST(grid, grid_to_index)
{
    using namespace dr;

    struct TestCase
    {
        Vec3<isize> count;
        Vec3<isize> id;
        struct
        {
            isize index;
        } expect;
    };

    TestCase const test_cases[] = {
        {{4, 3, 2}, {0, 0, 0}, {0}},
        {{4, 3, 2}, {1, 0, 0}, {1}},
        {{4, 3, 2}, {0, 1, 0}, {4}},
        {{4, 3, 2}, {0, 0, 1}, {12}},
        {{4, 3, 2}, {1, 1, 0}, {5}},
        {{4, 3, 2}, {1, 0, 1}, {13}},
        {{4, 3, 2}, {0, 1, 1}, {16}},
        {{4, 3, 2}, {1, 1, 1}, {17}},
    };

    for (auto const& [count, id, expect] : test_cases)
    {
        isize const index = grid_to_index(id, grid_stride(count));
        ASSERT_EQ(expect.index, index);
    }
}

UTEST(grid, index_to_grid)
{
    using namespace dr;

    struct TestCase
    {
        Vec3<isize> count;
        isize index;
        struct
        {
            Vec3<isize> id;
        } expect;
    };

    TestCase const test_cases[] = {
        {{4, 3, 2}, 0, {{0, 0, 0}}},
        {{4, 3, 2}, 1, {{1, 0, 0}}},
        {{4, 3, 2}, 4, {{0, 1, 0}}},
        {{4, 3, 2}, 12, {{0, 0, 1}}},
        {{4, 3, 2}, 5, {{1, 1, 0}}},
        {{4, 3, 2}, 13, {{1, 0, 1}}},
        {{4, 3, 2}, 16, {{0, 1, 1}}},
        {{4, 3, 2}, 17, {{1, 1, 1}}},
    };

    for (auto const& [count, index, expect] : test_cases)
    {
        Vec3<isize> const id = index_to_grid(index, grid_stride(count));
        ASSERT_EQ(expect.id[0], id[0]);
        ASSERT_EQ(expect.id[1], id[1]);
        ASSERT_EQ(expect.id[2], id[2]);
    }
}

UTEST(grid, grid_to_world)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Grid3<f64> grid;
        Vec3<f64> grid_pt;
        struct
        {
            Vec3<f64> world_pt;
        } expect;
    };

    TestCase const test_cases[] = {
        {
            {{4, 3, 2}, vec<3>(10.0), vec<3>(1.0)},
            {0.0, 0.0, 0.0},
            {{1.0, 1.0, 1.0}},
        },
        {
            {{4, 3, 2}, vec<3>(10.0), vec<3>(1.0)},
            {0.5, 0.5, 0.5},
            {{6.0, 6.0, 6.0}},
        },
        {
            {{4, 3, 2}, vec<3>(10.0), vec<3>(1.0)},
            {1.0, 1.0, 1.0},
            {{11.0, 11.0, 11.0}},
        },
        {
            {{4, 3, 2}, vec<3>(10.0), vec<3>(1.0)},
            {1.0, 0.5, 2.0},
            {{11.0, 6.0, 21.0}},
        },
    };

    for (auto const& [grid, grid_pt, expect] : test_cases)
    {
        Vec3<f64> const world_pt = grid.to_world(grid_pt);
        ASSERT_NEAR(expect.world_pt[0], world_pt[0], eps);
        ASSERT_NEAR(expect.world_pt[1], world_pt[1], eps);
        ASSERT_NEAR(expect.world_pt[2], world_pt[2], eps);
    }
}

UTEST(grid, world_to_grid)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Grid3<f64> grid;
        Vec3<f64> world_pt;
        struct
        {
            Vec3<f64> grid_pt;
        } expect;
    };

    TestCase const test_cases[] = {
        {
            {{4, 3, 2}, vec<3>(10.0), vec<3>(1.0)},
            {1.0, 1.0, 1.0},
            {{0.0, 0.0, 0.0}},
        },
        {
            {{4, 3, 2}, vec<3>(10.0), vec<3>(1.0)},
            {6.0, 6.0, 6.0},
            {{0.5, 0.5, 0.5}},
        },
        {
            {{4, 3, 2}, vec<3>(10.0), vec<3>(1.0)},
            {11.0, 11.0, 11.0},
            {{1.0, 1.0, 1.0}},
        },
        {
            {{4, 3, 2}, vec<3>(10.0), vec<3>(1.0)},
            {11.0, 6.0, 21.0},
            {{1.0, 0.5, 2.0}},
        },
    };

    for (auto const& [grid, world_pt, expect] : test_cases)
    {
        Vec3<f64> const grid_pt = grid.to_grid(world_pt);
        ASSERT_NEAR(expect.grid_pt[0], grid_pt[0], eps);
        ASSERT_NEAR(expect.grid_pt[1], grid_pt[1], eps);
        ASSERT_NEAR(expect.grid_pt[2], grid_pt[2], eps);
    }
}