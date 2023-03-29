#pragma once

#include <dr/container_utils.hpp>
#include <dr/linalg_types.hpp>
#include <dr/memory.hpp>

namespace dr
{

/// Creates a fixed-size vector view of the given scalars
template <int size, typename Scalar>
auto as_vec(Scalar* const coeffs)
{
    return Eigen::Map<Vec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size vector view of the given scalars
template <int size, typename Scalar>
auto as_vec(Scalar const* const coeffs)
{
    return Eigen::Map<Vec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size covector view of the given scalars
template <int size, typename Scalar>
auto as_covec(Scalar* const coeffs)
{
    return Eigen::Map<Covec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size covector view of the given scalars
template <int size, typename Scalar>
auto as_covec(Scalar const* const coeffs)
{
    return Eigen::Map<Covec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size matrix view of the given scalars
template <int rows, int cols, typename Scalar>
auto as_mat(Scalar* const coeffs)
{
    return Eigen::Map<Mat<Scalar, rows, cols>>{coeffs};
}

/// Creates a fixed-size matrix view of the given scalars
template <int rows, int cols, typename Scalar>
auto as_mat(Scalar const* const coeffs)
{
    return Eigen::Map<Mat<Scalar, rows, cols> const>{coeffs};
}

/// Creates a fixed-size matrix view of the given vectors
template <int cols, typename Scalar, int size>
auto as_mat(Vec<Scalar, size>* const vecs)
{
    return Eigen::Map<Mat<Scalar, size, cols>>{as<Scalar>(vecs)};
}

/// Creates a fixed-size matrix view of the given vectors
template <int cols, typename Scalar, int size>
auto as_mat(Vec<Scalar, size> const* const vecs)
{
    return Eigen::Map<Mat<Scalar, size, cols> const>{as<Scalar>(vecs)};
}

/// Creates a fixed-size matrix view of the given covectors
template <int rows, typename Scalar, int size>
auto as_mat(Covec<Scalar, size>* const covecs)
{
    return Eigen::Map<Mat<Scalar, rows, size, Eigen::RowMajor>>{as<Scalar>(covecs)};
}

/// Creates a fixed-size matrix view of the given covectors
template <int rows, typename Scalar, int size>
auto as_mat(Covec<Scalar, size> const* const covecs)
{
    return Eigen::Map<Mat<Scalar, rows, size, Eigen::RowMajor> const>{as<Scalar>(covecs)};
}

/// Creates a vector view of the given scalars
template <typename Scalar>
auto as_vec(Span<Scalar> const& coeffs)
{
    return Eigen::Map<VecN<Scalar>>{coeffs.data(), coeffs.size()};
}

/// Creates a vector view of the given scalars
template <typename Scalar>
auto as_vec(Span<Scalar const> const& coeffs)
{
    return Eigen::Map<VecN<Scalar> const>{coeffs.data(), coeffs.size()};
}

/// Creates a covector view of the given scalars
template <typename Scalar>
auto as_covec(Span<Scalar> const& coeffs)
{
    return Eigen::Map<CovecN<Scalar>>{coeffs.data(), coeffs.size()};
}

/// Creates a covector view of the given scalars
template <typename Scalar>
auto as_covec(Span<Scalar const> const& coeffs)
{
    return Eigen::Map<CovecN<Scalar> const>{coeffs.data(), coeffs.size()};
}

/// Creates a matrix view of the given scalars
template <typename Scalar>
auto as_mat(Span<Scalar> const& coeffs, isize const rows)
{
    return Eigen::Map<MatN<Scalar>>{coeffs.data(), rows, coeffs.size() / rows};
}

/// Creates a matrix view of the given scalars
template <typename Scalar>
auto as_mat(Span<Scalar const> const& coeffs, isize const rows)
{
    return Eigen::Map<MatN<Scalar> const>{coeffs.data(), rows, coeffs.size() / rows};
}

/// Creates a matrix view of the given vectors
template <typename Scalar, int size>
auto as_mat(Span<Vec<Scalar, size>> const& vecs)
{
    return Eigen::Map<MatN<Scalar>>{as<Scalar>(vecs.data()), size, vecs.size()};
}

/// Creates a matrix view of the given vectors
template <typename Scalar, int size>
auto as_mat(Span<Vec<Scalar, size> const> const& vecs)
{
    return Eigen::Map<MatN<Scalar> const>{as<Scalar>(vecs.data()), size, vecs.size()};
}

/// Creates a matrix view of the given covectors
template <typename Scalar, int size>
auto as_mat(Span<Covec<Scalar, size>> const& covecs)
{
    return Eigen::Map<MatN<Scalar, Eigen::RowMajor>>{as<Scalar>(covecs.data()), covecs.size(), size};
}

/// Creates a matrix view of the given covectors
template <typename Scalar, int size>
auto as_mat(Span<Covec<Scalar, size> const> const& covecs)
{
    return Eigen::Map<MatN<Scalar, Eigen::RowMajor> const>{as<Scalar>(covecs.data()), covecs.size(), size};
}

/// Creates a span of scalars from the given matrix
template <typename Scalar, int rows, int cols, int options>
Span<Scalar> as_span(Mat<Scalar, rows, cols, options>& mat)
{
    return {mat.data(), mat.size()};
}

/// Creates a span of scalars from the given matrix
template <typename Scalar, int rows, int cols, int options>
Span<Scalar const> as_span(Mat<Scalar, rows, cols, options> const& mat)
{
    return {mat.data(), mat.size()};
}

/// Deleted to avoid creating a span from a temporary
template <typename Scalar, int rows, int cols, int options>
Span<Scalar const> as_span(Mat<Scalar, rows, cols, options> const&& mat) = delete;

/// Creates a span of vectors from the given matrix
template <typename Scalar, int stride>
Span<Vec<Scalar, stride>> as_span(VecArray<Scalar, stride>& mat)
{
    return {as<Vec<Scalar, stride>>(mat.data()), mat.cols()};
}

/// Creates a span of vectors from the given matrix
template <typename Scalar, int stride>
Span<Vec<Scalar, stride> const> as_span(VecArray<Scalar, stride> const& mat)
{
    return {as<Vec<Scalar, stride>>(mat.data()), mat.cols()};
}

/// Deleted to avoid creating a span over a temporary
template <typename Scalar, int stride>
Span<Vec<Scalar, stride> const> as_span(VecArray<Scalar, stride> const&& mat) = delete;

/// Creates a span of covectors from the given matrix
template <typename Scalar, int stride>
Span<Covec<Scalar, stride>> as_span(CovecArray<Scalar, stride>& mat)
{
    return {as<Covec<Scalar, stride>>(mat.data()), mat.rows()};
}

/// Creates a span of covectors from the given matrix
template <typename Scalar, int stride>
Span<Covec<Scalar, stride> const> as_span(CovecArray<Scalar, stride> const& mat)
{
    return {as<Covec<Scalar, stride>>(mat.data()), mat.rows()};
}

/// Deleted to avoid creating a span over a temporary
template <typename Scalar, int stride>
Span<Covec<Scalar, stride> const> as_span(CovecArray<Scalar, stride> const&& mat) = delete;

} // namespace dr
