#pragma once

#include <dr/dynamic_array.hpp>
#include <dr/math_traits.hpp>
#include <dr/math_types.hpp>
#include <dr/span.hpp>
#include <dr/sparse_linalg_types.hpp>

namespace dr
{

/// Repeats each matrix coefficient a specified number of times along the diagonal. This is
/// equivalent to taking the right Kronecker product with the identity matrix of the specified size
/// (i.e. A ⊗ I).
template <typename Scalar, typename Index>
void repeat_diagonal_each(DynamicArray<Triplet<Scalar, Index>>& coeffs, Index const count)
{
    static_assert(is_integer<Index> || is_natural<Index>);

    usize const num_coeffs = coeffs.size();

    for (usize i = 0; i < num_coeffs; ++i)
    {
        auto& c = coeffs[i];
        c = {c.row() * count, c.col() * count, c.value()};

        for (Index j = 1; j < count; ++j)
            coeffs.emplace_back(c.row() + j, c.col() + j, c.value());
    }
}

/// Repeats all matrix coefficients a specified number of times along the diagonal. This is
/// equivalent to taking the left Kronecker product with the identity matrix of the specified size
/// (i.e. I ⊗ A).
template <typename Scalar, typename Index>
void repeat_diagonal_all(
    DynamicArray<Triplet<Scalar, Index>>& coeffs,
    Index const rows,
    Index const cols,
    Index const count)
{
    static_assert(is_integer<Index> || is_natural<Index>);

    for (Index i = 1; i < count; ++i)
    {
        usize const num_coeffs = coeffs.size();

        for (usize j = 0; j < num_coeffs; ++j)
        {
            auto const& c = coeffs[j];
            coeffs.emplace_back(c.row() + i * rows, c.col() + i * cols, c.value());
        }
    }
}

/// Symmetrizes a matrix representing a quadratic form by taking the average with its transpose
template <typename Real, typename Index>
void symmetrize_quadratic(DynamicArray<Triplet<Real, Index>>& coeffs)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    usize const num_coeffs = coeffs.size();

    for (usize i = 0; i < num_coeffs; ++i)
    {
        auto& c = coeffs[i];
        c = {c.row(), c.col(), c.value() * Real{0.5}};
        coeffs.emplace_back(c.col(), c.row(), c.value());
    }
}

} // namespace dr