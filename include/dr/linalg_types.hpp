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

template <
    typename Scalar,
    int rows = Eigen::Dynamic,
    int cols = Eigen::Dynamic,
    int options = default_mat_options<rows, cols>>
using Mat = Eigen::Matrix<Scalar, rows, cols, options>;

template <typename Scalar, int size = Eigen::Dynamic>
using Vec = Mat<Scalar, size, 1, Eigen::ColMajor>;

template <typename Scalar, int size>
using VecArray = Mat<Scalar, size, Eigen::Dynamic, Eigen::ColMajor>;

template <typename Scalar, int size = Eigen::Dynamic>
using Covec = Mat<Scalar, 1, size, Eigen::RowMajor>;

template <typename Scalar, int size>
using CovecArray = Mat<Scalar, Eigen::Dynamic, size, Eigen::RowMajor>;

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