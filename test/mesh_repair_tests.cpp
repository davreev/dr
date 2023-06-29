#include <utest.h>

#include <dr/dynamic_array.hpp>
#include <dr/mesh_repair.hpp>

#include "test_utils.hpp"

UTEST(mesh_repair, find_unique_points)
{
    using namespace dr;

    struct TestCase
    {
        DynamicArray<Vec2<f64>> points;
        f64 tolerance;
        struct
        {
            DynamicArray<i32> unique_points;
            DynamicArray<i32> point_to_unique;
        } result;
    };

    TestCase const test_cases[] = {
        {
            {
                {0.0, 0.0},
                {1.0, 1.0},
                {2.0, 2.0},
            },
            1.0e-8,
            {
                {0, 1, 2},
                {0, 1, 2},
            },
        },
        {
            {
                {0.0, 0.0},
                {1.0, 1.0},
                {1.0, 1.0},
            },
            1.0e-8,
            {
                {0, 1},
                {0, 1, 1},
            },
        },
        {
            {
                {0.0, 0.0},
                {0.0, 0.0},
                {0.0, 0.0},
            },
            1.0e-8,
            {
                {0},
                {0, 0, 0},
            },
        },
        {
            {
                {-1.0, -1.0},
                {1.0, 1.0},
                {-1.0, -1.0},
                {1.0, 1.0},
            },
            1.0e-8,
            {
                {0, 1},
                {0, 1, 0, 1},
            },
        },
        {
            {
                {0.0, 0.0},
                {0.1, 0.1},
                {0.2, 0.2},
                {0.3, 0.3},
                {0.4, 0.4},
                {0.5, 0.5},
                {0.6, 0.6},
                {0.7, 0.7},
                {0.8, 0.8},
                {0.9, 0.9},
            },
            0.15,
            {
                {0, 2, 4, 6, 8},
                {0, 0, 1, 1, 2, 2, 3, 3, 4, 4},
            },
        },
    };

    DynamicArray<i32> unique_pts{};
    DynamicArray<i32> pt_to_unique{};

    // Deduplication via sort
    for (auto const& [points, tol, result] : test_cases)
    {
        unique_pts.clear();
        pt_to_unique.assign(points.size(), -1);
        find_unique_points(as_span(points), tol, unique_pts, as_span(pt_to_unique));

        ASSERT_TRUE(equal(as_span(pt_to_unique), as_span(result.point_to_unique)));
        ASSERT_TRUE(equal(as_span(unique_pts), as_span(result.unique_points)));
    }

    HashGrid2<f64> grid{};

    // Deduplication via hash grid
    for (auto const& [points, tol, result] : test_cases)
    {
        unique_pts.clear();
        pt_to_unique.assign(points.size(), -1);
        find_unique_points(as_span(points), grid, tol, unique_pts, as_span(pt_to_unique));

        ASSERT_TRUE(equal(as_span(pt_to_unique), as_span(result.point_to_unique)));
        ASSERT_TRUE(equal(as_span(unique_pts), as_span(result.unique_points)));
    }
}

UTEST(mesh_repair, gather_points)
{
    using namespace dr;

    struct TestCase
    {
        DynamicArray<Vec2<f64>> points;
        f64 radius_start;
        f64 radius_end;
        struct
        {
            isize num_unique_before;
            isize num_unique_after;
        } result;
    };

    TestCase const test_cases[] = {
        {
            {
                {0.0, 0.0},
                {0.1, 0.0},
                {0.0, 0.1},
                {0.1, 0.1},

                {1.0, 0.0},
                {1.1, 0.0},
                {1.0, 0.1},
                {1.1, 0.1},
            },
            0.2,
            0.1,
            {8, 2},
        },
        {
            {
                {0.0, 0.0},
                {0.5, 0.0},
                {0.0, 0.5},
                {0.5, 0.5},

                {1.0, 0.0},
                {1.5, 0.0},
                {1.0, 0.5},
                {1.5, 0.5},
            },
            0.6,
            0.1,
            {8, 2},
        },
    };

    constexpr f64 rel_tol = 1.0e-3;

    for (auto const& [points_in, rad_start, rad_end, result] : test_cases)
    {
        DynamicArray<Vec2<f64>> points = points_in;
        f64 const tol = rad_end * rel_tol;

        HashGrid2<f64> grid{};
        DynamicArray<i32> unique_pts{};
        DynamicArray<i32> pt_to_unique(points.size(), -1);

        {
            find_unique_points(
                as_span(points).as_const(),
                grid,
                tol,
                unique_pts,
                as_span(pt_to_unique));

            ASSERT_EQ(dr::size(unique_pts), result.num_unique_before);
        }

        points = points_in;
        bool const converged = gather_points(as_span(points), grid, rad_start, rad_end, 10);
        ASSERT_TRUE(converged);

        {
            find_unique_points(
                as_span(points).as_const(),
                grid,
                tol,
                unique_pts,
                as_span(pt_to_unique));

            ASSERT_EQ(dr::size(unique_pts), result.num_unique_after);
        }
    }
}

UTEST(mesh_repair, merge_vertices)
{
    using namespace dr;

    struct TestCase
    {
        DynamicArray<Vec2<f64>> vertices;
        DynamicArray<Vec3<i32>> faces;
        struct
        {
            DynamicArray<Vec3<i32>> merged_faces;
        } result;
    };

    TestCase const test_cases[] = {
        {
            {
                {0.0, 0.0},
                {1.0, 0.0},
                {0.0, 0.0},
            },
            {
                {0, 1, 2},
            },
            {
                {},
            },
        },
        {
            {
                {0.0, 0.0},
                {1.0, 0.0},
                {0.0, 0.0},
                {1.0, 1.0},
            },
            {
                {0, 1, 2},
                {3, 2, 1},
            },
            {
                {
                    {2, 0, 1},
                },
            },
        },
        {
            {
                {0.0, 0.0},
                {1.0, 0.0},
                {0.0, 0.0},
                {1.0, 1.0},
                {0.0, 2.0},
                {1.0, 2.0},
            },
            {
                {0, 1, 2},
                {3, 2, 1},
                {2, 3, 4},
                {5, 4, 3},
            },
            {
                {
                    {2, 0, 1},
                    {0, 2, 3},
                    {4, 3, 2},
                },
            },
        },
    };

    constexpr f64 eps = 1.0e-8;

    for (auto const& [verts_in, faces_in, result] : test_cases)
    {
        DynamicArray<Vec2<f64>> verts = verts_in;
        DynamicArray<Vec3<i32>> faces = faces_in;

        HashGrid2<f64> grid{};
        DynamicArray<i32> unique_verts{};
        DynamicArray<i32> vert_to_unique(verts.size(), -1);
        find_unique_points(
            as_span(verts).as_const(),
            grid,
            eps,
            unique_verts,
            as_span(vert_to_unique));

        auto merged_faces = reindex_faces(as_span(faces), as_span(vert_to_unique).as_const(), true);
        ASSERT_TRUE(equal(merged_faces, as_span(result.merged_faces)));
    }
}
