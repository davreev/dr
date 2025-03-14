#include <utest.h>

#include <dr/spline.hpp>

UTEST(spline, linear)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        f64 vals[2];
        f64 t;
        struct
        {
            f64 val;
            f64 dt;
        } expect{};
    };

    TestCase const test_cases[] = {
        {
            {0.0, 1.0},
            0.5,
            {
                0.5,
                1.0,
            },
        },
        {
            {1.0, 1.0},
            0.5,
            {
                1.0,
                0.0,
            },
        },
        {

            {-1.0, 1.0},
            0.5,
            {
                0.0,
                2.0,
            },
        },
        {

            {-1.0, 1.0},
            1.5,
            {
                2.0,
                2.0,
            },
        },
        {

            {-1.0, 1.0},
            -0.5,
            {
                -2.0,
                2.0,
            },
        },
    };

    for (auto const& [vals, t, expect] : test_cases)
    {
        auto const val = eval_linear(vals, t);
        ASSERT_NEAR(expect.val, val, eps);

        auto const dt = eval_linear_dt(vals, t);
        ASSERT_NEAR(expect.dt, dt, eps);
    }
}

UTEST(spline, cubic)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        f64 vals[4];
        f64 t;
        struct
        {
            f64 val;
            f64 dt;
        } expect{};
    };

    TestCase const test_cases[] = {
        {
            {0.0, 1.0, 2.0, 3.0},
            0.5,
            {
                1.5,
                1.0,
            },
        },
        {
            {-1.0, 0.0, 1.0, 2.0},
            0.5,
            {
                0.5,
                1.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0},
            -0.5,
            {
                0.5,
                1.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0},
            1.5,
            {
                2.5,
                1.0,
            },
        },
        {
            {0.0, 0.0, 1.0, 1.0},
            0.0,
            {
                0.0,
                0.5,
            },
        },
        {
            {0.0, 0.0, 1.0, 1.0},
            1.0,
            {
                1.0,
                0.5,
            },
        },
        {
            {0.0, 0.0, 1.0, 1.0},
            0.5,
            {
                0.5,
                1.25,
            },
        },
    };

    for (auto const& [vals, t, expect] : test_cases)
    {
        auto const val = eval_cubic(vals, t);
        ASSERT_NEAR(expect.val, val, eps);

        auto const dt = eval_cubic_dt(vals, t);
        ASSERT_NEAR(expect.dt, dt, eps);
    }
}

UTEST(spline, bezier_quadratic)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        f64 vals[3];
        f64 t;
        struct
        {
            f64 val;
            f64 dt;
        } expect{};
    };

    TestCase const test_cases[] = {
        {
            {0.0, 1.0, 2.0},
            0.5,
            {
                1.0,
                2.0,
            },
        },
        {
            {-1.0, 0.0, 1.0},
            0.5,
            {
                0.0,
                2.0,
            },
        },
        {
            {0.0, 1.0, 2.0},
            -0.5,
            {
                -1.0,
                2.0,
            },
        },
        {
            {0.0, 1.0, 2.0},
            1.5,
            {
                3.0,
                2.0,
            },
        },
        {
            {0.0, 0.0, 2.0},
            0.0,
            {
                0.0,
                0.0,
            },
        },
        {
            {0.0, 0.0, 2.0},
            1.0,
            {
                2.0,
                4.0,
            },
        },
        {
            {0.0, 0.0, 2.0},
            0.5,
            {
                0.5,
                2.0,
            },
        },
    };

    for (auto const& [vals, t, expect] : test_cases)
    {
        auto const val = eval_bezier_quadratic(vals, t);
        ASSERT_NEAR(expect.val, val, eps);

        auto const dt = eval_bezier_quadratic_dt(vals, t);
        ASSERT_NEAR(expect.dt, dt, eps);
    }
}

UTEST(spline, bezier_cubic)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        f64 vals[4];
        f64 t;
        struct
        {
            f64 val;
            f64 dt;
        } expect{};
    };

    TestCase const test_cases[] = {
        {
            {0.0, 1.0, 2.0, 3.0},
            0.5,
            {
                1.5,
                3.0,
            },
        },
        {
            {-1.0, 0.0, 1.0, 2.0},
            0.5,
            {
                0.5,
                3.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0},
            -0.5,
            {
                -1.5,
                3.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0},
            1.5,
            {
                4.5,
                3.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0},
            0.0,
            {
                0.0,
                0.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0},
            1.0,
            {
                2.0,
                0.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0},
            0.25,
            {
                0.3125,
                2.25,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0},
            0.5,
            {
                1.0,
                3.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0},
            0.75,
            {
                1.6875,
                2.25,
            },
        },
    };

    for (auto const& [vals, t, expect] : test_cases)
    {
        auto const val = eval_bezier_cubic(vals, t);
        ASSERT_NEAR(expect.val, val, eps);

        auto const dt = eval_bezier_cubic_dt(vals, t);
        ASSERT_NEAR(expect.dt, dt, eps);
    }
}

UTEST(spline, bilinear)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        f64 vals[4];
        f64 u;
        f64 v;
        struct
        {
            f64 val;
            f64 du;
            f64 dv;
        } expect{};
    };

    TestCase const test_cases[] = {
        {
            {0.0, 0.0, 2.0, 2.0},
            0.0,
            0.0,
            {
                0.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0},
            0.5,
            0.5,
            {
                1.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0},
            1.0,
            1.0,
            {
                2.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0},
            -0.5,
            -0.5,
            {
                -1.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0},
            1.5,
            1.5,
            {
                3.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 2.0, 0.0, 2.0},
            0.0,
            0.0,
            {
                0.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 2.0, 0.0, 2.0},
            0.5,
            0.5,
            {
                1.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 2.0, 0.0, 2.0},
            1.0,
            1.0,
            {
                2.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 2.0, 0.0, 2.0},
            -0.5,
            -0.5,
            {
                -1.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 2.0, 0.0, 2.0},
            1.5,
            1.5,
            {
                3.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0},
            0.0,
            0.0,
            {
                0.0,
                1.0,
                2.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0},
            0.5,
            0.5,
            {
                1.5,
                1.0,
                2.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0},
            1.0,
            1.0,
            {
                3.0,
                1.0,
                2.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0},
            -0.5,
            -0.5,
            {
                -1.5,
                1.0,
                2.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0},
            1.5,
            1.5,
            {
                4.5,
                1.0,
                2.0,
            },
        },
    };

    for (auto const& [vals, u, v, expect] : test_cases)
    {
        auto const val = eval_bilinear(vals, u, v);
        ASSERT_NEAR(expect.val, val, eps);

        auto const du = eval_bilinear_du(vals, u, v);
        ASSERT_NEAR(expect.du, du, eps);

        auto const dv = eval_bilinear_dv(vals, u, v);
        ASSERT_NEAR(expect.dv, dv, eps);
    }
}

UTEST(spline, trilinear)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        f64 vals[8];
        f64 u;
        f64 v;
        f64 w;
        struct
        {
            f64 val;
            f64 du;
            f64 dv;
            f64 dw;
        } expect{};
    };

    TestCase const test_cases[] = {
        {
            {0.0, 2.0, 0.0, 2.0, 0.0, 2.0, 0.0, 2.0},
            0.0,
            0.0,
            0.0,
            {
                0.0,
                2.0,
                0.0,
                0.0,
            },
        },
        {
            {0.0, 2.0, 0.0, 2.0, 0.0, 2.0, 0.0, 2.0},
            0.5,
            0.5,
            0.5,
            {
                1.0,
                2.0,
                0.0,
                0.0,
            },
        },
        {
            {0.0, 2.0, 0.0, 2.0, 0.0, 2.0, 0.0, 2.0},
            1.0,
            1.0,
            1.0,
            {
                2.0,
                2.0,
                0.0,
                0.0,
            },
        },
        {
            {0.0, 2.0, 0.0, 2.0, 0.0, 2.0, 0.0, 2.0},
            -0.5,
            -0.5,
            -0.5,
            {
                -1.0,
                2.0,
                0.0,
                0.0,
            },
        },
        {
            {0.0, 2.0, 0.0, 2.0, 0.0, 2.0, 0.0, 2.0},
            1.5,
            1.5,
            1.5,
            {
                3.0,
                2.0,
                0.0,
                0.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 2.0, 2.0},
            0.0,
            0.0,
            0.0,
            {
                0.0,
                0.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 2.0, 2.0},
            0.5,
            0.5,
            0.5,
            {
                1.0,
                0.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 2.0, 2.0},
            1.0,
            1.0,
            1.0,
            {
                2.0,
                0.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 2.0, 2.0},
            -0.5,
            -0.5,
            -0.5,
            {
                -1.0,
                0.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 2.0, 2.0},
            1.5,
            1.5,
            1.5,
            {
                3.0,
                0.0,
                2.0,
                0.0,
            },
        },
        {
            {0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 2.0},
            0.0,
            0.0,
            0.0,
            {
                0.0,
                0.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 2.0},
            0.5,
            0.5,
            0.5,
            {
                1.0,
                0.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 2.0},
            1.0,
            1.0,
            1.0,
            {
                2.0,
                0.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 2.0},
            -0.5,
            -0.5,
            -0.5,
            {
                -1.0,
                0.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 2.0},
            1.5,
            1.5,
            1.5,
            {
                3.0,
                0.0,
                0.0,
                2.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
            0.0,
            0.0,
            0.0,
            {
                0.0,
                1.0,
                2.0,
                4.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
            0.5,
            0.5,
            0.5,
            {
                3.5,
                1.0,
                2.0,
                4.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
            1.0,
            1.0,
            1.0,
            {
                7.0,
                1.0,
                2.0,
                4.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
            -0.5,
            -0.5,
            -0.5,
            {
                -3.5,
                1.0,
                2.0,
                4.0,
            },
        },
        {
            {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
            1.5,
            1.5,
            1.5,
            {
                10.5,
                1.0,
                2.0,
                4.0,
            },
        },
    };

    for (auto const& [vals, u, v, w, expect] : test_cases)
    {
        auto const val = eval_trilinear(vals, u, v, w);
        ASSERT_NEAR(expect.val, val, eps);

        auto const du = eval_trilinear_du(vals, u, v, w);
        ASSERT_NEAR(expect.du, du, eps);

        auto const dv = eval_trilinear_dv(vals, u, v, w);
        ASSERT_NEAR(expect.dv, dv, eps);

        auto const dw = eval_trilinear_dw(vals, u, v, w);
        ASSERT_NEAR(expect.dw, dw, eps);
    }
}