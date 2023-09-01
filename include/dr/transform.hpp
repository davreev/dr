#pragma once

#include <dr/math_ctors.hpp>
#include <dr/math_traits.hpp>
#include <dr/math_types.hpp>

namespace dr
{

template <typename Real, int dim>
struct Affine
{
    static_assert(is_real<Real>);

    Mat<Real, dim, dim> linear{Mat<Real, dim, dim>::Identity()};
    Vec<Real, dim> translation{};

    /// Returns the inverse of this transformation
    Affine<Real, dim> inverse() const
    {
        Mat<Real, dim, dim> const l_inv = linear.inverse();
        return {
            l_inv,
            l_inv * -translation,
        };
    }

    /// Returns the matrix representation of this transformation
    Mat<Real, dim + 1, dim + 1> to_matrix() const
    {
        // T L
        Mat<Real, dim + 1, dim + 1> m{};
        m.template topLeftCorner<dim, dim>() = linear;
        m.template rightCols<1>() = translation.homogeneous();
        return m;
    }

    /// Returns the matrix representation of this transformation's inverse
    Mat<Real, dim + 1, dim + 1> inverse_to_matrix() const
    {
        // L' T'
        Mat<Real, dim, dim> const l_inv = linear.inverse();
        Mat<Real, dim + 1, dim + 1> m{};
        m.template topLeftCorner<dim, dim>() = l_inv;
        m.template rightCols<1>() = (l_inv * -translation).homogeneous();
        return m;
    }

    /// Applies this transformation to the given vector
    Vec<Real, dim> apply(Vec<Real, dim> const& u) const
    {
        // T L u
        return translation + (linear * u);
    }

    /// Applies this transformation to the given vector
    Vec<Real, dim> operator*(Vec<Real, dim> const& u) const { return apply(u); }

    /// Applies this transformation to another transformation
    Affine<Real, dim> apply(Affine<Real, dim> const& other) const
    {
        // Ta La Tb Lb
        return {
            linear * other.linear,
            translation + linear * other.translation,
        };
    }

    /// Applies this transformation to another transformation
    Affine<Real, dim> operator*(Affine<Real, dim> const& other) const { return apply(other); }
};

template <typename Real>
using Affine2 = Affine<Real, 2>;

template <typename Real>
using Affine3 = Affine<Real, 3>;

template <typename Real, int dim>
struct Rotation;

template <typename Real>
struct Rotation<Real, 2>
{
    static_assert(is_real<Real>);

    /// First basis vector after rotation (assumed to be unit length)
    Vec2<Real> x{Vec2<Real>::UnitX()};

    /// Returns the inverse of this transformation
    Rotation<Real, 2> inverse() const { return {vec(x[0], -x[1])}; }

    /// Returns the matrix representation of this transformation
    Mat2<Real> to_matrix() const { return mat(x, vec(-x[1], x[0])); }

    /// Applies this transformation to the given vector
    Vec2<Real> apply(Vec2<Real> const& u) const { return to_matrix() * u; }

    /// Applies this transformation to the given vector
    Vec2<Real> operator*(Vec2<Real> const& u) const { return apply(u); }

    /// Applies this transformation to another transformation
    Rotation<Real, 2> apply(Rotation<Real, 2> const& other) const { return {apply(other.x)}; }

    /// Applies this transformation to another transformation
    Rotation<Real, 2> operator*(Rotation<Real, 2> const& other) const { return apply(other); }
};

template <typename Real>
struct Rotation<Real, 3>
{
    static_assert(is_real<Real>);

    /// Quaternion representation of the rotation (assumed to be unit length)
    Quat<Real> q{Quat<Real>::Identity()};

    /// Returns the inverse of this transformation
    Rotation<Real, 3> inverse() const { return {q.conjugate()}; }

    /// Returns the matrix representation of this transformation
    Mat3<Real> to_matrix() const { return q.toRotationMatrix(); }

    /// Applies this transformation to the given vector
    Vec3<Real> apply(Vec3<Real> const& u) const { return q * u; }

    /// Applies this transformation to the given vector
    Vec3<Real> operator*(Vec3<Real> const& u) const { return apply(u); }

    /// Applies this transformation to another transformation
    Rotation<Real, 3> apply(Rotation<Real, 3> const& other) const { return {q * other.q}; }

    /// Applies this transformation to another transformation
    Rotation<Real, 3> operator*(Rotation<Real, 3> const& other) const { return apply(other); }
};

template <typename Real>
using Rotation2 = Rotation<Real, 2>;

template <typename Real>
using Rotation3 = Rotation<Real, 3>;

template <typename Real, int dim>
struct Conformal
{
    static_assert(is_real<Real>);

    Rotation<Real, dim> rotation{};
    Vec<Real, dim> translation{};
    Real scale{1.0};

    /// Returns the inverse of this transformation
    Conformal<Real, dim> inverse() const
    {
        Rotation<Real, dim> const r_inv = rotation.inverse();
        Real const s_inv = Real{1.0} / scale;
        return {
            r_inv,
            s_inv * (r_inv * -translation),
            s_inv,
        };
    }

    /// Returns the matrix representation of this transformation
    Mat<Real, dim + 1, dim + 1> to_matrix() const
    {
        // T R S
        Mat<Real, dim + 1, dim + 1> m{};
        m.template topLeftCorner<dim, dim>() = scale * rotation.to_matrix();
        m.template rightCols<1>() = translation.homogeneous();
        return m;
    }

    /// Returns the matrix representation of this transformation's inverse
    Mat<Real, dim + 1, dim + 1> inverse_to_matrix() const
    {
        // S' R' T'
        Mat<Real, dim, dim> const l_inv = rotation.inverse().to_matrix() / scale;
        Mat<Real, dim + 1, dim + 1> m{};
        m.template topLeftCorner<dim, dim>() = l_inv;
        m.template rightCols<1>() = (l_inv * -translation).homogeneous();
        return m;
    }

    /// Applies this transformation to the given vector
    Vec<Real, dim> apply(Vec<Real, dim> const& u) const
    {
        // T R S u
        return translation + (rotation * (scale * u));
    }

    /// Applies this transformation to the given vector
    Vec<Real, dim> operator*(Vec<Real, dim> const& u) const { return apply(u); }

    /// Applies this transformation to another transformation
    Conformal<Real, dim> apply(Conformal<Real, dim> const& other) const
    {
        // Ta Ra Sa Tb Rb Sb
        return {
            rotation * other.rotation,
            translation + rotation * (scale * other.translation),
            scale * other.scale,
        };
    }

    /// Applies this transformation to another transformation
    Conformal<Real, dim> operator*(Conformal<Real, dim> const& other) const { return apply(other); }

    /// Applies the inverse of this transformation to the given vector
    Vec<Real, dim> apply_inverse(Vec<Real, dim> const& u) const
    {
        // S' R' T' u
        Real const s_inv = Real{1.0} / scale;
        return s_inv * (rotation.inverse() * (u - translation));
    }

    /// Applies the inverse of this transformation to another transformation
    Conformal<Real, dim> apply_inverse(Conformal<Real, dim> const& other) const
    {
        // Sa' Ra' Ta' Tb Rb Sb
        Rotation<Real, dim> const r_inv = rotation.inverse();
        Real const s_inv = Real{1.0} / scale;
        return {
            r_inv * other.rotation,
            s_inv * (r_inv * (other.translation - translation)),
            s_inv * other.scale,
        };
    }

    /// Implicit conversion to Affine
    operator Affine<Real, dim>() const { return {scale * rotation.to_matrix(), translation}; };
};

template <typename Real>
using Conformal2 = Conformal<Real, 2>;

template <typename Real>
using Conformal3 = Conformal<Real, 3>;

template <typename Real, int dim>
struct Rigid
{
    static_assert(is_real<Real>);

    Rotation<Real, dim> rotation{};
    Vec<Real, dim> translation{};

    /// Returns the inverse of this transformation
    Rigid<Real, dim> inverse() const
    {
        Rotation<Real, dim> const r_inv = rotation.inverse();
        return {r_inv, r_inv * -translation};
    }

    /// Returns the matrix representation of this transformation
    Mat<Real, dim + 1, dim + 1> to_matrix() const
    {
        // T R
        Mat<Real, dim + 1, dim + 1> m{};
        m.template topLeftCorner<dim, dim>() = rotation.to_matrix();
        m.template rightCols<1>() = translation.homogeneous();
        return m;
    }

    /// Returns the matrix representation of this transformation's inverse
    Mat<Real, dim + 1, dim + 1> inverse_to_matrix() const
    {
        // R' T'
        Mat<Real, dim, dim> const r_inv = rotation.inverse().to_matrix();
        Mat<Real, dim + 1, dim + 1> m{};
        m.template topLeftCorner<dim, dim>() = r_inv;
        m.template rightCols<1>() = (r_inv * -translation).homogeneous();
        return m;
    }

    /// Applies this transformation to the given vector
    Vec<Real, dim> apply(Vec<Real, dim> const& u) const
    {
        // T R u
        return translation + (rotation * u);
    }

    /// Applies this transformation to the given vector
    Vec<Real, dim> operator*(Vec<Real, dim> const& u) const { return apply(u); }

    /// Applies this transformation to another transformation
    Rigid<Real, dim> apply(Rigid<Real, dim> const& other) const
    {
        // Ta Ra Tb Rb
        return {
            rotation * other.rotation,
            translation + rotation * other.translation,
        };
    }

    /// Applies this transformation to another transformation
    Rigid<Real, dim> operator*(Rigid<Real, dim> const& other) const { return apply(other); }

    /// Applies the inverse of this transformation to the given vector
    Vec<Real, dim> apply_inverse(Vec<Real, dim> const& u) const
    {
        // R' T' u
        return rotation.inverse() * (u - translation);
    }

    /// Applies the inverse of this transformation to another transformation
    Rigid<Real, dim> apply_inverse(Rigid<Real, dim> const& other) const
    {
        // Sa' Ra' Ta' Tb Rb Sb
        Rotation<Real, dim> const r_inv = rotation.inverse();
        return {
            r_inv * other.rotation,
            r_inv * (other.translation - translation),
        };
    }

    /// Implicit conversion to Conformal
    operator Conformal<Real, dim>() const { return {rotation, translation}; };

    /// Implicit conversion to Affine
    operator Affine<Real, dim>() const { return {rotation.to_matrix(), translation}; };
};

template <typename Real>
using Rigid2 = Rigid<Real, 2>;

template <typename Real>
using Rigid3 = Rigid<Real, 3>;

} // namespace dr