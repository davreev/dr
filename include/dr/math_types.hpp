#pragma once

/*
    Convenience aliases for commonly used fixed-size math types
*/

#include <complex>

#include <Eigen/Geometry>

#include <dr/linalg_types.hpp>

namespace dr
{

template <typename Scalar>
using Mat2 = Mat<Scalar, 2, 2>;

template <typename Scalar>
using Mat3 = Mat<Scalar, 3, 3>;

template <typename Scalar>
using Mat4 = Mat<Scalar, 4, 4>;

template <typename Scalar>
using Vec2 = Vec<Scalar, 2>;

template <typename Scalar>
using Vec3 = Vec<Scalar, 3>;

template <typename Scalar>
using Vec4 = Vec<Scalar, 4>;

template <typename Scalar>
using Covec2 = Covec<Scalar, 2>;

template <typename Scalar>
using Covec3 = Covec<Scalar, 3>;

template <typename Scalar>
using Covec4 = Covec<Scalar, 4>;

template <typename Scalar>
using Quat = Eigen::Quaternion<Scalar>;

template <typename Scalar>
using Complex = std::complex<Scalar>;

}