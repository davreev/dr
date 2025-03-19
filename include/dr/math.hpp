#pragma once

/*
    Assorted helper functions and for fixed-size math types
*/

#include <cmath>

#include <dr/math_ctors.hpp>
#include <dr/math_constants.hpp>
#include <dr/math_traits.hpp>
#include <dr/math_types.hpp>

namespace dr
{

template <typename Real>
constexpr Real deg_to_rad(Real const angle)
{
    static_assert(is_real<Real>);
    return angle * (pi<Real> / Real{180.0});
}

template <typename Real>
constexpr Real rad_to_deg(Real const angle)
{
    static_assert(is_real<Real>);
    return angle * (Real{180.0} / pi<Real>);
}

template <typename Scalar>
constexpr Scalar sign(Scalar const x)
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    return (x > Scalar{0}) ? Scalar{1} : ((x < Scalar{0}) ? Scalar{-1} : Scalar{0});
}

template <typename Scalar>
constexpr Scalar min(Scalar const a, Scalar const b)
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    return (b < a) ? b : a;
}

template <typename Scalar>
constexpr Scalar max(Scalar const a, Scalar const b)
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    return (b > a) ? b : a;
}

template <typename Real>
constexpr Real saturate(Real const x)
{
    static_assert(is_real<Real>);
    return (x < Real{0.0}) ? Real{0.0} : ((x > Real{1.0}) ? Real{1.0} : x);
}

template <typename Scalar>
constexpr Scalar clamp(Scalar const x, Scalar const min, Scalar const max)
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    return (x < min) ? min : ((x > max) ? max : x);
}

template <typename Scalar>
constexpr Scalar abs(Scalar const x)
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    return (x < Scalar{0}) ? -x : x;
}

/// Returns true if two values are within absolute tolerance of eachother
template <typename Real>
constexpr bool near_equal(Real const a, Real const b, Real const abs_tol)
{
    static_assert(is_real<Real>);
    assert(abs_tol >= Real{0.0});
    return abs(a - b) <= abs_tol;
}

/// Returns true if two values are within tolerance of eachother
template <typename Real>
constexpr bool near_equal(Real const a, Real const b, Real const abs_tol, Real const rel_tol)
{
    // http://realtimecollisiondetection.net/blog/?p=89

    static_assert(is_real<Real>);
    assert(abs_tol >= Real{0.0} && rel_tol >= Real{0.0});

    Real const max_abs = max(abs(a), abs(b));
    return abs(a - b) <= max(abs_tol, rel_tol * max_abs);
}

template <typename Scalar>
Scalar mod(Scalar const x, Scalar const y)
{
    static_assert(is_real<Scalar> || is_integer<Scalar> || is_natural<Scalar>);

    if constexpr (is_real<Scalar>)
    {
        return x - y * std::floor(x / y);
    }
    else if constexpr (is_integer<Scalar>)
    {
        Scalar const rem = x % y;
        return (rem * y < 0) ? rem + y : rem;
    }
    else if constexpr (is_natural<Scalar>)
    {
        return x % y;
    }
}

/// Returns the fractional component of a number
template <typename Real>
Real fract(Real const x)
{
    static_assert(is_real<Real>);
    return x - std::floor(x);
}

/// Returns the fractional and whole components of a number
template <typename Real>
Real fract(Real const x, Real& whole)
{
    static_assert(is_real<Real>);
    whole = std::floor(x);
    return x - whole;
}

template <typename Real>
constexpr Real lerp(Real const x0, Real const x1, Real const t)
{
    static_assert(is_real<Real>);
    return x0 + (x1 - x0) * t;
}

template <typename Real>
constexpr Real inv_lerp(Real const x0, Real const x1, Real const x)
{
    static_assert(is_real<Real>);
    return (x - x0) / (x1 - x0);
}

template <typename Real>
constexpr Real remap(Real const x, Real const x0, Real const x1, Real const y0, Real const y1)
{
    static_assert(is_real<Real>);
    return lerp(y0, y1, inv_lerp(x0, x1, x));
}

template <typename Real>
constexpr Real hermite_c1(Real const x)
{
    static_assert(is_real<Real>);
    return x * x * (Real{3.0} - Real{2.0} * x);
}

template <typename Real>
constexpr Real smooth_step(Real const t)
{
    static_assert(is_real<Real>);
    return hermite_c1(saturate(t));
}

template <typename Real>
constexpr Real smooth_step(Real const x0, Real const x1, Real const x)
{
    static_assert(is_real<Real>);
    return hermite_c1(saturate(inv_lerp(x0, x1, x)));
}

template <typename Real>
constexpr Real smooth_pulse(Real const center, Real const width, Real const x)
{
    // http://www.iquilezles.org/www/articles/functions/functions.htm

    static_assert(is_real<Real>);
    return hermite_c1(Real{1.0} - saturate(abs(x - center) / width));
}

template <typename Real>
constexpr Real ramp(Real const x0, Real const x1, Real const x)
{
    static_assert(is_real<Real>);
    return saturate(inv_lerp(x0, x1, x));
}

template <typename Scalar>
Scalar cross(Vec2<Scalar> const& a, Vec2<Scalar> const& b)
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);

    // Hodge star of wedge product bw 1-vectors in R2
    // ⋆ (a1 e1 + a2 e2) ^ (b1 e1 + b2 e2) = ⋆ (a1 b2 - a2 b1) e1 ^ e2 = a1 b2 - a2 b1
    return a.x() * b.y() - a.y() * b.x();
}

/// Returns the cross product with the X axis
template <typename Scalar>
Vec3<Scalar> cross_x(Vec3<Scalar> const& v)
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    return Vec3<Scalar>(Scalar{0}, v.z(), -v.y());
}

/// Returns the cross product with the Y axis
template <typename Scalar>
Vec3<Scalar> cross_y(Vec3<Scalar> const& v)
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    return Vec3<Scalar>(-v.z(), Scalar{0}, v.x());
}

/// Returns the cross product with the Z axis
template <typename Scalar>
Vec3<Scalar> cross_z(Vec3<Scalar> const& v)
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    return Vec3<Scalar>(v.y(), -v.x(), Scalar{0});
}

/// Returns the projection of one vector onto another
template <typename Real, int dim>
Vec<Real, dim> project(Vec<Real, dim> const& a, Vec<Real, dim> const& b)
{
    static_assert(is_real<Real>);
    return b * (a.dot(b) / b.squaredNorm());
}

/// Returns the rejection of one vector onto another
template <typename Real, int dim>
Vec<Real, dim> reject(Vec<Real, dim> const& a, Vec<Real, dim> const& b)
{
    static_assert(is_real<Real>);
    return a - project(a, b);
}

/// Returns the reflection of one vector about another
template <typename Real, int dim>
Vec<Real, dim> reflect(Vec<Real, dim> const& a, Vec<Real, dim> const& b)
{
    static_assert(is_real<Real>);
    return a - Real{2.0} * reject(a, b);
}

/// Returns true if two vectors are within an absolute tolerance of eachother
template <typename Real, int dim>
bool near_equal(Vec<Real, dim> const& a, Vec<Real, dim> const& b, Real const abs_tol)
{
    static_assert(is_real<Real>);
    assert(abs_tol >= Real{0.0});
    return (a - b).cwiseAbs().maxCoeff() <= abs_tol;
}

/// Returns true if two vectors are within tolerance of eachother
template <typename Real, int dim>
bool near_equal(Vec<Real, dim> const& a, Vec<Real, dim> const& b, Real const abs_tol, Real const rel_tol)
{
    // http://realtimecollisiondetection.net/blog/?p=89

    static_assert(is_real<Real>);
    assert(abs_tol >= Real{0.0} && rel_tol >= Real{0.0});

    Real const max_abs = max(a.cwiseAbs().maxCoeff(), b.cwiseAbs().maxCoeff());
    return (a - b).cwiseAbs().maxCoeff() <= max(abs_tol, rel_tol * max_abs);
}

/// Returns true if two vectors are within an absolute tolerance of being parallel
template <typename Real, int dim>
bool near_parallel(Vec<Real, dim> const& a, Vec<Real, dim> const& b, Real const abs_tol)
{
    static_assert(is_real<Real>);
    assert(abs_tol >= Real{0.0});
    return reject(a, b).cwiseAbs().maxCoeff() <= abs_tol;
}

/// Returns true if two vectors are within tolerance of being parallel
template <typename Real, int dim>
bool near_parallel(Vec<Real, dim> const& a, Vec<Real, dim> const& b, Real const abs_tol, Real const rel_tol)
{
    // http://realtimecollisiondetection.net/blog/?p=89

    static_assert(is_real<Real>);
    assert(abs_tol >= Real{0.0} && rel_tol >= Real{0.0});

    Real const max_abs = max(a.cwiseAbs().maxCoeff(), b.cwiseAbs().maxCoeff());
    return reject(a, b).cwiseAbs().maxCoeff() <= max(abs_tol, rel_tol * max_abs);
}

/// Returns the normalized linear interpolation between two quaternions
template <typename Real>
Quat<Real> nlerp(Quat<Real> const& q0, Quat<Real> const& q1, Real const t)
{
    static_assert(is_real<Real>);

    auto v0 = q0.vec();
    auto v1 = q1.vec();

    // Ensure the shortest path is taken bw the two rotations
    if (v0.dot(v1) < Real{0.0})
        return Quat<Real>(v0 - (v1 + v0) * t).normalized();
    else
        return Quat<Real>(v0 + (v1 - v0) * t).normalized();
}

template <typename Real>
Real sqrt_safe(Real const x)
{
    static_assert(is_real<Real>);
    return std::sqrt(max(x, Real{0.0}));
}

template <typename Real>
Real asin_safe(Real const x)
{
    static_assert(is_real<Real>);
    return std::asin(clamp(x, Real{-1.0}, Real{1.0}));
}

template <typename Real>
Real acos_safe(Real const x)
{
    static_assert(is_real<Real>);
    return std::acos(clamp(x, Real{-1.0}, Real{1.0}));
}

/// Returns the smallest angle between two vectors
template <typename Real, int dim>
Real angle(Vec<Real, dim> const& a, Vec<Real, dim> const& b)
{
    static_assert(is_real<Real>);
    return acos_safe(a.dot(b) / std::sqrt(a.squaredNorm() * b.squaredNorm()));
}

/// Returns the signed angle between two vectors
template <typename Real>
Real signed_angle(Vec2<Real> const& a, Vec2<Real> const& b)
{
    static_assert(is_real<Real>);
    return std::atan2(cross(a, b), a.dot(b));
}

/// Returns the signed angle between two vectors relative to a third "up" vector
template <typename Real>
Real signed_angle(Vec3<Real> const& a, Vec3<Real> const& b, Vec3<Real> const& up)
{
    static_assert(is_real<Real>);
    Vec3<Real> const c = a.cross(b);
    return std::atan2(c.norm() * sign(c.dot(up)), a.dot(b));
}

/// Returns the signed angle between two vectors in a plane
template <typename Real>
Real angle_in_plane(Vec3<Real> const& a, Vec3<Real> const& b, Vec3<Real> const& normal)
{
    static_assert(is_real<Real>);
    return signed_angle(reject(a, normal), reject(b, normal), normal);
}

/// Returns the sine of the smallest angle between two vectors
template <typename Real>
Real sin_angle(Vec3<Real> const& a, Vec3<Real>& b)
{
    static_assert(is_real<Real>);

    // |cross(a, b)| = |a||b|sin(theta)
    Real const d = a.squaredNorm() * b.squaredNorm();
    return (d > Real{0.0}) ? a.cross(b).norm() / std::sqrt(d) : Real{0.0};
}

/// Returns the cosine of the smallest angle between two vectors
template <typename Real>
Real cos_angle(Vec3<Real> const& a, Vec3<Real>& b)
{
    static_assert(is_real<Real>);

    // dot(a, b) = |a||b|cos(theta)
    Real const d = a.squaredNorm() * b.squaredNorm();
    return (d > Real{0.0}) ? a.dot(b) / std::sqrt(d) : Real{0.0};
}

/// Returns the tangent of the smallest angle between two vectors
template <typename Real>
Real tan_angle(Vec3<Real> const& a, Vec3<Real>& b)
{
    static_assert(is_real<Real>);

    // |cross(a, b)| / dot(a, b) = |a||b|sin(theta) / |a||b|cos(theta) = tan(theta)
    return a.cross(b).norm() / a.dot(b);
}

/// Returns the cotangent of the smallest angle between two vectors
template <typename Real>
Real cot_angle(Vec3<Real> const& a, Vec3<Real>& b)
{
    static_assert(is_real<Real>);

    // dot(a, b) / |cross(a, b)| = |a||b|cos(theta) / |a||b|sin(theta) = cot(theta)
    return a.dot(b) / a.cross(b).norm();
}

/// Returns the least-squares solution to an overdetermined linear system
template <typename Real, int rows, int cols>
Vec<Real, cols> solve_least_squares(Mat<Real, rows, cols> const& A, Vec<Real, rows> const& b)
{
    static_assert(is_real<Real> && rows > cols && cols > 0);
    return (A.transpose() * A).inverse() * (A.transpose() * b);
}

} // namespace dr