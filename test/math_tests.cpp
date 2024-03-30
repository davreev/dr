#include <utest/utest.h>

#include <dr/math.hpp>

UTEST(math, solve_least_squares)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Mat<f64, 4, 3> A;
        Vec<f64, 4> b;
        Vec<f64, 3> result;
    };

    TestCase const test_cases[] = {
        {
            mat(
                vec(1.0, 0.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0, 0.0),
                vec(0.0, 0.0, 1.0, 0.0)),
            vec(0.0, 0.0, 0.0, 1.0),
            vec(0.0, 0.0, 0.0),
        },
        {
            mat(
                vec(1.0, 0.0, 0.0, 0.0),
                vec(0.0, 1.0, 0.0, 0.0),
                vec(0.0, 0.0, 1.0, 0.0)),
            vec(1.0, 1.0, 0.0, 1.0),
            vec(1.0, 1.0, 0.0),
        },
        {
            mat(
                vec(1.0, 1.0, 0.0, 0.0),
                vec(-1.0, 1.0, 0.0, 0.0),
                vec(0.0, 0.0, 1.0, 0.0)),
            vec(0.0, 2.0, 1.0, 1.0),
            vec(1.0, 1.0, 1.0),
        },
    };

    for (auto const& [A, b, result] : test_cases)
    {
        auto const x = solve_least_squares(A, b);
        ASSERT_NEAR(result[0], x[0], eps);
        ASSERT_NEAR(result[1], x[1], eps);
        ASSERT_NEAR(result[2], x[2], eps);
    }
}

UTEST(math, signed_angle)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        Vec3<f64> a;
        Vec3<f64> b;
        Vec3<f64> up;
        f64 result;
    };

    TestCase const test_cases[] = {
        {
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            pi<f64> * 0.5,
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0),
            vec(0.0, 0.0, -1.0),
            -pi<f64> * 0.5,
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(1.0, 1.0, 0.0),
            vec(0.0, 0.0, 1.0),
            pi<f64> * 0.25,
        },
        {
            vec(1.0, 0.0, 0.0),
            vec(1.0, 1.0, 0.0),
            vec(0.0, 0.0, -1.0),
            -pi<f64> * 0.25,
        },
    };

    for (auto const& [a, b, up, result] : test_cases)
    {
        auto const t = signed_angle(a, b, up);
        ASSERT_NEAR(result, t, eps);
    }
}

UTEST(math, lerp)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        f64 x0;
        f64 x1;
        f64 t;
        f64 result;
    };

    TestCase const test_cases[] = {
        {0.0, 1.0, 0.25, 0.25},
        {1.0, 0.0, 0.25, 0.75},
        {-1.0, 1.0, 0.25, -0.5},
        {1.0, -1.0, 0.25, 0.5},
        {1.0, 1.0, 0.0, 1.0},
        {1.0, 1.0, 1.0, 1.0},
    };

    for (auto const& [x0, x1, t, result] : test_cases)
    {
        auto const x = lerp(x0, x1, t);
        ASSERT_NEAR(result, x, eps);
    }
}

UTEST(math, inv_lerp)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        f64 x0;
        f64 x1;
        f64 x;
        f64 result;
    };

    TestCase const test_cases[] = {
        {0.0, 4.0, 1.0, 0.25},
        {4.0, 0.0, 1.0, 0.75},
        {2.0, -2.0, 1.0, 0.25},
        {-2.0, 2.0, 1.0, 0.75},
        {0.0, 1.0, 0.25, 0.25},
        {1.0, 0.0, 0.25, 0.75},
    };

    for (auto const& [x0, x1, x, result] : test_cases)
    {
        auto const t = inv_lerp(x0, x1, x);
        ASSERT_NEAR(result, t, eps);
    }
}

UTEST(math, mod)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    ASSERT_NEAR(0.0, mod(0.0, 1.0), eps);
    ASSERT_NEAR(0.0, mod(1.0, 1.0), eps);
    ASSERT_NEAR(0.1, mod(1.1, 1.0), eps);
    ASSERT_NEAR(0.9, mod(-0.1, 1.0), eps);
    ASSERT_NEAR(-0.9, mod(0.1, -1.0), eps);
    ASSERT_NEAR(-0.1, mod(-0.1, -1.0), eps);

    ASSERT_NEAR(0, mod(0, 5), eps);
    ASSERT_NEAR(0, mod(5, 5), eps);
    ASSERT_NEAR(1, mod(6, 5), eps);
    ASSERT_NEAR(4, mod(-1, 5), eps);
    ASSERT_NEAR(-4, mod(1, -5), eps);
    ASSERT_NEAR(-1, mod(-1, -5), eps);

    ASSERT_NEAR(0u, mod(0u, 5u), eps);
    ASSERT_NEAR(0u, mod(5u, 5u), eps);
    ASSERT_NEAR(1u, mod(6u, 5u), eps);
    ASSERT_NEAR(1u, mod(11u, 5u), eps);
}

UTEST(math, near_equal_scalar)
{
    using namespace dr;

    struct TestCase
    {
        f64 a;
        f64 b;
        f64 tol;
        bool result;
    };

    TestCase const test_cases[] = {
        {1.0, 1.001, 0.01, true},
        {1.0, 1.001, 0.001, true},
        {1.0, 1.01, 0.001, false},
        {10.0, 10.01, 0.001, true},
        {10.0, 10.01, 0.0001, false},
        {100.0, 100.01, 0.0001, true},
        {-100.0, -100.01, 0.0001, true},
    };

    for (auto const& [a, b, tol, result] : test_cases)
    {
        ASSERT_EQ(near_equal(a, b, tol, tol), result);
    }
}

UTEST(math_traits, is_natural)
{
    using namespace dr;
    
    ASSERT_TRUE(is_natural<unsigned int>);
    ASSERT_TRUE(is_natural<std::size_t>);

    ASSERT_FALSE(is_natural<float>);
    ASSERT_FALSE(is_natural<double>);

    ASSERT_FALSE(is_natural<int>);
    ASSERT_FALSE(is_natural<std::ptrdiff_t>);

    ASSERT_FALSE(is_natural<std::complex<float>>);
    ASSERT_FALSE(is_natural<std::complex<double>>);
}

UTEST(math_traits, is_integer)
{
    using namespace dr;
    
    ASSERT_TRUE(is_integer<int>);
    ASSERT_TRUE(is_integer<std::ptrdiff_t>);

    ASSERT_FALSE(is_integer<float>);
    ASSERT_FALSE(is_integer<double>);

    ASSERT_FALSE(is_integer<unsigned int>);
    ASSERT_FALSE(is_integer<std::size_t>);

    ASSERT_FALSE(is_integer<std::complex<float>>);
    ASSERT_FALSE(is_integer<std::complex<double>>);
}

UTEST(math_traits, is_real)
{
    using namespace dr;
    
    ASSERT_TRUE(is_real<float>);
    ASSERT_TRUE(is_real<double>);

    ASSERT_FALSE(is_real<int>);
    ASSERT_FALSE(is_real<std::ptrdiff_t>);

    ASSERT_FALSE(is_real<unsigned int>);
    ASSERT_FALSE(is_real<std::size_t>);

    ASSERT_FALSE(is_real<std::complex<float>>);
    ASSERT_FALSE(is_real<std::complex<double>>);
}

UTEST(math_traits, is_complex)
{
    using namespace dr;
    
    ASSERT_TRUE(is_complex<std::complex<float>>);
    ASSERT_TRUE(is_complex<std::complex<double>>);

    ASSERT_FALSE(is_complex<float>);
    ASSERT_FALSE(is_complex<double>);

    ASSERT_FALSE(is_complex<int>);
    ASSERT_FALSE(is_complex<std::ptrdiff_t>);

    ASSERT_FALSE(is_complex<unsigned int>);
    ASSERT_FALSE(is_complex<std::size_t>);
}