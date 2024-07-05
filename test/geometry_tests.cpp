#include <utest.h>

#include <optional>

#include <dr/dynamic_array.hpp>
#include <dr/geometry.hpp>

UTEST(geometry, eval_gradient)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> p[3];
        f64 f[3];
        Covec3<f64> result;
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
    };

    for (auto const& [p, f, result] : test_cases)
    {
        auto const grad = eval_gradient(p[0], p[1], p[2], f[0], f[1], f[2]);
        ASSERT_NEAR(result[0], grad[0], eps);
        ASSERT_NEAR(result[1], grad[1], eps);
        ASSERT_NEAR(result[2], grad[2], eps);
    }
}

UTEST(geometry, eval_jacobian)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> p[3];
        Vec2<f64> f[3];
        Mat<f64, 2, 3> result;
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
    };

    for (auto const& [p, f, result] : test_cases)
    {
        auto const J = eval_jacobian(p[0], p[1], p[2], f[0], f[1], f[2]);
        ASSERT_NEAR(result(0, 0), J(0, 0), eps);
        ASSERT_NEAR(result(0, 1), J(0, 1), eps);
        ASSERT_NEAR(result(0, 2), J(0, 2), eps);
        ASSERT_NEAR(result(1, 0), J(1, 0), eps);
        ASSERT_NEAR(result(1, 1), J(1, 1), eps);
        ASSERT_NEAR(result(1, 2), J(1, 2), eps);
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
        f64 result[3];
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

    for (auto const& [p, f, result] : test_cases)
    {
        auto const d = eval_divergence(p[0], p[1], p[2], f);
        ASSERT_NEAR(result[0], d[0], eps);
        ASSERT_NEAR(result[1], d[1], eps);
        ASSERT_NEAR(result[2], d[2], eps);
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
        f64 result[3];
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

    for (auto const& [p, f, result] : test_cases)
    {
        auto const lap = eval_laplacian(p[0], p[1], p[2], f[0], f[1], f[2]);
        ASSERT_NEAR(result[0], lap[0], eps);
        ASSERT_NEAR(result[1], lap[1], eps);
        ASSERT_NEAR(result[2], lap[2], eps);
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
        f64 result;
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

    for (auto const& [p, ln_start, ln_delta, result] : test_cases)
    {
        auto const t = nearest_point_line(p, ln_start, ln_delta);
        ASSERT_NEAR(result, t, eps);
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
        Vec3<f64> result;
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

    for (auto const& [p, pl_orig, pl_norm, result] : test_cases)
    {
        auto const cp = nearest_point_plane(p, pl_orig, pl_norm);
        ASSERT_NEAR(result[0], cp[0], eps);
        ASSERT_NEAR(result[1], cp[1], eps);
        ASSERT_NEAR(result[2], cp[2], eps);
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
        Vec2<f64> result;
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

    for (auto const& [a_start, a_delta, b_start, b_delta, result] : test_cases)
    {
        auto const t = nearest_line_line(a_start, a_delta, b_start, b_delta);
        ASSERT_NEAR(result[0], t[0], eps);
        ASSERT_NEAR(result[1], t[1], eps);
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
        std::optional<Vec2<f64>> result;
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
            std::nullopt,
        },
    };

    for (auto const& [a_start, a_delta, b_start, b_delta, result] : test_cases)
    {
        auto const t = intersect_line_line(a_start, a_delta, b_start, b_delta);

        if (t.has_value())
        {
            ASSERT_TRUE(result.has_value());
            ASSERT_NEAR(result.value()[0], t.value()[0], eps);
            ASSERT_NEAR(result.value()[1], t.value()[1], eps);
        }
        else
        {
            ASSERT_FALSE(result.has_value());
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
        std::optional<f64> result;
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
            std::nullopt,
        },
    };

    for (auto const& [ln_start, ln_delta, pl_orig, pl_norm, result] : test_cases)
    {
        auto const t = intersect_line_plane(ln_start, ln_delta, pl_orig, pl_norm);

        if (t.has_value())
        {
            ASSERT_TRUE(result.has_value());
            ASSERT_NEAR(result.value(), t.value(), eps);
        }
        else
        {
            ASSERT_FALSE(result.has_value());
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
        std::optional<Vec2<f64>> result;
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

    for (auto const& [ln_start, ln_delta, sp_orig, sp_rad, result] : test_cases)
    {
        auto const t = intersect_line_sphere(ln_start, ln_delta, sp_orig, sp_rad);

        if (t.has_value())
        {
            ASSERT_TRUE(result.has_value());
            ASSERT_NEAR(result.value()[0], t.value()[0], eps);
            ASSERT_NEAR(result.value()[1], t.value()[1], eps);
        }
        else
        {
            ASSERT_FALSE(result.has_value());
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
        Vec3<f64> result;
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

    for (auto const& [p, tri_a, tri_b, tri_c, result] : test_cases)
    {
        auto const t = to_barycentric(p, tri_a, tri_b, tri_c);
        ASSERT_NEAR(result[0], t[0], eps);
        ASSERT_NEAR(result[1], t[1], eps);
        ASSERT_NEAR(result[2], t[2], eps);
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
        Vec3<f64> result;
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

    for (auto const& [p, tri_a, tri_b, tri_c, result] : test_cases)
    {
        auto const t = to_barycentric(p, tri_a, tri_b, tri_c);
        ASSERT_NEAR(result[0], t[0], eps);
        ASSERT_NEAR(result[1], t[1], eps);
        ASSERT_NEAR(result[2], t[2], eps);
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
        f64 result;
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

    for (auto const& [a, b, c, result] : test_cases)
    {
        auto const t = solid_angle(a, b, c, vec<3>(0.0));
        ASSERT_NEAR(result, t, eps);
    }
}

UTEST(geometry, signed_area)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        DynamicArray<Vec2<f64>> polygon;
        f64 result;
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

    for (auto const& [poly, result] : test_cases)
    {
        auto const a = signed_area(as_span(poly));
        ASSERT_NEAR(result, a, eps);
    }
}

UTEST(geometry, vector_area)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        DynamicArray<Vec3<f64>> polygon;
        Vec3<f64> result;
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

    for (auto const& [poly, result] : test_cases)
    {
        auto const v = vector_area(as_span(poly));
        ASSERT_NEAR(result[0], v[0], eps);
        ASSERT_NEAR(result[1], v[1], eps);
        ASSERT_NEAR(result[2], v[2], eps);
    }
}