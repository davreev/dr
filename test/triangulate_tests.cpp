#include <utest.h>

#include <dr/triangulate.hpp>

UTEST(mesh, fan_triangulator)
{
    using namespace dr;

    struct TestCase
    {
        i32 num_vertices;
        struct
        {
            Vec3<i32> tris[16];
            i32 num_tris;
        } expect;
    };

    TestCase const test_cases[] = {
        {
            3,
            {
                {{0, 1, 2}},
                1,
            },
        },
        {
            4,
            {
                {{0, 1, 2}, {0, 2, 3}},
                2,
            },
        },
        {
            5,
            {
                {{0, 1, 2}, {0, 2, 3}, {0, 3, 4}},
                3,
            },
        },
        {
            6,
            {
                {{0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 5}},
                4,
            },
        },
    };

    for (auto const& [num_vertices, expect] : test_cases)
    {
        FanTriangulator<i32> tri_it{num_vertices};
        i32 tri_count = 0;

        for (; tri_it.is_valid(); tri_it.advance(), ++tri_count)
        {
            Vec3<i32> const a = expect.tris[tri_count];
            Vec3<i32> const b = tri_it.current();
            ASSERT_EQ(a[0], b[0]);
            ASSERT_EQ(a[1], b[1]);
            ASSERT_EQ(a[2], b[2]);
        }

        ASSERT_EQ(expect.num_tris, tri_count);
    }
}

UTEST(mesh, strip_triangulator)
{
    using namespace dr;

    struct TestCase
    {
        i32 num_vertices;
        struct
        {
            Vec3<i32> tris[16];
            i32 num_tris;
        } expect;
    };

    TestCase const test_cases[] = {
        {
            3,
            {
                {{0, 1, 2}},
                1,
            },
        },
        {
            4,
            {
                {{0, 1, 3}, {2, 3, 1}},
                2,
            },
        },
        {
            5,
            {
                {{0, 1, 4}, {2, 4, 1}, {4, 2, 3}},
                3,
            },
        },
        {
            6,
            {
                {{0, 1, 5}, {2, 5, 1}, {5, 2, 4}, {3, 4, 2}},
                4,
            },
        },
    };

    for (auto const& [num_vertices, expect] : test_cases)
    {
        StripTriangulator<i32> tri_it{num_vertices};
        i32 tri_count = 0;

        for (; tri_it.is_valid(); tri_it.advance(), ++tri_count)
        {
            Vec3<i32> const a = expect.tris[tri_count];
            Vec3<i32> const b = tri_it.current();
            ASSERT_EQ(a[0], b[0]);
            ASSERT_EQ(a[1], b[1]);
            ASSERT_EQ(a[2], b[2]);
        }

        ASSERT_EQ(expect.num_tris, tri_count);
    }
}