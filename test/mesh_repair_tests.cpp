#include <utest.h>

#include <vector>

#include <dr/mesh_repair.hpp>

#include "test_utils.hpp"

UTEST(mesh_repair, find_unique_points)
{
    using namespace dr;

    struct TestCase
    {
        std::vector<Vec2<f64>> points;
        f64 tolerance;
        struct
        {
            std::vector<i32> to_unique;
            std::vector<i32> from_unique;
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
                {0, 1, 1},
                {0, 1},
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
                {0, 0, 0},
                {0},
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
                {0, 1, 0, 1},
                {0, 1},
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
                {0, 0, 1, 1, 2, 2, 3, 3, 4, 4},
                {0, 2, 4, 6, 8},
            },
        },
    };

    for (auto const& [points, tol, result] : test_cases)
    {
        std::vector<i32> to_unique(points.size(), -1);
        std::pmr::vector<i32> from_unique{};
        find_unique_points(as_span(points), tol, as_span(to_unique), from_unique);

        ASSERT_TRUE(equal(as_span(to_unique), as_span(result.to_unique)));
        ASSERT_TRUE(equal(as_span(from_unique), as_span(result.from_unique)));

        HashGrid2<f64> grid{};
        find_unique_points(grid, as_span(points), tol, as_span(to_unique), from_unique);

        ASSERT_TRUE(equal(as_span(to_unique), as_span(result.to_unique)));
        ASSERT_TRUE(equal(as_span(from_unique), as_span(result.from_unique)));
    }
}

UTEST(mesh_repair, gather_points)
{
    using namespace dr;

    struct TestCase
    {
        std::vector<Vec2<f64>> points;
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
        std::vector<Vec2<f64>> points = points_in;
        f64 const tol = rad_end * rel_tol;

        HashGrid2<f64> grid{};
        std::vector<i32> to_unique(points.size(), -1);
        std::pmr::vector<i32> from_unique{};

        {
            find_unique_points(grid, as_span(points).as_const(), tol, as_span(to_unique), from_unique);
            ASSERT_EQ(size<isize>(from_unique), result.num_unique_before);
        }

        points = points_in;
        bool const converged = gather_points(as_span(points), grid, rad_start, rad_end, 10);
        ASSERT_TRUE(converged);

        {
            find_unique_points(grid, as_span(points).as_const(), tol, as_span(to_unique), from_unique);
            ASSERT_EQ(size<isize>(from_unique), result.num_unique_after);
        }
    }
}

UTEST(mesh_repair, merge_vertices)
{
    using namespace dr;

    struct TestCase
    {
        std::vector<Vec2<f64>> vertices;
        std::vector<Vec3<i32>> faces;
        struct
        {
            std::vector<Vec3<i32>> faces_merged;
            std::vector<Vec3<i32>> faces_cleaned;
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
                {
                    {0, 1, 0},
                },
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
                    {0, 1, 0},
                    {2, 0, 1},
                },
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
                    {0, 1, 0},
                    {2, 0, 1},
                    {0, 2, 3},
                    {4, 3, 2},
                },
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
        std::vector<Vec2<f64>> verts = verts_in;
        std::vector<Vec3<i32>> faces = faces_in;

        HashGrid2<f64> grid{};
        std::vector<i32> to_unique(verts.size(), -1);
        std::pmr::vector<i32> from_unique{};
        find_unique_points(grid, as_span(verts).as_const(), eps, as_span(to_unique), from_unique);

        // NOTE: Can use the same buffer for output vertices here since from_unique is monotonic
        merge_vertices(
            as_span(verts).as_const(), 
            as_span(to_unique), 
            as_span(from_unique), 
            as_span(faces), 
            as_span(verts).front(from_unique.size()));

        ASSERT_TRUE(equal(as_span(faces), as_span(result.faces_merged)));

        auto faces_cleaned = as_span(faces);
        remove_degenerate_faces(faces_cleaned);

        ASSERT_TRUE(equal(faces_cleaned, as_span(result.faces_cleaned)));
    }
}
