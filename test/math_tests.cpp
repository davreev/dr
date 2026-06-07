#include <utest.h>

#include <dr/math.hpp>
#include <dr/math_traits.hpp>

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
            mat(vec(1.0, 0.0, 0.0, 0.0), vec(0.0, 1.0, 0.0, 0.0), vec(0.0, 0.0, 1.0, 0.0)),
            vec(0.0, 0.0, 0.0, 1.0),
            vec(0.0, 0.0, 0.0),
        },
        {
            mat(vec(1.0, 0.0, 0.0, 0.0), vec(0.0, 1.0, 0.0, 0.0), vec(0.0, 0.0, 1.0, 0.0)),
            vec(1.0, 1.0, 0.0, 1.0),
            vec(1.0, 1.0, 0.0),
        },
        {
            mat(vec(1.0, 1.0, 0.0, 0.0), vec(-1.0, 1.0, 0.0, 0.0), vec(0.0, 0.0, 1.0, 0.0)),
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
    ASSERT_NEAR(0.0, mod(2.0, 1.0), eps);
    ASSERT_NEAR(0.0, mod(-1.0, 1.0), eps);

    ASSERT_NEAR(0.0, mod(0.0, -1.0), eps);
    ASSERT_NEAR(0.0, mod(-1.0, -1.0), eps);
    ASSERT_NEAR(0.0, mod(-2.0, -1.0), eps);
    ASSERT_NEAR(0.0, mod(1.0, -1.0), eps);

    ASSERT_NEAR(0.1, mod(0.1, 1.0), eps);
    ASSERT_NEAR(0.1, mod(1.1, 1.0), eps);
    ASSERT_NEAR(0.1, mod(2.1, 1.0), eps);
    ASSERT_NEAR(0.1, mod(-0.9, 1.0), eps);

    ASSERT_NEAR(-0.9, mod(0.1, -1.0), eps);
    ASSERT_NEAR(-0.9, mod(-0.9, -1.0), eps);
    ASSERT_NEAR(-0.9, mod(-1.9, -1.0), eps);
    ASSERT_NEAR(-0.9, mod(1.1, -1.0), eps);

    ASSERT_EQ(0, mod(0, 5));
    ASSERT_EQ(0, mod(5, 5));
    ASSERT_EQ(0, mod(10, 5));
    ASSERT_EQ(0, mod(-5, 5));

    ASSERT_EQ(0, mod(0, -5));
    ASSERT_EQ(0, mod(-5, -5));
    ASSERT_EQ(0, mod(-10, -5));
    ASSERT_EQ(0, mod(5, -5));

    ASSERT_EQ(1, mod(1, 5));
    ASSERT_EQ(1, mod(6, 5));
    ASSERT_EQ(1, mod(11, 5));
    ASSERT_EQ(1, mod(-4, 5));

    ASSERT_EQ(-4, mod(1, -5));
    ASSERT_EQ(-4, mod(-4, -5));
    ASSERT_EQ(-4, mod(-9, -5));
    ASSERT_EQ(-4, mod(6, -5));

    ASSERT_EQ(0u, mod(0u, 5u));
    ASSERT_EQ(0u, mod(5u, 5u));
    ASSERT_EQ(0u, mod(10u, 5u));

    ASSERT_EQ(1u, mod(1u, 5u));
    ASSERT_EQ(1u, mod(6u, 5u));
    ASSERT_EQ(1u, mod(11u, 5u));
}

UTEST(math, wrap)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    ASSERT_NEAR(1.0, wrap(1.0, 1.0, 2.0), eps);
    ASSERT_NEAR(1.0, wrap(2.0, 1.0, 2.0), eps);
    ASSERT_NEAR(1.0, wrap(3.0, 1.0, 2.0), eps);
    ASSERT_NEAR(1.0, wrap(0.0, 1.0, 2.0), eps);

    ASSERT_NEAR(1.1, wrap(1.1, 1.0, 2.0), eps);
    ASSERT_NEAR(1.1, wrap(2.1, 1.0, 2.0), eps);
    ASSERT_NEAR(1.1, wrap(3.1, 1.0, 2.0), eps);
    ASSERT_NEAR(1.1, wrap(0.1, 1.0, 2.0), eps);

    ASSERT_NEAR(2.0, wrap(2.0, 2.0, 1.0), eps);
    ASSERT_NEAR(2.0, wrap(1.0, 2.0, 1.0), eps);
    ASSERT_NEAR(2.0, wrap(0.0, 2.0, 1.0), eps);
    ASSERT_NEAR(2.0, wrap(3.0, 2.0, 1.0), eps);

    ASSERT_NEAR(1.1, wrap(2.1, 2.0, 1.0), eps);
    ASSERT_NEAR(1.1, wrap(1.1, 2.0, 1.0), eps);
    ASSERT_NEAR(1.1, wrap(0.1, 2.0, 1.0), eps);
    ASSERT_NEAR(1.1, wrap(3.1, 2.0, 1.0), eps);

    ASSERT_EQ(5, wrap(5, 5, 10));
    ASSERT_EQ(5, wrap(10, 5, 10));
    ASSERT_EQ(5, wrap(15, 5, 10));
    ASSERT_EQ(5, wrap(0, 5, 10));

    ASSERT_EQ(6, wrap(6, 5, 10));
    ASSERT_EQ(6, wrap(11, 5, 10));
    ASSERT_EQ(6, wrap(16, 5, 10));
    ASSERT_EQ(6, wrap(1, 5, 10));

    ASSERT_EQ(10, wrap(10, 10, 5));
    ASSERT_EQ(10, wrap(5, 10, 5));
    ASSERT_EQ(10, wrap(0, 10, 5));
    ASSERT_EQ(10, wrap(15, 10, 5));

    ASSERT_EQ(6, wrap(11, 10, 5));
    ASSERT_EQ(6, wrap(6, 10, 5));
    ASSERT_EQ(6, wrap(1, 10, 5));
    ASSERT_EQ(6, wrap(16, 10, 5));
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

/*
    Compile-time checks
*/

namespace dr
{

/*
    Explicit instantiation of templates to catch compile errors
*/

template f64 deg_to_rad(f64);

template f64 rad_to_deg(f64);

template f64 sign(f64);

template f64 min(f64, f64);

template f64 max(f64, f64);

template f64 saturate(f64);

template f64 clamp(f64, f64, f64);

template f64 abs(f64);

template bool near_equal(f64, f64, f64);

template bool near_equal(f64, f64, f64, f64);

template f64 mod(f64, f64);

template f64 wrap(f64, f64, f64);

template f64 fract(f64);

template f64 fract(f64, f64&);

template f64 lerp(f64, f64, f64);

template f64 inv_lerp(f64, f64, f64);

template f64 remap(f64, f64, f64, f64, f64);

template f64 hermite_c1(f64);

template f64 smooth_step(f64);

template f64 smooth_step(f64, f64, f64);

template f64 smooth_pulse(f64, f64, f64);

template f64 ramp(f64, f64, f64);

template f64 cross(Vec2<f64> const&, Vec2<f64> const&);

template Vec3<f64> cross_x(Vec3<f64> const&);

template Vec3<f64> cross_y(Vec3<f64> const&);

template Vec3<f64> cross_z(Vec3<f64> const&);

template Vec2<f64> perp_ccw(Vec2<f64> const&);

template Vec2<f64> perp_cw(Vec2<f64> const&);

template Vec<f64, 3> project(Vec<f64, 3> const&, Vec<f64, 3> const&);

template Vec<f64, 3> reject(Vec<f64, 3> const&, Vec<f64, 3> const&);

template Vec<f64, 3> reflect(Vec<f64, 3> const&, Vec<f64, 3> const&);

template bool near_equal(Vec<f64, 3> const&, Vec<f64, 3> const&, f64);

template bool near_equal(Vec<f64, 3> const&, Vec<f64, 3> const&, f64, f64);

template bool near_parallel(Vec<f64, 3> const&, Vec<f64, 3> const&, f64);

template bool near_parallel(Vec<f64, 3> const&, Vec<f64, 3> const&, f64, f64);

template Quat<f64> nlerp(Quat<f64> const&, Quat<f64> const&, f64);

template f64 sqrt_safe(f64);

template f64 asin_safe(f64);

template f64 acos_safe(f64);

template f64 angle(Vec<f64, 3> const&, Vec<f64, 3> const&);

template f64 signed_angle(Vec2<f64> const&, Vec2<f64> const&);

template f64 signed_angle(Vec3<f64> const&, Vec3<f64> const&, Vec3<f64> const&);

template f64 angle_in_plane(Vec3<f64> const&, Vec3<f64> const&, Vec3<f64> const&);

template f64 sin_angle(Vec3<f64> const&, Vec3<f64> const&);

template f64 cos_angle(Vec3<f64> const&, Vec3<f64> const&);

template f64 tan_angle(Vec3<f64> const&, Vec3<f64> const&);

template f64 cot_angle(Vec3<f64> const&, Vec3<f64> const&);

template Vec<f64, 2> solve_least_squares(Mat<f64, 3, 2> const&, Vec<f64, 3> const&);

template Mat<f64, 3, 3> mat<3, f64>(f64);

template Mat<f64, 3, 3> mat<3, f64>(Vec<f64, 3> const&);

template Mat<f64, 3, 3> mat<3, f64>(Covec<f64, 3> const&);

template Mat<f64, 3, 2> mat(Vec<f64, 3> const&, Vec<f64, 3> const&);

template Mat<f64, 3, 3> mat(Vec<f64, 3> const&, Vec<f64, 3> const&, Vec<f64, 3> const&);

template Mat<f64, 3, 4> mat(
    Vec<f64, 3> const&,
    Vec<f64, 3> const&,
    Vec<f64, 3> const&,
    Vec<f64, 3> const&);

template Mat<f64, 2, 3> mat(Covec<f64, 3> const&, Covec<f64, 3> const&);

template Mat<f64, 3, 3> mat(Covec<f64, 3> const&, Covec<f64, 3> const&, Covec<f64, 3> const&);

template Mat<f64, 4, 3> mat(
    Covec<f64, 3> const&,
    Covec<f64, 3> const&,
    Covec<f64, 3> const&,
    Covec<f64, 3> const&);

template Vec<f64, 3> vec<3, f64>(f64);

template Vec<f64, 2> vec(f64, f64);

template Vec<f64, 3> vec(f64, f64, f64);

template Vec<f64, 4> vec(f64, f64, f64, f64);

template Vec<f64, 3> col<3, f64>(f64);

template Vec<f64, 2> col(f64, f64);

template Vec<f64, 3> col(f64, f64, f64);

template Vec<f64, 4> col(f64, f64, f64, f64);

template auto col(MatExpr<Vec3<f64>> const&);

template Covec<f64, 3> covec<3, f64>(f64);

template Covec<f64, 2> covec(f64, f64);

template Covec<f64, 3> covec(f64, f64, f64);

template Covec<f64, 4> covec(f64, f64, f64, f64);

template Covec<f64, 3> row<3, f64>(f64);

template Covec<f64, 2> row(f64, f64);

template Covec<f64, 3> row(f64, f64, f64);

template Covec<f64, 4> row(f64, f64, f64, f64);

template auto row(MatExpr<Covec<f64, 3>> const&);

template Complex<f64> complex(f64, f64);

template Quat<f64> quat(f64, f64, f64, f64);

template Quat<f64> quat(f64, Vec3<f64> const&);

} // namespace dr