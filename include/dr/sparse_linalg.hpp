#pragma once

#include <dr/math_types.hpp>
#include <dr/span.hpp>
#include <dr/sparse_linalg_types.hpp>

#include <dr/shim/pmr/vector.hpp>

namespace dr
{

/// Collects the coefficients of the incidence matrix for the given mesh elements
template <typename Scalar, typename Index, int size>
void incidence_coeffs(
    Span<Vec<Index, size> const> const& elements,
    std::pmr::vector<Triplet<Scalar, Index>>& result)
{
    result.clear();

    Index const num_elems = static_cast<Index>(elements.size());
    result.reserve(num_elems * size);

    for (Index i = 0; i < num_elems; ++i)
    {
        auto const& e = elements[i];

        for (int j = 0; j < size; ++j)
            result.emplace_back(e[j], i, Scalar{1});
    }
}

/// Creates the incidence matrix for the given elements
template <typename Scalar, typename Index, int size>
void incidence_mat(
    Span<Vec<Index, size> const> const& elements,
    Index const rows,
    std::pmr::vector<Triplet<Scalar, Index>>& coeffs,
    SparseMat<Scalar, Index>& result)
{
    incidence_coeffs(elements, coeffs);
    result.resize(rows, elements.size());
    result.setFromTriplets(coeffs.begin(), coeffs.end());
}

/// Repeats each matrix coefficient a specified number of times along the diagonal. This is equivalent to taking the
/// right Kronecker product with the identity matrix of the specified size (i.e. A ⊗ I).
template <typename Scalar, typename Index>
void repeat_diagonal_each(std::pmr::vector<Triplet<Scalar, Index>>& coeffs, Index const count)
{
    usize const num_coeffs = coeffs.size();

    for (usize i = 0; i < num_coeffs; ++i)
    {
        auto& c = coeffs[i];
        c = {c.row() * count, c.col() * count, c.value()};

        for (Index j = 1; j < count; ++j)
            coeffs.emplace_back(c.row() + j, c.col() + j, c.value());
    }
}

/// Repeats all matrix coefficients a specified number of times along the diagonal. This is equivalent to taking the
/// left Kronecker product with the identity matrix of the specified size (i.e. I ⊗ A).
template <typename Scalar, typename Index>
void repeat_diagonal_all(
    std::pmr::vector<Triplet<Scalar, Index>>& coeffs,
    Index const rows,
    Index const cols,
    Index const count)
{
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
void symmetrize_quadratic(std::pmr::vector<Triplet<Real, Index>>& coeffs)
{
    usize const num_coeffs = coeffs.size();

    for (usize i = 0; i < num_coeffs; ++i)
    {
        auto& c = coeffs[i];
        c = {c.row(), c.col(), c.value() * Real{0.5}};
        coeffs.emplace_back(c.col(), c.row(), c.value());
    }
}

} // namespace dr