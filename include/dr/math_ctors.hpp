#pragma once

/*
    Convenience constructors for fixed-size math types
*/

#include <dr/math_types.hpp>

namespace dr
{

template <int rows, typename Scalar>
Mat<Scalar, rows, rows> mat(Scalar const diag)
{
    Mat<Scalar, rows, rows> m;
    m.diagonal().array() = diag;
    return m;
}

template <int rows, typename Scalar>
Mat<Scalar, rows, rows> mat(Vec<Scalar, rows> const& diag)
{
    Mat<Scalar, rows, rows> m;
    m.diagonal().array() = diag.array();
    return m;
}

template <int rows, typename Scalar>
Mat<Scalar, rows, rows> mat(Covec<Scalar, rows> const& diag)
{
    Mat<Scalar, rows, rows> m;
    m.diagonal().array() = diag.array();
    return m;
}

template <typename Scalar, int rows>
Mat<Scalar, rows, 2> mat(Vec<Scalar, rows> const& col0, Vec<Scalar, rows> const& col1)
{
    Mat<Scalar, rows, 2> m;
    m.col(0) = col0;
    m.col(1) = col1;
    return m;
}

template <typename Scalar, int rows>
Mat<Scalar, rows, 3> mat(
    Vec<Scalar, rows> const& col0,
    Vec<Scalar, rows> const& col1,
    Vec<Scalar, rows> const& col2)
{
    Mat<Scalar, rows, 3> m;
    m.col(0) = col0;
    m.col(1) = col1;
    m.col(2) = col2;
    return m;
}

template <typename Scalar, int rows>
Mat<Scalar, rows, 4> mat(
    Vec<Scalar, rows> const& col0,
    Vec<Scalar, rows> const& col1,
    Vec<Scalar, rows> const& col2,
    Vec<Scalar, rows> const& col3)
{
    Mat<Scalar, rows, 4> m;
    m.col(0) = col0;
    m.col(1) = col1;
    m.col(2) = col2;
    m.col(3) = col3;
    return m;
}

template <typename Scalar, int cols>
Mat<Scalar, 2, cols> mat(Covec<Scalar, cols> const& row0, Covec<Scalar, cols> const& row1)
{
    Mat<Scalar, 2, cols> m;
    m.row(0) = row0;
    m.row(1) = row1;
    return m;
}

template <typename Scalar, int cols>
Mat<Scalar, 3, cols> mat(
    Covec<Scalar, cols> const& row0,
    Covec<Scalar, cols> const& row1,
    Covec<Scalar, cols> const& row2)
{
    Mat<Scalar, 3, cols> m;
    m.row(0) = row0;
    m.row(1) = row1;
    m.row(2) = row2;
    return m;
}

template <typename Scalar, int cols>
Mat<Scalar, 4, cols> mat(
    Covec<Scalar, cols> const& row0,
    Covec<Scalar, cols> const& row1,
    Covec<Scalar, cols> const& row2,
    Covec<Scalar, cols> const& row3)
{
    Mat<Scalar, 4, cols> m;
    m.row(0) = row0;
    m.row(1) = row1;
    m.row(2) = row2;
    m.row(3) = row3;
    return m;
}

template <int size, typename Scalar>
Vec<Scalar, size> vec(Scalar const coeff)
{
    return Vec<Scalar, size>::Constant(coeff);
}

template <typename Scalar>
Vec<Scalar, 2> vec(Scalar const x, Scalar const y)
{
    return {x, y};
}

template <typename Scalar>
Vec<Scalar, 3> vec(Scalar const x, Scalar const y, Scalar const z)
{
    return {x, y, z};
}

template <typename Scalar>
Vec<Scalar, 4> vec(Scalar const x, Scalar const y, Scalar const z, Scalar const w)
{
    return {x, y, z, w};
}

template <int size, typename Scalar>
Vec<Scalar, size> col(Scalar const coeff)
{
    return Vec<Scalar, size>::Constant(coeff);
}

template <typename Scalar>
Vec<Scalar, 2> col(Scalar const x, Scalar const y)
{
    return {x, y};
}

template <typename Scalar>
Vec<Scalar, 3> col(Scalar const x, Scalar const y, Scalar const z)
{
    return {x, y, z};
}

template <typename Scalar>
Vec<Scalar, 4> col(Scalar const x, Scalar const y, Scalar const z, Scalar const w)
{
    return {x, y, z, w};
}

template <typename Derived>
auto col(MatExpr<Derived> const& expr)
{
    using Result = typename MatExpr<Derived>::EvalReturnType;
    static_assert(Result::ColsAtCompileTime == 1);
    static_assert(Result::RowsAtCompileTime != Eigen::Dynamic);
    return expr.eval();
}

template <int size, typename Scalar>
Covec<Scalar, size> covec(Scalar const coeff)
{
    return Covec<Scalar, size>::Constant(coeff);
}

template <typename Scalar>
Covec<Scalar, 2> covec(Scalar const x, Scalar const y)
{
    return {x, y};
}

template <typename Scalar>
Covec<Scalar, 3> covec(Scalar const x, Scalar const y, Scalar const z)
{
    return {x, y, z};
}

template <typename Scalar>
Covec<Scalar, 4> covec(Scalar const x, Scalar const y, Scalar const z, Scalar const w)
{
    return {x, y, z, w};
}

template <int size, typename Scalar>
Covec<Scalar, size> row(Scalar const coeff)
{
    return Covec<Scalar, size>::Constant(coeff);
}

template <typename Scalar>
Covec<Scalar, 2> row(Scalar const x, Scalar const y)
{
    return {x, y};
}

template <typename Scalar>
Covec<Scalar, 3> row(Scalar const x, Scalar const y, Scalar const z)
{
    return {x, y, z};
}

template <typename Scalar>
Covec<Scalar, 4> row(Scalar const x, Scalar const y, Scalar const z, Scalar const w)
{
    return {x, y, z, w};
}

template <typename Derived>
auto row(MatExpr<Derived> const& expr)
{
    using Result = typename MatExpr<Derived>::EvalReturnType;
    static_assert(Result::RowsAtCompileTime == 1);
    static_assert(Result::ColsAtCompileTime != Eigen::Dynamic);
    return expr.eval();
}

template <typename Scalar>
Complex<Scalar> complex(Scalar const re, Scalar const im)
{
    return {re, im};
}

template <typename Scalar>
Quat<Scalar> quat(Scalar const re, Scalar const im0, Scalar const im1, Scalar const im2)
{
    return {re, im0, im1, im2};
}

template <typename Scalar>
Quat<Scalar> quat(Scalar const re, Vec3<Scalar> const& im)
{
    return {re, im[0], im[1], im[2]};
}

} // namespace dr
