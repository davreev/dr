#pragma once

#include <dr/math_traits.hpp>
#include <dr/math_types.hpp>

namespace dr
{

template <typename Real, typename _ = Real>
struct Isometry3;

template <typename Real>
struct Isometry3<Real, std::enable_if_t<is_real<Real>, Real>>
{
    Quat<Real> rotation{Quat<Real>::Identity()}; // Assumed to be unit length
    Vec3<Real> translation{Vec3<Real>::Zero()};

    /// Returns the inverse of this transformation
    Isometry3<Real> inverse() const
    {
        Quat<Real> const r_inv = rotation.conjugate();
        return {
            r_inv,
            r_inv * -translation,
        };
    }

    /// Returns the matrix representation of this transformation
    Mat4<Real> to_matrix() const
    {
        // Compute as TR
        Mat4<Real> m;
        m.setIdentity();

        auto r = m.template topLeftCorner<3, 3>();
        r = rotation.toRotationMatrix();

        auto t = m.template rightCols<1>();
        t = translation.homogeneous();

        return m;
    }

    /// Returns the matrix representation of the inverse of this transformation
    Mat4<Real> inverse_to_matrix() const
    {
        // Compute as R'T'
        Mat4<Real> m;
        m.setIdentity();

        auto r = m.template topLeftCorner<3, 3>();
        r = rotation.toRotationMatrix().transpose();

        auto t = m.template rightCols<1>();
        t = (r * -translation).homogeneous();

        return m;
    }

    /// Applies this transformation to the given vector
    Vec3<Real> apply(Vec3<Real> const& u) const
    {
        // T R u
        return translation + (rotation * u);
    }

    /// Applies this transformation to another transformation
    Isometry3<Real> apply(Isometry3<Real> const& other) const
    {
        // Ta Ra Tb Rb
        return {
            rotation * other.rotation,
            translation + rotation * other.translation,
        };
    }

    /// Applies the inverse of this transformation to the given vector
    Vec3<Real> apply_inverse(Vec3<Real> const& u) const
    {
        // R' T' u
        return rotation.conjugate() * (u - translation);
    }

    /// Applies this transformation to another transformation
    Isometry3<Real> apply_inverse(Isometry3<Real> const& other) const
    {
        // Ra' Ta' Tb Rb
        Quat<Real> const r_inv = rotation.conjugate();
        return {
            r_inv * other.rotation,
            r_inv * (other.translation - translation),
        };
    }

    Vec3<Real> operator*(Vec3<Real> const& u) const
    {
        return apply(u);
    }

    Isometry3<Real> operator*(Isometry3<Real> const& other) const
    {
        return apply(other);
    }
};

} // namespace dr