#pragma once

/*
    Convenience aliases for commonly used Eigen types
*/

#include <Eigen/Dense>

#include <dr/basic_types.hpp>

namespace dr
{
namespace impl
{

template <int rows, int cols>
struct DefaultMatOptions
{
    static constexpr int value = Eigen::ColMajor;
};

template <int cols>
struct DefaultMatOptions<1, cols>
{
    static constexpr int value = Eigen::RowMajor;
};

template <>
struct DefaultMatOptions<1, 1>
{
    static constexpr int value = Eigen::ColMajor;
};

} // namespace impl

template <int rows, int cols>
inline constexpr int default_mat_options = impl::DefaultMatOptions<rows, cols>::value;

template <typename Scalar, int rows, int cols, int options = default_mat_options<rows, cols>>
using Mat = Eigen::Matrix<Scalar, rows, cols, options>;

template <typename Scalar, int options = Eigen::ColMajor>
using MatN = Mat<Scalar, Eigen::Dynamic, Eigen::Dynamic, options>;

template <typename Scalar, int size>
using Vec = Mat<Scalar, size, 1, Eigen::ColMajor>;

template <typename Scalar>
using VecN = Vec<Scalar, Eigen::Dynamic>;

template <typename Scalar, int stride>
using VecArray = Mat<Scalar, stride, Eigen::Dynamic, Eigen::ColMajor>;

template <typename Scalar, int size>
using Covec = Mat<Scalar, 1, size, Eigen::RowMajor>;

template <typename Scalar>
using CovecN = Covec<Scalar, Eigen::Dynamic>;

template <typename Scalar, int stride>
using CovecArray = Mat<Scalar, Eigen::Dynamic, stride, Eigen::RowMajor>;

template <typename Derived>
using MatBase = Eigen::MatrixBase<Derived>;

enum MatHint : u8
{
    MatHint_None = 0,
    MatHint_Rectangular,
    MatHint_Square,
    MatHint_Symmetric,
    MatHint_Semidefinite,
    MatHint_Definite,
    _MatHint_Count,
};

} // namespace dr