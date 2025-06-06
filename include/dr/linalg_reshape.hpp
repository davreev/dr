#pragma once

#include <array>

#include <dr/container_utils.hpp>
#include <dr/linalg_types.hpp>
#include <dr/memory.hpp>

namespace dr
{

/// Creates a fixed-size vector view of the given scalars
template <typename Scalar, int size>
auto as_vec(Scalar (&coeffs)[size])
{
    return MatView<Vec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size vector view of the given scalars
template <typename Scalar, int size>
auto as_vec(Scalar const (&coeffs)[size])
{
    return MatView<Vec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size vector view of the given scalars
template <int size, typename Scalar>
auto as_vec(Scalar* const coeffs)
{
    return MatView<Vec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size vector view of the given scalars
template <int size, typename Scalar>
auto as_vec(Scalar const* const coeffs)
{
    return MatView<Vec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size column view of the given scalars
template <typename Scalar, int size>
auto as_col(Scalar (&coeffs)[size])
{
    return MatView<Vec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size column view of the given scalars
template <typename Scalar, int size>
auto as_col(Scalar const (&coeffs)[size])
{
    return MatView<Vec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size column view of the given scalars
template <int size, typename Scalar>
auto as_col(Scalar* const coeffs)
{
    return MatView<Vec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size column view of the given scalars
template <int size, typename Scalar>
auto as_col(Scalar const* const coeffs)
{
    return MatView<Vec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size vector view of the given scalars
template <typename Scalar, int size>
auto as_covec(Scalar (&coeffs)[size])
{
    return MatView<Covec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size vector view of the given scalars
template <typename Scalar, int size>
auto as_covec(Scalar const (&coeffs)[size])
{
    return MatView<Covec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size covector view of the given scalars
template <int size, typename Scalar>
auto as_covec(Scalar* const coeffs)
{
    return MatView<Covec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size covector view of the given scalars
template <int size, typename Scalar>
auto as_covec(Scalar const* const coeffs)
{
    return MatView<Covec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size vector view of the given scalars
template <typename Scalar, int size>
auto as_row(Scalar (&coeffs)[size])
{
    return MatView<Covec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size vector view of the given scalars
template <typename Scalar, int size>
auto as_row(Scalar const (&coeffs)[size])
{
    return MatView<Covec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size row view of the given scalars
template <int size, typename Scalar>
auto as_row(Scalar* const coeffs)
{
    return MatView<Covec<Scalar, size>>{coeffs};
}

/// Creates a fixed-size row view of the given scalars
template <int size, typename Scalar>
auto as_row(Scalar const* const coeffs)
{
    return MatView<Covec<Scalar, size> const>{coeffs};
}

/// Creates a fixed-size matrix view of the given scalars
template <int rows, int cols, typename Scalar>
auto as_mat(Scalar* const coeffs)
{
    return MatView<Mat<Scalar, rows, cols>>{coeffs};
}

/// Creates a fixed-size matrix view of the given scalars
template <int rows, int cols, typename Scalar>
auto as_mat(Scalar const* const coeffs)
{
    return MatView<Mat<Scalar, rows, cols> const>{coeffs};
}

/// Creates a fixed-size matrix view of the given vectors
template <int cols, typename Scalar, int size>
auto as_mat(Vec<Scalar, size>* const vecs)
{
    return MatView<Mat<Scalar, size, cols>>{vecs->data()};
}

/// Creates a fixed-size matrix view of the given vectors
template <int cols, typename Scalar, int size>
auto as_mat(Vec<Scalar, size> const* const vecs)
{
    return MatView<Mat<Scalar, size, cols> const>{vecs->data()};
}

/// Creates a fixed-size matrix view of the given covectors
template <int rows, typename Scalar, int size>
auto as_mat(Covec<Scalar, size>* const covecs)
{
    return MatView<Mat<Scalar, rows, size, Eigen::RowMajor>>{covecs->data()};
}

/// Creates a fixed-size matrix view of the given covectors
template <int rows, typename Scalar, int size>
auto as_mat(Covec<Scalar, size> const* const covecs)
{
    return MatView<Mat<Scalar, rows, size, Eigen::RowMajor> const>{covecs->data()};
}

/// Creates a vector view of the given scalars
template <typename Scalar>
auto as_vec(Span<Scalar> const& coeffs)
{
    return MatView<Vec<Scalar>>{coeffs.data(), coeffs.size()};
}

/// Creates a vector view of the given scalars
template <typename Scalar>
auto as_vec(Span<Scalar const> const& coeffs)
{
    return MatView<Vec<Scalar> const>{coeffs.data(), coeffs.size()};
}

/// Creates a covector view of the given scalars
template <typename Scalar>
auto as_covec(Span<Scalar> const& coeffs)
{
    return MatView<Covec<Scalar>>{coeffs.data(), coeffs.size()};
}

/// Creates a covector view of the given scalars
template <typename Scalar>
auto as_covec(Span<Scalar const> const& coeffs)
{
    return MatView<Covec<Scalar> const>{coeffs.data(), coeffs.size()};
}

/// Creates a matrix view of the given scalars
template <typename Scalar>
auto as_mat(Span<Scalar> const& coeffs, isize const rows)
{
    return MatView<Mat<Scalar>>{coeffs.data(), rows, coeffs.size() / rows};
}

/// Creates a matrix view of the given scalars
template <typename Scalar>
auto as_mat(Span<Scalar const> const& coeffs, isize const rows)
{
    return MatView<Mat<Scalar> const>{coeffs.data(), rows, coeffs.size() / rows};
}

/// Creates a matrix view of the given vectors
template <typename Scalar, int size>
auto as_mat(Span<Vec<Scalar, size>> const& vecs)
{
    return MatView<VecArray<Scalar, size>>{vecs.data()->data(), size, vecs.size()};
}

/// Creates a matrix view of the given vectors
template <typename Scalar, int size>
auto as_mat(Span<Vec<Scalar, size> const> const& vecs)
{
    return MatView<VecArray<Scalar, size> const>{vecs.data()->data(), size, vecs.size()};
}

/// Creates a matrix view of the given covectors
template <typename Scalar, int size>
auto as_mat(Span<Covec<Scalar, size>> const& covecs)
{
    return MatView<CovecArray<Scalar, size>>{covecs.data()->data(), covecs.size(), size};
}

/// Creates a matrix view of the given covectors
template <typename Scalar, int size>
auto as_mat(Span<Covec<Scalar, size> const> const& covecs)
{
    return MatView<CovecArray<Scalar, size> const>{covecs.data()->data(), covecs.size(), size};
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
template <typename Scalar, int size>
Span<Vec<Scalar, size>> as_span(VecArray<Scalar, size>& mat)
{
    return {as<Vec<Scalar, size>>(mat.data()), mat.cols()};
}

/// Creates a span of vectors from the given matrix
template <typename Scalar, int size>
Span<Vec<Scalar, size> const> as_span(VecArray<Scalar, size> const& mat)
{
    return {as<Vec<Scalar, size>>(mat.data()), mat.cols()};
}

/// Deleted to avoid creating a span over a temporary
template <typename Scalar, int size>
Span<Vec<Scalar, size> const> as_span(VecArray<Scalar, size> const&& mat) = delete;

/// Creates a span of covectors from the given matrix
template <typename Scalar, int size>
Span<Covec<Scalar, size>> as_span(CovecArray<Scalar, size>& mat)
{
    return {as<Covec<Scalar, size>>(mat.data()), mat.rows()};
}

/// Creates a span of covectors from the given matrix
template <typename Scalar, int size>
Span<Covec<Scalar, size> const> as_span(CovecArray<Scalar, size> const& mat)
{
    return {as<Covec<Scalar, size>>(mat.data()), mat.rows()};
}

/// Deleted to avoid creating a span over a temporary
template <typename Scalar, int size>
Span<Covec<Scalar, size> const> as_span(CovecArray<Scalar, size> const&& mat) = delete;

/// Allows decomposition of fixed-size matrix types into their coeffs via structured binding
template <typename Derived>
auto unpack(MatExpr<Derived> const& expr)
{
    using Scalar = typename Derived::Scalar;

    constexpr usize m = Derived::RowsAtCompileTime;
    constexpr usize n = Derived::ColsAtCompileTime;
    static_assert(m != Eigen::Dynamic && n != Eigen::Dynamic);

    std::array<Scalar, m * n> result{};
    as_mat<m, n>(result.data()) = expr;
    return result;
}

template <typename Derived>
[[deprecated("Use unpack instead")]]
auto expand(MatExpr<Derived> const& expr)
{
    unpack(expr);
}

} // namespace dr
