#pragma once

/*
    Assorted helper functions and for fixed-size math types
*/

#include <cassert>
#include <cmath>
#include <type_traits>

#include <dr/linalg_traits.hpp>
#include <dr/math_constants.hpp>
#include <dr/math_ctors.hpp>
#include <dr/math_types.hpp>
#include <dr/num_traits.hpp>

namespace dr
{

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num sign(Num const x)
{
    static_assert(is_signed<Num>);
    return (x > Num{0}) ? Num{1} : ((x < Num{0}) ? Num{-1} : Num{0});
}

template <typename T>
MatValue<T> sign(MatExpr<T> const& x)
{
    static_assert(MatShape<T>::is_static);
    return x.cwiseSign();
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num min(Num const a, Num const b)
{
    return (b < a) ? b : a;
}

template <typename T>
MatValue<T> min(MatExpr<T> const& a, MatValue<T> const& b)
{
    static_assert(MatShape<T>::is_static);
    return a.cwiseMin(b);
}

template <typename T>
MatValue<T> min(MatExpr<T> const& a, MatScalar<T> const b)
{
    static_assert(MatShape<T>::is_static);
    return a.cwiseMin(b);
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num max(Num const a, Num const b)
{
    return (b > a) ? b : a;
}

template <typename T>
MatValue<T> max(MatExpr<T> const& a, MatValue<T> const& b)
{
    static_assert(MatShape<T>::is_static);
    return a.cwiseMax(b);
}

template <typename T>
MatValue<T> max(MatExpr<T> const& a, MatScalar<T> const b)
{
    static_assert(MatShape<T>::is_static);
    return a.cwiseMax(b);
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num clamp(Num const x, Num const min, Num const max)
{
    return (x < min) ? min : ((x > max) ? max : x);
}

template <typename T>
MatValue<T> clamp(MatExpr<T> const& x, MatValue<T> const& min, MatValue<T> const& max)
{
    static_assert(MatShape<T>::is_static);
    return x.cwiseMax(min).cwiseMin(max);
}

template <typename T>
MatValue<T> clamp(MatExpr<T> const& x, MatScalar<T> const min, MatScalar<T> const max)
{
    static_assert(MatShape<T>::is_static);
    return x.cwiseMax(min).cwiseMin(max);
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num saturate(Num const x)
{
    static_assert(is_real<Num>);
    return (x < Num{0.0}) ? Num{0.0} : ((x > Num{1.0}) ? Num{1.0} : x);
}

template <typename T>
MatValue<T> saturate(MatExpr<T> const& x)
{
    using Scalar = MatScalar<T>;
    static_assert(MatShape<T>::is_static);
    return clamp(x, Scalar{0.0}, Scalar{1.0});
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num abs(Num const x)
{
    static_assert(is_signed<Num>);
    return (x < Num{0}) ? -x : x;
}

template <typename T>
MatValue<T> abs(MatExpr<T> const& x)
{
    static_assert(MatShape<T>::is_static);
    return x.cwiseAbs();
}

/// Returns true if two values are within absolute tolerance of eachother
template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr bool near_equal(Num const a, Num const b, Num const abs_tol)
{
    static_assert(is_real<Num>);
    assert(abs_tol >= Num{0.0});
    return abs(a - b) <= abs_tol;
}

/// Returns true if two values are within tolerance of eachother
template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr bool near_equal(Num const a, Num const b, Num const abs_tol, Num const rel_tol)
{
    // http://realtimecollisiondetection.net/blog/?p=89

    static_assert(is_real<Num>);
    assert(abs_tol >= Num{0.0} && rel_tol >= Num{0.0});

    Num const max_abs = max(abs(a), abs(b));
    return abs(a - b) <= max(abs_tol, rel_tol * max_abs);
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
Num mod(Num const x, Num const y)
{
    if constexpr (is_real<Num>)
    {
        return x - y * std::floor(x / y);
    }
    else if constexpr (is_integer<Num>)
    {
        Num const rem = x % y;
        return (rem != 0 && (rem ^ y) < 0) ? rem + y : rem;
    }
    else if constexpr (is_natural<Num>)
    {
        return x % y;
    }
    else
    {
        static_assert(always_false<Num>);
    }
}

template <typename T>
MatValue<T> mod(MatExpr<T> const& x, MatValue<T> const& y)
{
    using Scalar = MatScalar<T>;
    static_assert(MatShape<T>::is_static);

    if constexpr (is_index<Scalar>)
    {
        return x.binaryExpr(y, [](Scalar const a, Scalar const b) {
            return mod(a, b);
        });
    }
    else
    {
        // Ensures x is only evaluated once
        MatValue<T> const x_val = x;
        return x_val.array() - y.array() * (x_val.array() / y.array()).floor();
    }
}

template <typename T>
MatValue<T> mod(MatExpr<T> const& x, MatScalar<T> const y)
{
    static_assert(MatShape<T>::is_static);
    return mod(x, MatValue<T>::Constant(y));
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
Num wrap(Num const x, Num const x0, Num const x1)
{
    static_assert(is_signed<Num>);
    return mod(x - x0, x1 - x0) + x0;
}

template <typename T>
MatValue<T> wrap(MatExpr<T> const& x, MatValue<T> const& x0, MatValue<T> const& x1)
{
    static_assert(MatShape<T>::is_static);
    return mod(x - x0, x1 - x0) + x0;
}

template <typename T>
MatValue<T> wrap(MatExpr<T> const& x, MatScalar<T> const x0, MatScalar<T> const x1)
{
    using Mat = MatValue<T>;
    static_assert(MatShape<T>::is_static);
    return wrap(x, Mat::Constant(x0), Mat::Constant(x1));
}

/// Returns the fractional component of a number
template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
Num fract(Num const x)
{
    static_assert(is_real<Num>);
    return x - std::floor(x);
}

/// Returns the fractional component of each coefficient
template <typename T>
MatValue<T> fract(MatExpr<T> const& x)
{
    static_assert(MatShape<T>::is_static);

    // Ensures x is only evaluated once
    MatValue<T> const x_val = x;
    return x_val.array() - x_val.array().floor();
}

/// Returns the fractional and whole components of a number
template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
Num fract(Num const x, Num& whole)
{
    static_assert(is_real<Num>);
    whole = std::floor(x);
    return x - whole;
}

/// Returns the fractional and whole components of each coefficient
template <typename T>
MatValue<T> fract(MatExpr<T> const& x, MatValue<T>& whole)
{
    static_assert(MatShape<T>::is_static);

    // Ensures x is only evaluated once
    MatValue<T> const x_val = x;
    whole = x_val.array().floor();
    return x_val - whole;
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num lerp(Num const x0, Num const x1, Num const t)
{
    static_assert(is_real<Num>);
    return x0 + (x1 - x0) * t;
}

template <typename T>
MatValue<T> lerp(MatExpr<T> const& x0, MatValue<T> const& x1, MatValue<T> const& t)
{
    static_assert(MatShape<T>::is_static);

    // Ensures x0 is only evaluated once
    MatValue<T> const x0_val = x0;
    return x0_val + (x1 - x0_val).cwiseProduct(t);
}

template <typename T>
MatValue<T> lerp(MatExpr<T> const& x0, MatValue<T> const& x1, MatScalar<T> const t)
{
    static_assert(MatShape<T>::is_static);

    // Ensures x0 is only evaluated once
    MatValue<T> const x0_val = x0;
    return x0_val + (x1 - x0_val) * t;
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num inv_lerp(Num const x0, Num const x1, Num const x)
{
    static_assert(is_real<Num>);
    return (x - x0) / (x1 - x0);
}

template <typename T>
MatValue<T> inv_lerp(MatExpr<T> const& x0, MatValue<T> const& x1, MatValue<T> const& x)
{
    static_assert(MatShape<T>::is_static);

    // Ensures x0 is only evaluated once
    MatValue<T> const x0_val = x0;
    return (x - x0_val).cwiseQuotient(x1 - x0_val);
}

template <typename T>
MatValue<T> inv_lerp(MatScalar<T> const x0, MatScalar<T> const x1, MatExpr<T> const& x)
{
    static_assert(MatShape<T>::is_static);
    return (x.array() - x0) / (x1 - x0);
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num remap(Num const x, Num const x0, Num const x1, Num const y0, Num const y1)
{
    static_assert(is_real<Num>);
    return lerp(y0, y1, inv_lerp(x0, x1, x));
}

template <typename T>
MatValue<T> remap(
    MatExpr<T> const& x,
    MatValue<T> const& x0,
    MatValue<T> const& x1,
    MatValue<T> const& y0,
    MatValue<T> const& y1)
{
    static_assert(MatShape<T>::is_static);
    return lerp(y0, y1, inv_lerp(x0, x1, x));
}

template <typename T>
MatValue<T> remap(
    MatExpr<T> const& x,
    MatScalar<T> const x0,
    MatScalar<T> const x1,
    MatScalar<T> const y0,
    MatScalar<T> const y1)
{
    static_assert(MatShape<T>::is_static);
    MatValue<T> const t = inv_lerp(x0, x1, x);
    return y0 + (y1 - y0) * t.array();
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num hermite_c1(Num const x)
{
    static_assert(is_real<Num>);
    return x * x * (Num{3.0} - Num{2.0} * x);
}

template <typename T>
MatValue<T> hermite_c1(MatExpr<T> const& x)
{
    using Scalar = MatScalar<T>;
    static_assert(MatShape<T>::is_static);

    // Ensures x is only evaluated once
    MatValue<T> const x_val = x;
    return x_val.array().square() * (Scalar{3.0} - Scalar{2.0} * x_val.array());
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num smooth_step(Num const t)
{
    static_assert(is_real<Num>);
    return hermite_c1(saturate(t));
}

template <typename T>
MatValue<T> smooth_step(MatExpr<T> const& t)
{
    static_assert(MatShape<T>::is_static);
    return hermite_c1(saturate(t));
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num smooth_step(Num const x0, Num const x1, Num const x)
{
    static_assert(is_real<Num>);
    return hermite_c1(saturate(inv_lerp(x0, x1, x)));
}

template <typename T>
MatValue<T> smooth_step(MatExpr<T> const& x0, MatValue<T> const& x1, MatValue<T> const& x)
{
    static_assert(MatShape<T>::is_static);
    return hermite_c1(saturate(inv_lerp(x0, x1, x)));
}

template <typename T>
MatValue<T> smooth_step(MatScalar<T> const x0, MatScalar<T> const x1, MatExpr<T> const& x)
{
    static_assert(MatShape<T>::is_static);
    return hermite_c1(saturate(inv_lerp(x0, x1, x)));
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
constexpr Num ramp(Num const x0, Num const x1, Num const x)
{
    static_assert(is_real<Num>);
    return saturate(inv_lerp(x0, x1, x));
}

template <typename T>
MatValue<T> ramp(MatExpr<T> const& x0, MatValue<T> const& x1, MatValue<T> const& x)
{
    static_assert(MatShape<T>::is_static);
    return saturate(inv_lerp(x0, x1, x));
}

template <typename T>
MatValue<T> ramp(MatScalar<T> const x0, MatScalar<T> const x1, MatExpr<T> const& x)
{
    static_assert(MatShape<T>::is_static);
    return saturate(inv_lerp(x0, x1, x));
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
Num sqrt_safe(Num const x)
{
    static_assert(is_real<Num>);
    return std::sqrt(max(x, Num{0.0}));
}

template <typename T>
MatValue<T> sqrt_safe(MatExpr<T> const& x)
{
    using Scalar = MatScalar<T>;
    static_assert(MatShape<T>::is_static);
    return x.cwiseMax(Scalar{0.0}).cwiseSqrt();
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
Num asin_safe(Num const x)
{
    static_assert(is_real<Num>);
    return std::asin(clamp(x, Num{-1.0}, Num{1.0}));
}

template <typename T>
MatValue<T> asin_safe(MatExpr<T> const& x)
{
    using Scalar = MatScalar<T>;
    static_assert(MatShape<T>::is_static);
    return x.cwiseMax(Scalar{-1.0}).cwiseMin(Scalar{1.0}).array().asin();
}

template <typename Num, std::enable_if_t<is_number<Num>>* = nullptr>
Num acos_safe(Num const x)
{
    static_assert(is_real<Num>);
    return std::acos(clamp(x, Num{-1.0}, Num{1.0}));
}

template <typename T>
MatValue<T> acos_safe(MatExpr<T> const& x)
{
    using Scalar = MatScalar<T>;
    static_assert(MatShape<T>::is_static);
    return x.cwiseMax(Scalar{-1.0}).cwiseMin(Scalar{1.0}).array().acos();
}

template <typename Scalar>
Scalar cross(Vec2<Scalar> const& a, Vec2<Scalar> const& b)
{
    // Hodge star of wedge product bw 1-vectors in R2
    // ⋆ (a1 e1 + a2 e2) ^ (b1 e1 + b2 e2) = ⋆ (a1 b2 - a2 b1) e1 ^ e2 = a1 b2 - a2 b1
    return a.x() * b.y() - a.y() * b.x();
}

/// Returns the cross product with the x axis
template <typename Scalar>
Vec3<Scalar> cross_x(Vec3<Scalar> const& v)
{
    return {Scalar{0}, v.z(), -v.y()};
}

/// Returns the cross product with the y axis
template <typename Scalar>
Vec3<Scalar> cross_y(Vec3<Scalar> const& v)
{
    return {-v.z(), Scalar{0}, v.x()};
}

/// Returns the cross product with the z axis
template <typename Scalar>
Vec3<Scalar> cross_z(Vec3<Scalar> const& v)
{
    return {v.y(), -v.x(), Scalar{0}};
}

/// Returns the perpendicular vector rotated a quarter turn counterclockwise
template <typename Scalar>
Vec2<Scalar> perp_ccw(Vec2<Scalar> const& v)
{
    return {-v.y(), v.x()};
}

/// Returns the perpendicular vector rotated a quarter turn clockwise
template <typename Scalar>
Vec2<Scalar> perp_cw(Vec2<Scalar> const& v)
{
    return {v.y(), -v.x()};
}

/// Returns the projection of one vector onto another
template <typename Real, int dim>
Vec<Real, dim> project(Vec<Real, dim> const& a, Vec<Real, dim> const& b)
{
    static_assert(dim > 0);

    return b * (a.dot(b) / b.squaredNorm());
}

/// Returns the rejection of one vector onto another
template <typename Real, int dim>
Vec<Real, dim> reject(Vec<Real, dim> const& a, Vec<Real, dim> const& b)
{
    static_assert(dim > 0);
    return a - project(a, b);
}

/// Returns the reflection of one vector about another
template <typename Real, int dim>
Vec<Real, dim> reflect(Vec<Real, dim> const& a, Vec<Real, dim> const& b)
{
    static_assert(dim > 0);
    return a - Real{2.0} * reject(a, b);
}

/// Returns true if two vectors are within an absolute tolerance of eachother
template <typename Real, int dim>
bool near_equal(Vec<Real, dim> const& a, Vec<Real, dim> const& b, Real const abs_tol)
{
    static_assert(dim > 0);
    assert(abs_tol >= Real{0.0});
    return (a - b).cwiseAbs().maxCoeff() <= abs_tol;
}

/// Returns true if two vectors are within tolerance of eachother
template <typename Real, int dim>
bool near_equal(
    Vec<Real, dim> const& a,
    Vec<Real, dim> const& b,
    Real const abs_tol,
    Real const rel_tol)
{
    // http://realtimecollisiondetection.net/blog/?p=89

    static_assert(dim > 0);
    assert(abs_tol >= Real{0.0} && rel_tol >= Real{0.0});

    Real const max_abs = max(a.cwiseAbs().maxCoeff(), b.cwiseAbs().maxCoeff());
    return (a - b).cwiseAbs().maxCoeff() <= max(abs_tol, rel_tol * max_abs);
}

/// Returns true if two vectors are within an absolute tolerance of being parallel
template <typename Real, int dim>
bool near_parallel(Vec<Real, dim> const& a, Vec<Real, dim> const& b, Real const abs_tol)
{
    static_assert(dim > 0);

    assert(abs_tol >= Real{0.0});
    return reject(a, b).cwiseAbs().maxCoeff() <= abs_tol;
}

/// Returns true if two vectors are within tolerance of being parallel
template <typename Real, int dim>
bool near_parallel(
    Vec<Real, dim> const& a,
    Vec<Real, dim> const& b,
    Real const abs_tol,
    Real const rel_tol)
{
    // http://realtimecollisiondetection.net/blog/?p=89

    static_assert(dim > 0);
    assert(abs_tol >= Real{0.0} && rel_tol >= Real{0.0});

    Real const max_abs = max(a.cwiseAbs().maxCoeff(), b.cwiseAbs().maxCoeff());
    return reject(a, b).cwiseAbs().maxCoeff() <= max(abs_tol, rel_tol * max_abs);
}

/// Returns the normalized linear interpolation between two quaternions
template <typename Real>
Quat<Real> nlerp(Quat<Real> const& q0, Quat<Real> const& q1, Real const t)
{
    auto c0 = q0.coeffs();
    auto c1 = q1.coeffs();

    // Ensure the shortest path is taken bw the two rotations
    if (c0.dot(c1) < Real{0.0})
        return Quat<Real>(c0 - (c1 + c0) * t).normalized();
    else
        return Quat<Real>(c0 + (c1 - c0) * t).normalized();
}

/// Returns the smallest angle between two vectors
template <typename Real, int dim>
Real angle(Vec<Real, dim> const& a, Vec<Real, dim> const& b)
{
    static_assert(dim > 0);
    return acos_safe(a.dot(b) / std::sqrt(a.squaredNorm() * b.squaredNorm()));
}

/// Returns the signed angle between two vectors
template <typename Real>
Real signed_angle(Vec2<Real> const& a, Vec2<Real> const& b)
{
    return std::atan2(cross(a, b), a.dot(b));
}

/// Returns the signed angle between two vectors relative to a third "up" vector
template <typename Real>
Real signed_angle(Vec3<Real> const& a, Vec3<Real> const& b, Vec3<Real> const& up)
{
    Vec3<Real> const c = a.cross(b);
    return std::atan2(c.norm() * sign(c.dot(up)), a.dot(b));
}

/// Returns the signed angle between two vectors in a plane
template <typename Real>
Real angle_in_plane(Vec3<Real> const& a, Vec3<Real> const& b, Vec3<Real> const& normal)
{
    return signed_angle(reject(a, normal), reject(b, normal), normal);
}

/// Returns the sine of the smallest angle between two vectors
template <typename Real>
Real sin_angle(Vec3<Real> const& a, Vec3<Real> const& b)
{
    // |cross(a, b)| = |a||b|sin(theta)
    Real const d = a.squaredNorm() * b.squaredNorm();
    return (d > Real{0.0}) ? a.cross(b).norm() / std::sqrt(d) : Real{0.0};
}

/// Returns the cosine of the smallest angle between two vectors
template <typename Real>
Real cos_angle(Vec3<Real> const& a, Vec3<Real> const& b)
{
    // dot(a, b) = |a||b|cos(theta)
    Real const d = a.squaredNorm() * b.squaredNorm();
    return (d > Real{0.0}) ? a.dot(b) / std::sqrt(d) : Real{0.0};
}

/// Returns the tangent of the smallest angle between two vectors
template <typename Real>
Real tan_angle(Vec3<Real> const& a, Vec3<Real> const& b)
{
    // |cross(a, b)| / dot(a, b) = |a||b|sin(theta) / |a||b|cos(theta) = tan(theta)
    return a.cross(b).norm() / a.dot(b);
}

/// Returns the cotangent of the smallest angle between two vectors
template <typename Real>
Real cot_angle(Vec3<Real> const& a, Vec3<Real> const& b)
{
    // dot(a, b) / |cross(a, b)| = |a||b|cos(theta) / |a||b|sin(theta) = cot(theta)
    return a.dot(b) / a.cross(b).norm();
}

/// Returns the least-squares solution to an overdetermined linear system
template <typename T, typename U>
auto solve_least_squares(MatExpr<T> const& A, MatExpr<U> const& b)
{
    static_assert(MatShape<T>::is_static);
    static_assert(MatShape<U>::is_static);

    static_assert(MatShape<T>::rows > MatShape<T>::cols);
    static_assert(MatShape<T>::rows == MatShape<U>::rows);

    // Ensures A is only evaluated once
    MatValue<T> const A_val = A;
    return ((A_val.transpose() * A_val).inverse() * (A_val.transpose() * b)).eval();
}

} // namespace dr