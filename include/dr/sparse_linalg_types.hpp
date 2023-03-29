#pragma once

/*
    Convenience aliases for commonly used sparse Eigen types
*/

#include <Eigen/SparseCore>

#include <dr/basic_types.hpp>

namespace dr
{

template <typename Scalar, typename Index = i32, int options = Eigen::ColMajor>
using SparseMat = Eigen::SparseMatrix<Scalar, options, Index>;

template <typename Scalar, typename Index = i32>
using SparseVec = Eigen::SparseVector<Scalar, Eigen::ColMajor, Index>;

template <typename Scalar, typename Index = i32>
using SparseCovec = Eigen::SparseVector<Scalar, Eigen::RowMajor, Index>;

template <typename Scalar, typename Index = i32>
using Triplet = Eigen::Triplet<Scalar, Index>;

} // namespace dr