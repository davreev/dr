#include <utest.h>

#include <dr/dynamic_array.hpp>
#include <dr/geometry.hpp>
#include <dr/result.hpp>

UTEST(geometry, eval_gradient_tri)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> p[3];
        f64 f[3];
        Covec3<f64> expect;
    };

    TestCase const test_cases[] = {
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            {0.0, 0.0, 1.0},
            vec(0.0, 1.0, 0.0),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            {0.0, 0.0, 2.0},
            vec(0.0, 2.0, 0.0),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 2.0, 0.0),
            },
            {0.0, 0.0, 1.0},
            vec(0.0, 0.5, 0.0),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            {1.0, 1.0, 1.0},
            vec(0.0, 0.0, 0.0),
        },
    };

    for (auto const& [p, f, expect] : test_cases)
    {
        auto const grad = eval_gradient(p[0], p[1], p[2], f[0], f[1], f[2]);
        ASSERT_NEAR(expect[0], grad[0], eps);
        ASSERT_NEAR(expect[1], grad[1], eps);
        ASSERT_NEAR(expect[2], grad[2], eps);
    }
}

UTEST(geometry, eval_jacobian_tri)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> p[3];
        Vec2<f64> f[3];
        Mat<f64, 2, 3> expect;
    };

    TestCase const test_cases[] = {
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            {
                vec(0.0, 0.0),
                vec(0.0, 0.0),
                vec(1.0, 1.0),
            },
            mat(row(0.0, 1.0, 0.0), row(0.0, 1.0, 0.0)),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            {
                vec(0.0, 0.0),
                vec(0.0, 0.0),
                vec(1.0, 2.0),
            },
            mat(row(0.0, 1.0, 0.0), row(0.0, 2.0, 0.0)),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 2.0, 0.0),
            },
            {
                vec(0.0, 0.0),
                vec(0.0, 0.0),
                vec(1.0, 2.0),
            },
            mat(row(0.0, 0.5, 0.0), row(0.0, 1.0, 0.0)),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            {
                vec(1.0, 2.0),
                vec(1.0, 2.0),
                vec(1.0, 2.0),
            },
            mat(row(0.0, 0.0, 0.0), row(0.0, 0.0, 0.0)),
        },
    };

    for (auto const& [p, f, expect] : test_cases)
    {
        auto const J = eval_jacobian(p[0], p[1], p[2], f[0], f[1], f[2]);
        ASSERT_NEAR(expect(0, 0), J(0, 0), eps);
        ASSERT_NEAR(expect(0, 1), J(0, 1), eps);
        ASSERT_NEAR(expect(0, 2), J(0, 2), eps);
        ASSERT_NEAR(expect(1, 0), J(1, 0), eps);
        ASSERT_NEAR(expect(1, 1), J(1, 1), eps);
        ASSERT_NEAR(expect(1, 2), J(1, 2), eps);
    }
}

UTEST(geometry, eval_gradient_tet)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> p[4];
        f64 f[4];
        Covec3<f64> expect;
    };

    TestCase const test_cases[] = {
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
                vec(0.0, 0.0, 1.0),
            },
            {0.0, 0.0, 0.0, 1.0},
            vec(0.0, 0.0, 1.0),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
                vec(0.0, 0.0, 1.0),
            },
            {0.0, 0.0, 0.0, 2.0},
            vec(0.0, 0.0, 2.0),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
                vec(0.0, 0.0, 2.0),
            },
            {0.0, 0.0, 0.0, 1.0},
            vec(0.0, 0.0, 0.5),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
                vec(0.0, 0.0, 1.0),
            },
            {1.0, 1.0, 1.0, 1.0},
            vec(0.0, 0.0, 0.0),
        },
    };

    for (auto const& [p, f, expect] : test_cases)
    {
        auto const grad = eval_gradient(p[0], p[1], p[2], p[3], f[0], f[1], f[2], f[3]);
        ASSERT_NEAR(expect[0], grad[0], eps);
        ASSERT_NEAR(expect[1], grad[1], eps);
        ASSERT_NEAR(expect[2], grad[2], eps);
    }
}

UTEST(geometry, eval_jacobian_tet)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> p[4];
        Vec2<f64> f[4];
        Mat<f64, 2, 3> expect;
    };

    TestCase const test_cases[] = {
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
                vec(0.0, 0.0, 1.0),
            },
            {
                vec(0.0, 0.0),
                vec(0.0, 0.0),
                vec(0.0, 0.0),
                vec(1.0, 1.0),
            },
            mat(row(0.0, 0.0, 1.0), row(0.0, 0.0, 1.0)),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
                vec(0.0, 0.0, 1.0),
            },
            {
                vec(0.0, 0.0),
                vec(0.0, 0.0),
                vec(0.0, 0.0),
                vec(1.0, 2.0),
            },
            mat(row(0.0, 0.0, 1.0), row(0.0, 0.0, 2.0)),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
                vec(0.0, 0.0, 2.0),
            },
            {
                vec(0.0, 0.0),
                vec(0.0, 0.0),
                vec(0.0, 0.0),
                vec(1.0, 2.0),
            },
            mat(row(0.0, 0.0, 0.5), row(0.0, 0.0, 1.0)),
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
                vec(0.0, 0.0, 1.0),
            },
            {
                vec(1.0, 2.0),
                vec(1.0, 2.0),
                vec(1.0, 2.0),
                vec(1.0, 2.0),
            },
            mat(row(0.0, 0.0, 0.0), row(0.0, 0.0, 0.0)),
        },
    };

    for (auto const& [p, f, expect] : test_cases)
    {
        auto const J = eval_jacobian(p[0], p[1], p[2], p[3], f[0], f[1], f[2], f[3]);
        ASSERT_NEAR(expect(0, 0), J(0, 0), eps);
        ASSERT_NEAR(expect(0, 1), J(0, 1), eps);
        ASSERT_NEAR(expect(0, 2), J(0, 2), eps);
        ASSERT_NEAR(expect(1, 0), J(1, 0), eps);
        ASSERT_NEAR(expect(1, 1), J(1, 1), eps);
        ASSERT_NEAR(expect(1, 2), J(1, 2), eps);
    }
}

UTEST(geometry, eval_divergence)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> p[3];
        Vec3<f64> f;
        f64 expect[3];
    };

    /*
        Testing on unit triangle

               v2
               |\          |\
               | \      e2 | \ e1
               |  \        |  \
               -----       -----
             v0     v1      e0

        Cotan weights

            w[e0] = w[e2] = cos(pi/4) / sin(pi/4) = 1
            w[e1] = cos(pi/2) / sin(pi/2) = 0
    */

    TestCase const test_cases[] = {
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            vec(1.0, 0.0, 0.0),
            {0.5, -0.5, 0.0},
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            vec(1.0, 1.0, 0.0),
            {1.0, -0.5, -0.5},
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            vec(1.0, -1.0, 0.0),
            {0.0, -0.5, 0.5},
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            vec(1.0, 2.0, 0.0),
            {1.5, -0.5, -1.0},
        },
    };

    for (auto const& [p, f, expect] : test_cases)
    {
        f64 div[3]{};
        eval_divergence(p[0], p[1], p[2], f, div[0], div[1], div[2]);
        ASSERT_NEAR(expect[0], div[0], eps);
        ASSERT_NEAR(expect[1], div[1], eps);
        ASSERT_NEAR(expect[2], div[2], eps);
    }
}

UTEST(geometry, eval_laplacian)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> p[3];
        f64 f[3];
        f64 expect[3];
    };

    /*
        Testing on unit triangle

               v2
               |\          |\
               | \      e2 | \ e1
               |  \        |  \
               -----       -----
             v0     v1      e0

        Cotan weights

            w[e0] = w[e2] = cos(pi/4) / sin(pi/4) = 1
            w[e1] = cos(pi/2) / sin(pi/2) = 0
    */

    TestCase const test_cases[] = {
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            {1.0, 1.0, 1.0},
            {0.0, 0.0, 0.0},
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            {1.0, 0.0, 0.0},
            {-1.0, 0.5, 0.5},
        },
        {
            {
                vec(0.0, 0.0, 0.0),
                vec(1.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0),
            },
            {0.0, 1.0, 2.0},
            {1.5, -0.5, -1.0},
        },
    };

    for (auto const& [p, f, expect] : test_cases)
    {
        f64 lap[3]{};
        eval_laplacian(p[0], p[1], p[2], f[0], f[1], f[2], lap[0], lap[1], lap[2]);
        ASSERT_NEAR(expect[0], lap[0], eps);
        ASSERT_NEAR(expect[1], lap[1], eps);
        ASSERT_NEAR(expect[2], lap[2], eps);
    }
}

UTEST(geometry, nearest_point_line)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> point;
        Vec3<f64> line_start;
        Vec3<f64> line_delta;
        f64 expect;
    };

    TestCase const test_cases[] = {
        {
            vec(0.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            0.0,
        },
        {
            vec(-1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            -1.0,
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            1.0,
        },
        {
            vec(2.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            2.0,
        },
        {
            vec(0.0, 0.0, 0.0),
            vec(0.0, -1.0, -1.0),
            vec(0.0, 2.0, 2.0),
            0.5,
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(0.0, -1.0, -1.0),
            vec(0.0, 2.0, 2.0),
            0.5,
        },
    };

    for (auto const& [p, ln_start, ln_delta, expect] : test_cases)
    {
        auto const t = nearest_point_line(p, ln_start, ln_delta);
        ASSERT_NEAR(expect, t, eps);
    }
}

UTEST(geometry, nearest_point_plane)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> point;
        Vec3<f64> plane_orig;
        Vec3<f64> plane_norm;
        Vec3<f64> expect;
    };

    TestCase const test_cases[] = {
        {
            vec(0.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(0.0, 0.0, 0.0),
        },
        {
            vec(1.0, 1.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(1.0, 1.0, 0.0),
        },
        {
            vec(0.0, 0.0, 0.0),
            vec(0.0, 0.0, -1.0),
            vec(0.0, 0.0, 1.0),
            vec(0.0, 0.0, -1.0),
        },
        {
            vec(1.0, 1.0, 0.0),
            vec(0.0, 0.0, -1.0),
            vec(0.0, 0.0, 1.0),
            vec(1.0, 1.0, -1.0),
        },
    };

    for (auto const& [p, pl_orig, pl_norm, expect] : test_cases)
    {
        auto const cp = nearest_point_plane(p, pl_orig, pl_norm);
        ASSERT_NEAR(expect[0], cp[0], eps);
        ASSERT_NEAR(expect[1], cp[1], eps);
        ASSERT_NEAR(expect[2], cp[2], eps);
    }
}

UTEST(geometry, nearest_line_line)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> a_start;
        Vec3<f64> a_delta;
        Vec3<f64> b_start;
        Vec3<f64> b_delta;
        Vec2<f64> expect;
    };

    TestCase const test_cases[] = {
        {
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0),
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(-1.0, 0.0),
        },
        {
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0),
        },
        {
            vec(-1.0, -1.0, 0.0),
            vec(2.0, 2.0, 0.0),
            vec(1.0, -1.0, 0.0),
            vec(-2.0, 2.0, 0.0),
            vec(0.5, 0.5),
        },
        {
            vec(-1.0, -1.0, -1.0),
            vec(2.0, 2.0, 0.0),
            vec(1.0, -1.0, 1.0),
            vec(-2.0, 2.0, 0.0),
            vec(0.5, 0.5),
        },
    };

    for (auto const& [a_start, a_delta, b_start, b_delta, expect] : test_cases)
    {
        auto const t = nearest_line_line(a_start, a_delta, b_start, b_delta);
        ASSERT_NEAR(expect[0], t[0], eps);
        ASSERT_NEAR(expect[1], t[1], eps);
    }
}

UTEST(geometry, intersect_line_line)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec2<f64> a_start;
        Vec2<f64> a_delta;
        Vec2<f64> b_start;
        Vec2<f64> b_delta;
        Maybe<Vec2<f64>> expect;
    };

    TestCase const test_cases[] = {
        {
            vec(0.0, 0.0),
            vec(1.0, 0.0),
            vec(0.0, 0.0),
            vec(0.0, 1.0),
            vec(0.0, 0.0),
        },
        {
            vec(0.0, 0.0),
            vec(1.0, 1.0),
            vec(1.0, 0.0),
            vec(-1.0, 1.0),
            vec(0.5, 0.5),
        },
        {
            vec(0.5, 0.5),
            vec(1.0, 1.0),
            vec(1.0, 0.0),
            vec(-1.0, 1.0),
            vec(0.0, 0.5),
        },
        {
            vec(0.0, 0.0),
            vec(2.0, 2.0),
            vec(1.0, 0.0),
            vec(-2.0, 2.0),
            vec(0.25, 0.25),
        },
        {
            vec(0.0, 0.0),
            vec(1.0, 0.0),
            vec(0.0, 1.0),
            vec(1.0, 0.0),
            {},
        },
    };

    for (auto const& [a_start, a_delta, b_start, b_delta, expect] : test_cases)
    {
        auto const t = intersect_line_line(a_start, a_delta, b_start, b_delta);

        if (t.has_value)
        {
            ASSERT_NEAR(expect.value[0], t.value[0], eps);
            ASSERT_NEAR(expect.value[1], t.value[1], eps);
        }
    }
}

UTEST(geometry, intersect_line_plane)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> line_start;
        Vec3<f64> line_delta;
        Vec3<f64> plane_orig;
        Vec3<f64> plane_norm;
        Maybe<f64> expect;
    };

    TestCase const test_cases[] = {
        {
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            0.0,
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            -1.0,
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(0.5, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            -2.0,
        },
        {
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            {},
        },
    };

    for (auto const& [ln_start, ln_delta, pl_orig, pl_norm, expect] : test_cases)
    {
        auto const t = intersect_line_plane(ln_start, ln_delta, pl_orig, pl_norm);

        if (t.has_value)
        {
            ASSERT_TRUE(expect.has_value);
            ASSERT_NEAR(expect.value, t.value, eps);
        }
    }
}

UTEST(geometry, intersect_line_sphere)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;
    constexpr f64 inv_sqrt2 = 1.0 / sqrt2<f64>;

    struct TestCase
    {
        Vec3<f64> line_start;
        Vec3<f64> line_delta;
        Vec3<f64> sphere_orig;
        f64 sphere_rad;
        Maybe<Vec2<f64>> expect;
    };

    TestCase const test_cases[] = {
        {
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            1.0,
            vec(-1.0, 1.0),
        },
        {
            vec(2.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            1.0,
            vec(-3.0, -1.0),
        },
        {
            vec(2.0, 0.0, 0.0),
            vec(2.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            1.0,
            vec(-1.5, -0.5),
        },
        {
            vec(0.0, 0.0, 0.0),
            vec(1.0, 1.0, 0.0),
            vec(0.0, 0.0, 0.0),
            1.0,
            vec(-inv_sqrt2, inv_sqrt2),
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 0.0),
            1.0,
            vec(0.0, 0.0),
        },
    };

    for (auto const& [ln_start, ln_delta, sp_orig, sp_rad, expect] : test_cases)
    {
        auto const t = intersect_line_sphere(ln_start, ln_delta, sp_orig, sp_rad);

        if (t.has_value)
        {
            ASSERT_NEAR(expect.value[0], t.value[0], eps);
            ASSERT_NEAR(expect.value[1], t.value[1], eps);
        }
    }
}

UTEST(geometry, to_barycentric_tri3)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> point;
        Vec3<f64> tri_a;
        Vec3<f64> tri_b;
        Vec3<f64> tri_c;
        Vec3<f64> expect;
    };

    TestCase const test_cases[] = {
        {
            vec(0.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(1.0, 0.0, 0.0),
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 1.0, 0.0),
        },
        {
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
        },
        {
            vec(0.5, 0.5, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.5, 0.5),
        },
        {
            vec(1.0, 1.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(-1.0, 1.0, 1.0),
        },
    };

    for (auto const& [p, tri_a, tri_b, tri_c, expect] : test_cases)
    {
        auto const t = to_barycentric(p, tri_a, tri_b, tri_c);
        ASSERT_NEAR(expect[0], t[0], eps);
        ASSERT_NEAR(expect[1], t[1], eps);
        ASSERT_NEAR(expect[2], t[2], eps);
    }
}

UTEST(geometry, to_barycentric_tri2)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec2<f64> point;
        Vec2<f64> tri_a;
        Vec2<f64> tri_b;
        Vec2<f64> tri_c;
        Vec3<f64> expect;
    };

    TestCase const test_cases[] = {
        {
            vec(0.0, 0.0),
            vec(0.0, 0.0),
            vec(1.0, 0.0),
            vec(0.0, 1.0),
            vec(1.0, 0.0, 0.0),
        },
        {
            vec(1.0, 0.0),
            vec(0.0, 0.0),
            vec(1.0, 0.0),
            vec(0.0, 1.0),
            vec(0.0, 1.0, 0.0),
        },
        {
            vec(0.0, 1.0),
            vec(0.0, 0.0),
            vec(1.0, 0.0),
            vec(0.0, 1.0),
            vec(0.0, 0.0, 1.0),
        },
        {
            vec(0.5, 0.5),
            vec(0.0, 0.0),
            vec(1.0, 0.0),
            vec(0.0, 1.0),
            vec(0.0, 0.5, 0.5),
        },
        {
            vec(1.0, 1.0),
            vec(0.0, 0.0),
            vec(1.0, 0.0),
            vec(0.0, 1.0),
            vec(-1.0, 1.0, 1.0),
        },
    };

    for (auto const& [p, tri_a, tri_b, tri_c, expect] : test_cases)
    {
        auto const t = to_barycentric(p, tri_a, tri_b, tri_c);
        ASSERT_NEAR(expect[0], t[0], eps);
        ASSERT_NEAR(expect[1], t[1], eps);
        ASSERT_NEAR(expect[2], t[2], eps);
    }
}

UTEST(geometry, to_barycentric_tet)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> point;
        Vec3<f64> tet_a;
        Vec3<f64> tet_b;
        Vec3<f64> tet_c;
        Vec3<f64> tet_d;
        Vec4<f64> expect;
    };

    TestCase const test_cases[] = {
        {
            vec(0.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(1.0, 0.0, 0.0, 0.0),
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(0.0, 1.0, 0.0, 0.0),
        },
        {
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(0.0, 0.0, 1.0, 0.0),
        },
        {
            vec(0.0, 0.0, 1.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(0.0, 0.0, 0.0, 1.0),
        },
        {
            vec(0.5, 0.5, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(0.0, 0.5, 0.5, 0.0),
        },
        {
            vec(0.25, 0.25, 0.5),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(0.0, 0.25, 0.25, 0.5),
        },
        {
            vec(1.0, 1.0, 0.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(-1.0, 1.0, 1.0, 0.0),
        },
        {
            vec(1.0, 1.0, 1.0),
            vec(0.0, 0.0, 0.0),
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            vec(-2.0, 1.0, 1.0, 1.0),
        },
    };

    for (auto const& [p, tet_a, tet_b, tet_c, tet_d, expect] : test_cases)
    {
        auto const t = to_barycentric(p, tet_a, tet_b, tet_c, tet_d);
        ASSERT_NEAR(expect[0], t[0], eps);
        ASSERT_NEAR(expect[1], t[1], eps);
        ASSERT_NEAR(expect[2], t[2], eps);
        ASSERT_NEAR(expect[3], t[3], eps);
    }
}

UTEST(geometry, solid_angle)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;
    constexpr f64 sphere_area = 4.0 * pi<f64>;
    constexpr f64 inv_sqrt2 = 1.0 / sqrt2<f64>;

    struct TestCase
    {
        Vec3<f64> a;
        Vec3<f64> b;
        Vec3<f64> c;
        f64 expect;
    };

    TestCase const test_cases[] = {
        {
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            sphere_area / 8.0,
        },
        {
            vec(0.0, 0.0, 1.0),
            vec(0.0, 1.0, 0.0),
            vec(1.0, 0.0, 0.0),
            sphere_area / -8.0,
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(inv_sqrt2, inv_sqrt2, 0.0),
            vec(0.0, 0.0, 1.0),
            sphere_area / 16.0,
        },
        {
            vec(inv_sqrt2, inv_sqrt2, 0.0),
            vec(-inv_sqrt2, inv_sqrt2, 0.0),
            vec(0.0, 0.0, 1.0),
            sphere_area / 8.0,
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(-1.0, 0.0, 0.0),
            vec(0.0, 0.0, 1.0),
            0.0, // Degenerate case
        },
    };

    for (auto const& [a, b, c, expect] : test_cases)
    {
        auto const t = solid_angle(a, b, c, vec<3>(0.0));
        ASSERT_NEAR(expect, t, eps);
    }
}

UTEST(geometry, signed_area)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        DynamicArray<Vec2<f64>> polygon;
        f64 expect;
    };

    TestCase const test_cases[] = {
        {
            {
                {0.0, 0.0},
                {1.0, 0.0},
                {0.0, 1.0},
            },
            0.5,
        },
        {
            {
                {0.0, 0.0},
                {0.0, 1.0},
                {1.0, 0.0},
            },
            -0.5,
        },
        {
            {
                {0.0, 0.0},
                {1.0, 0.0},
                {1.0, 1.0},
                {0.0, 1.0},
            },
            1.0,
        },
        {
            {
                {0.0, 0.0},
                {0.0, 1.0},
                {1.0, 1.0},
                {1.0, 0.0},
            },
            -1.0,
        },
        {
            {
                {0.0, 0.0},
                {1.0, 0.0},
                {2.0, 0.0},
                {1.0, 1.0},
                {0.0, 1.0},
            },
            1.5,
        },
        {
            {
                {0.0, 0.0},
                {0.0, 1.0},
                {1.0, 1.0},
                {2.0, 0.0},
                {1.0, 0.0},
            },
            -1.5,
        },
    };

    for (auto const& [poly, expect] : test_cases)
    {
        auto const a = signed_area(as_span(poly));
        ASSERT_NEAR(expect, a, eps);
    }
}

UTEST(geometry, vector_area)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        DynamicArray<Vec3<f64>> polygon;
        Vec3<f64> expect;
    };

    TestCase const test_cases[] = {
        {
            {
                {0.0, 0.0, 0.0},
                {1.0, 0.0, 0.0},
                {0.0, 1.0, 0.0},
            },
            {0.0, 0.0, 0.5},
        },
        {
            {
                {0.0, 0.0, 0.0},
                {0.0, 1.0, 0.0},
                {1.0, 0.0, 0.0},
            },
            {0.0, 0.0, -0.5},
        },
        {
            {
                {0.0, 0.0, 0.0},
                {1.0, 0.0, 0.0},
                {1.0, 1.0, 0.0},
                {0.0, 1.0, 0.0},
            },
            {0.0, 0.0, 1.0},
        },
        {
            {
                {0.0, 0.0, 0.0},
                {0.0, 1.0, 0.0},
                {1.0, 1.0, 0.0},
                {1.0, 0.0, 0.0},
            },
            {0.0, 0.0, -1.0},
        },
        {
            {
                {0.0, 0.0, 0.0},
                {0.0, 0.0, 1.0},
                {1.0, 0.0, 1.0},
                {1.0, 0.0, 0.0},
            },
            {0.0, 1.0, 0.0},
        },
        {
            {
                {0.0, 0.0, 0.0},
                {1.0, 0.0, 0.0},
                {1.0, 0.0, 1.0},
                {0.0, 0.0, 1.0},
            },
            {0.0, -1.0, 0.0},
        },
        {
            {
                {0.0, 0.0, 0.0},
                {0.0, 0.0, 1.0},
                {1.0, 0.0, 1.0},
                {2.0, 0.0, 0.0},
                {1.0, 0.0, 0.0},
            },
            {0.0, 1.5, 0.0},
        },
        {
            {
                {0.0, 0.0, 0.0},
                {1.0, 0.0, 0.0},
                {2.0, 0.0, 0.0},
                {1.0, 0.0, 1.0},
                {0.0, 0.0, 1.0},
            },
            {0.0, -1.5, 0.0},
        },
    };

    for (auto const& [poly, expect] : test_cases)
    {
        auto const v = vector_area(as_span(poly));
        ASSERT_NEAR(expect[0], v[0], eps);
        ASSERT_NEAR(expect[1], v[1], eps);
        ASSERT_NEAR(expect[2], v[2], eps);
    }
}