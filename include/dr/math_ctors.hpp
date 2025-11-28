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
    return (Mat<Scalar, rows, 2>() << col0, col1).finished();
}

template <typename Scalar, int rows>
Mat<Scalar, rows, 3> mat(
    Vec<Scalar, rows> const& col0,
    Vec<Scalar, rows> const& col1,
    Vec<Scalar, rows> const& col2)
{
    return (Mat<Scalar, rows, 3>() << col0, col1, col2).finished();
}

template <typename Scalar, int rows>
Mat<Scalar, rows, 4> mat(
    Vec<Scalar, rows> const& col0,
    Vec<Scalar, rows> const& col1,
    Vec<Scalar, rows> const& col2,
    Vec<Scalar, rows> const& col3)
{
    return (Mat<Scalar, rows, 4>() << col0, col1, col2, col3).finished();
}

template <typename Scalar, int cols>
Mat<Scalar, 2, cols> mat(Covec<Scalar, cols> const& row0, Covec<Scalar, cols> const& row1)
{
    return (Mat<Scalar, 2, cols>() << row0, row1).finished();
}

template <typename Scalar, int cols>
Mat<Scalar, 3, cols> mat(
    Covec<Scalar, cols> const& row0,
    Covec<Scalar, cols> const& row1,
    Covec<Scalar, cols> const& row2)
{
    return (Mat<Scalar, 3, cols>() << row0, row1, row2).finished();
}

template <typename Scalar, int cols>
Mat<Scalar, 4, cols> mat(
    Covec<Scalar, cols> const& row0,
    Covec<Scalar, cols> const& row1,
    Covec<Scalar, cols> const& row2,
    Covec<Scalar, cols> const& row3)
{
    return (Mat<Scalar, 4, cols>() << row0, row1, row2, row3).finished();
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
    static_assert(Result::RowsAtCompileTime != dynamic_size);
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
    static_assert(Result::ColsAtCompileTime != dynamic_size);
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
