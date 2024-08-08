#pragma once

/*
    Convenience aliases for commonly used Eigen types
*/

#include <Eigen/Dense>

#include <dr/basic_types.hpp>

namespace dr
{

template <int rows, int cols>
constexpr int default_mat_options()
{
    if constexpr(rows == 1 && cols != 1)
        return Eigen::RowMajor;
    else
        return Eigen::ColMajor;
}

template <
    typename Scalar,
    int rows = Eigen::Dynamic,
    int cols = Eigen::Dynamic,
    int options = default_mat_options<rows, cols>()>
using Mat = Eigen::Matrix<Scalar, rows, cols, options>;

template <typename Scalar, int size = Eigen::Dynamic>
using Vec = Mat<Scalar, size, 1, Eigen::ColMajor>;

template <typename Scalar, int size>
using VecArray = Mat<Scalar, size, Eigen::Dynamic, Eigen::ColMajor>;

template <typename Scalar, int size = Eigen::Dynamic>
using Covec = Mat<Scalar, 1, size, Eigen::RowMajor>;

template <typename Scalar, int size>
using CovecArray = Mat<Scalar, Eigen::Dynamic, size, Eigen::RowMajor>;

template <typename Derived>
using MatExpr = Eigen::MatrixBase<Derived>;

template <typename MatType, int options = Eigen::Unaligned, typename Stride = Eigen::Stride<0, 0>>
using MatView = Eigen::Map<MatType, options, Stride>;

enum MatHint : u8
{
    MatHint_None = 0,
    MatHint_Symmetric,
    MatHint_Semidefinite,
    MatHint_Definite,
    _MatHint_Count,
};

enum SolverType : u8
{
    SolverType_Direct = 0,
    SolverType_Iterative,
    _SolverType_Count,
};

} // namespace dr
