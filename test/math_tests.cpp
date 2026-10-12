#include <utest.h>

#include <dr/math.hpp>
#include <dr/meta.hpp>

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

UTEST(math, solve_least_squares_matrix_rhs)
{
    using namespace dr;

    Mat<f64, 4, 3> const A = mat(
        vec(1.0, 0.0, 0.0, 0.0), //
        vec(0.0, 1.0, 0.0, 0.0),
        vec(0.0, 0.0, 1.0, 0.0));

    Mat<f64, 4, 2> const b = mat(
        vec(1.0, 3.0, 5.0, 7.0), //
        vec(2.0, 4.0, 6.0, 8.0));

    // Both inputs are expressions; the result must own its coefficients and have
    // one row per unknown, rather than one row per equation.
    auto const x = solve_least_squares(A * Mat3<f64>::Identity(), b + b);
    static_assert(std::is_same_v<DropCvRef<decltype(x)>, Mat<f64, 3, 2>>);

    for (int row = 0; row < x.rows(); ++row)
    {
        for (int col = 0; col < x.cols(); ++col)
            ASSERT_NEAR(2.0 * b(row, col), x(row, col), 1.0e-12);
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

namespace dr
{
namespace test
{

template <typename T>
struct Number
{
    T value{};

    constexpr Number() = default;
    explicit constexpr Number(T const x) : value{x} {}

    friend constexpr Number operator+(Number a, Number b) { return Number{a.value + b.value}; }
    friend constexpr Number operator-(Number a, Number b) { return Number{a.value - b.value}; }
    friend constexpr Number operator*(Number a, Number b) { return Number{a.value * b.value}; }
    friend constexpr Number operator/(Number a, Number b) { return Number{a.value / b.value}; }
    friend constexpr Number operator%(Number a, Number b) { return Number{a.value % b.value}; }
    friend constexpr Number operator-(Number a) { return Number{-a.value}; }
    friend constexpr bool operator<(Number a, Number b) { return a.value < b.value; }
    friend constexpr bool operator>(Number a, Number b) { return a.value > b.value; }
    friend constexpr bool operator!=(Number a, Number b) { return a.value != b.value; }
};

using Real = Number<double>;

struct
{
    int floor{};
    int sqrt{};
    int asin{};
    int acos{};
} op_count;

Real floor(Real x)
{
    ++op_count.floor;
    return Real{std::floor(x.value)};
}

Real sqrt(Real x)
{
    ++op_count.sqrt;
    return Real{std::sqrt(x.value)};
}

Real asin(Real x)
{
    ++op_count.asin;
    return Real{std::asin(x.value)};
}

Real acos(Real x)
{
    ++op_count.acos;
    return Real{std::acos(x.value)};
}

} // namespace test

template <>
struct CustomNumTraits<test::Real> : DefaultNumTraits<test::Real>
{
    static constexpr bool is_real{true};
};

} // namespace dr

UTEST(math, custom_real_adl)
{
    using namespace dr;
    using test::Real;

    constexpr f64 eps = 1e-12;

    ASSERT_NEAR(dr::mod(Real{-1.25}, Real{1.0}).value, 0.75, eps);
    ASSERT_NEAR(dr::fract(Real{-1.25}).value, 0.75, eps);
    Real whole;
    ASSERT_NEAR(dr::fract(Real{-1.25}, whole).value, 0.75, eps);
    ASSERT_EQ(whole.value, -2.0);
    ASSERT_EQ(dr::sqrt_safe(Real{-4.0}).value, 0.0);
    ASSERT_NEAR(dr::asin_safe(Real{2.0}).value, std::asin(1.0), eps);
    ASSERT_EQ(dr::acos_safe(Real{2.0}).value, 0.0);

    auto const& op_count = dr::test::op_count;
    ASSERT_EQ(op_count.floor, 3);
    ASSERT_EQ(op_count.sqrt, 1);
    ASSERT_EQ(op_count.asin, 1);
    ASSERT_EQ(op_count.acos, 1);
}

/*
    Compile-time checks
*/

namespace dr
{

namespace
{

template <typename... Natural, typename... Integer, typename... Real, typename... Complex>
void check_num_traits(
    TypePack<Natural...>,
    TypePack<Integer...>,
    TypePack<Real...>,
    TypePack<Complex...>)
{
    static_assert((NumTraits<Natural>::is_natural && ...));
    static_assert((NumTraits<Integer>::is_integer && ...));
    static_assert((NumTraits<Real>::is_real && ...));
    static_assert((NumTraits<Complex>::is_complex && ...));

    static_assert((!NumTraits<Natural>::is_signed && ...));
    static_assert((NumTraits<Integer>::is_signed && ...));
    static_assert((NumTraits<Real>::is_signed && ...));
    static_assert((!NumTraits<Complex>::is_ordered && ...));

    static_assert((NumTraits<Natural>::is_index && ...));
    static_assert((NumTraits<Integer>::is_index && ...));
    static_assert((!NumTraits<Real>::is_index && ...));
    static_assert((!NumTraits<Complex>::is_index && ...));

    static_assert(
        (NumTraits<Natural>::is_number && ...) && (NumTraits<Integer>::is_number && ...)
        && (NumTraits<Real>::is_number && ...) && (NumTraits<Complex>::is_number && ...));
}

[[maybe_unused]]
void check_num_traits()
{
    check_num_traits(
        TypePack<u8, u16, u32, u64, usize, unsigned int, std::size_t>{},
        TypePack<i8, i16, i32, i64, isize, int, std::ptrdiff_t>{},
        TypePack<f32, f64, float, double>{},
        TypePack<Complex<float>, Complex<double>>{});

    static_assert(
        !NumTraits<void>::is_number && !NumTraits<bool>::is_number && !NumTraits<i32*>::is_number
        && !NumTraits<i32[3]>::is_number);

    // Trait queries normalize cv/ref qualifiers.
    static_assert(NumTraits<u32 const&>::is_natural);
    static_assert(NumTraits<i32 volatile&>::is_integer);
    static_assert(NumTraits<f64 const&&>::is_real);
    static_assert(NumTraits<i32 const&>::is_number && NumTraits<i32 const&>::is_signed);
    static_assert(NumTraits<u32 volatile&>::is_index && !NumTraits<u32 volatile&>::is_signed);
}

[[maybe_unused]]
void check_custom_num_traits()
{
    using Traits = NumTraits<test::Real>;
    static_assert(Traits::is_number && Traits::is_real && Traits::is_signed && Traits::is_ordered);
    static_assert(
        !Traits::is_builtin && !Traits::is_index && !Traits::is_natural && !Traits::is_integer
        && !Traits::is_rational && !Traits::is_complex);
}

[[maybe_unused]]
void check_point_wise_result_types()
{
    Vec3<f64> const v = Vec3<f64>::Ones();
    Covec3<f64> const c = Covec3<f64>::Ones();
    Mat3<f64> const m = Mat3<f64>::Identity();
    f64 buf[3]{};
    MatView<Vec3<f64>> const view{buf};

    // Each overload returns the plain matrix type, never an expression
    static_assert(std::is_same_v<decltype(abs(v)), Vec3<f64>>);
    static_assert(std::is_same_v<decltype(abs(c)), Covec3<f64>>);
    static_assert(std::is_same_v<decltype(abs(m)), Mat3<f64>>);

    // A block, a view and an expression all bind to the first parameter
    static_assert(std::is_same_v<decltype(saturate(m.col(0))), Vec3<f64>>);
    static_assert(std::is_same_v<decltype(saturate(m.row(0))), Covec3<f64>>);
    static_assert(std::is_same_v<decltype(saturate(view)), Vec3<f64>>);
    static_assert(std::is_same_v<decltype(saturate(v - v)), Vec3<f64>>);

    // An expression also binds to the other parameters. Eigen converts it to the plain type.
    static_assert(std::is_same_v<decltype(clamp(m, m - m, m + m)), Mat3<f64>>);
    static_assert(std::is_same_v<decltype(lerp(v, v * 2.0, v * 0.5)), Vec3<f64>>);

    // A scalar argument still selects the scalar overload
    static_assert(std::is_same_v<decltype(saturate(1.0)), f64>);
    static_assert(std::is_same_v<decltype(min(1.0f, 2.0f)), f32>);
}

} // namespace

/*
    Explicit instantiation of templates to catch compile errors
*/

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

/*
    Point-wise matrix overloads
*/

template Vec3<f64> sign(MatExpr<Vec3<f64>> const&);

template Vec3<f64> min(MatExpr<Vec3<f64>> const&, Vec3<f64> const&);

template Vec3<f64> min(MatExpr<Vec3<f64>> const&, f64);

template Vec3<f64> max(MatExpr<Vec3<f64>> const&, Vec3<f64> const&);

template Vec3<f64> max(MatExpr<Vec3<f64>> const&, f64);

template Vec3<f64> clamp(MatExpr<Vec3<f64>> const&, Vec3<f64> const&, Vec3<f64> const&);

template Vec3<f64> clamp(MatExpr<Vec3<f64>> const&, f64, f64);

template Vec3<f64> saturate(MatExpr<Vec3<f64>> const&);

template Vec3<f64> abs(MatExpr<Vec3<f64>> const&);

template Vec3<f64> mod(MatExpr<Vec3<f64>> const&, Vec3<f64> const&);

template Vec3<f64> mod(MatExpr<Vec3<f64>> const&, f64);

template Vec3<i32> mod(MatExpr<Vec3<i32>> const&, Vec3<i32> const&);

template Vec3<u32> mod(MatExpr<Vec3<u32>> const&, Vec3<u32> const&);

template Vec3<f64> wrap(MatExpr<Vec3<f64>> const&, Vec3<f64> const&, Vec3<f64> const&);

template Vec3<f64> wrap(MatExpr<Vec3<f64>> const&, f64, f64);

template Vec3<f64> fract(MatExpr<Vec3<f64>> const&);

template Vec3<f64> fract(MatExpr<Vec3<f64>> const&, Vec3<f64>&);

template Vec3<f64> lerp(MatExpr<Vec3<f64>> const&, Vec3<f64> const&, Vec3<f64> const&);

template Vec3<f64> lerp(MatExpr<Vec3<f64>> const&, Vec3<f64> const&, f64);

template Vec3<f64> inv_lerp(MatExpr<Vec3<f64>> const&, Vec3<f64> const&, Vec3<f64> const&);

template Vec3<f64> inv_lerp(f64, f64, MatExpr<Vec3<f64>> const&);

template Vec3<f64> remap(
    MatExpr<Vec3<f64>> const&,
    Vec3<f64> const&,
    Vec3<f64> const&,
    Vec3<f64> const&,
    Vec3<f64> const&);

template Vec3<f64> remap(MatExpr<Vec3<f64>> const&, f64, f64, f64, f64);

template Vec3<f64> hermite_c1(MatExpr<Vec3<f64>> const&);

template Vec3<f64> smooth_step(MatExpr<Vec3<f64>> const&);

template Vec3<f64> smooth_step(MatExpr<Vec3<f64>> const&, Vec3<f64> const&, Vec3<f64> const&);

template Vec3<f64> smooth_step(f64, f64, MatExpr<Vec3<f64>> const&);

template Vec3<f64> ramp(MatExpr<Vec3<f64>> const&, Vec3<f64> const&, Vec3<f64> const&);

template Vec3<f64> ramp(f64, f64, MatExpr<Vec3<f64>> const&);

template Vec3<f64> sqrt_safe(MatExpr<Vec3<f64>> const&);

template Vec3<f64> asin_safe(MatExpr<Vec3<f64>> const&);

template Vec3<f64> acos_safe(MatExpr<Vec3<f64>> const&);

template f64 angle(Vec<f64, 3> const&, Vec<f64, 3> const&);

template f64 signed_angle(Vec2<f64> const&, Vec2<f64> const&);

template f64 signed_angle(Vec3<f64> const&, Vec3<f64> const&, Vec3<f64> const&);

template f64 angle_in_plane(Vec3<f64> const&, Vec3<f64> const&, Vec3<f64> const&);

template f64 sin_angle(Vec3<f64> const&, Vec3<f64> const&);

template f64 cos_angle(Vec3<f64> const&, Vec3<f64> const&);

template f64 tan_angle(Vec3<f64> const&, Vec3<f64> const&);

template f64 cot_angle(Vec3<f64> const&, Vec3<f64> const&);

template auto solve_least_squares(MatExpr<Mat<f64, 3, 2>> const&, MatExpr<Vec<f64, 3>> const&);

/*
    Math constructors
*/

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

template auto vec(MatExpr<Vec<f64, 4>> const&);

template auto vec(MatExpr<Covec<f64, 4>> const&);

template Vec<f64, 3> col<3, f64>(f64);

template Vec<f64, 2> col(f64, f64);

template Vec<f64, 3> col(f64, f64, f64);

template Vec<f64, 4> col(f64, f64, f64, f64);

template auto col(MatExpr<Vec<f64, 4>> const&);

template auto col(MatExpr<Covec<f64, 4>> const&);

template Covec<f64, 3> covec<3, f64>(f64);

template Covec<f64, 2> covec(f64, f64);

template Covec<f64, 3> covec(f64, f64, f64);

template Covec<f64, 4> covec(f64, f64, f64, f64);

template auto covec(MatExpr<Vec<f64, 4>> const&);

template auto covec(MatExpr<Covec<f64, 4>> const&);

template Covec<f64, 3> row<3, f64>(f64);

template Covec<f64, 2> row(f64, f64);

template Covec<f64, 3> row(f64, f64, f64);

template Covec<f64, 4> row(f64, f64, f64, f64);

template auto row(MatExpr<Covec<f64, 4>> const&);

template auto row(MatExpr<Vec<f64, 4>> const&);

template Complex<f64> complex(f64, f64);

template auto quat(MatExpr<Vec<f64, 4>> const&);

template auto quat(MatExpr<Covec<f64, 4>> const&);

template auto quat(MatExpr<Mat3<f64>> const&);

template Quat<f64> quat(f64, f64, f64, f64);

template Quat<f64> quat(f64, Vec3<f64> const&);

template Quat<f64> quat(AngleAxis<f64> const&);

template AngleAxis<f64> angle_axis(f64, Vec3<f64> const&);

template AngleAxis<f64> axis_angle(Vec3<f64> const&, f64);

} // namespace dr
