#pragma once

#include <cassert>
#include <type_traits>

#include <dr/dynamic_array.hpp>
#include <dr/geometry.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/linalg_types.hpp>
#include <dr/math_types.hpp>
#include <dr/span.hpp>
#include <dr/sparse_linalg_types.hpp>

namespace dr
{

/// Creates the coefficients of the cotangent Laplace matrix for the given triangle mesh
template <typename Real, typename Index>
void make_cotan_laplacian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    DynamicArray<Triplet<Real, Index>>& result)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    result.clear();

    isize const num_faces = face_vertices.size();
    result.reserve(num_faces * 12);

    auto const add_coeffs = [&](Index const i, Index const j, Real const w) {
        result.emplace_back(i, j, w);
        result.emplace_back(j, i, w);
        result.emplace_back(i, i, -w);
        result.emplace_back(j, j, -w);
    };

    for (isize i = 0; i < num_faces; ++i)
    {
        auto const& f_v = face_vertices[i];
        Vec3<Real> const& p0 = vertex_positions[f_v[0]];
        Vec3<Real> const& p1 = vertex_positions[f_v[1]];
        Vec3<Real> const& p2 = vertex_positions[f_v[2]];
        Vec3<Real> const w = cotan_weights<Real>(p1 - p0, p2 - p1, p0 - p2);
        add_coeffs(f_v[0], f_v[1], w[0]);
        add_coeffs(f_v[1], f_v[2], w[1]);
        add_coeffs(f_v[2], f_v[0], w[2]);
    }
}

/// Creates the cotangent Laplace matrix for the given triangle mesh
template <typename Real, typename Index>
void make_cotan_laplacian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    SparseMat<Real, Index>& result,
    Allocator alloc = {})
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    DynamicArray<Triplet<Real, Index>> coeffs{alloc};
    make_cotan_laplacian(vertex_positions, face_vertices, coeffs);

    result.resize(vertex_positions.size(), vertex_positions.size());
    result.setFromTriplets(coeffs.begin(), coeffs.end());
}

/// Creates the coefficients of the incidence matrix for the given mesh elements
template <typename Scalar, typename Index, int size>
void make_incidence(
    Span<Vec<Index, size> const> const& elements,
    DynamicArray<Triplet<Scalar, Index>>& result)
{
    static_assert(is_integer<Index> || is_natural<Index>);

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
void make_incidence(
    Span<Vec<Index, size> const> const& elements,
    Index const rows,
    SparseMat<Scalar, Index>& result,
    Allocator alloc = {})
{
    static_assert(is_integer<Index> || is_natural<Index>);

    DynamicArray<Triplet<Scalar, Index>> coeffs{alloc};
    make_incidence(elements, coeffs);

    result.resize(rows, elements.size());
    result.setFromTriplets(coeffs.begin(), coeffs.end());
}

/// Creates the coefficients of the vector area matrix from given boundary edges
template <typename Real, typename Index>
void make_vector_area(
    Span<Vec2<Index> const> const& boundary_edge_vertices,
    Index const num_vertices,
    DynamicArray<Triplet<Real, Index>>& result)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    result.clear();

    isize const num_edges = boundary_edge_vertices.size();
    result.reserve(num_edges * 2);

    for (auto const& e_v : boundary_edge_vertices)
    {
        result.emplace_back(e_v[0], e_v[1] + num_vertices, Real{0.5});
        result.emplace_back(e_v[0] + num_vertices, e_v[1], Real{-0.5});
    }
}

/// Creates the vector area matrix from given boundary edges
template <typename Real, typename Index>
void make_vector_area(
    Span<Vec2<Index> const> const& boundary_edge_vertices,
    Index const num_vertices,
    SparseMat<Real, Index>& result,
    Allocator alloc = {})
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    DynamicArray<Triplet<Real, Index>> coeffs{alloc};
    make_vector_area(boundary_edge_vertices, num_vertices, coeffs);

    result.resize(num_vertices * 2, num_vertices * 2);
    result.setFromTriplets(coeffs.begin(), coeffs.end());
}

/// Evaluates the gradient of a scalar function defined on mesh vertices. Returns a covector
/// associated with each face.
template <typename Real, typename Index>
void eval_gradient(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Real const> const& vertex_scalars,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Covec3<Real>> const& result,
    isize const num_threads = 1)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    assert(result.size() == face_vertices.size());
    assert(num_threads > 0);

    auto const body = [&](isize const i) {
        auto const f_v = face_vertices[i];
        result[i] = eval_gradient(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_scalars[f_v[0]],
            vertex_scalars[f_v[1]],
            vertex_scalars[f_v[2]]);
    };

    if (num_threads > 1)
    {
#pragma omp parallel for num_threads(num_threads) schedule(static)
        for (isize i = 0; i < face_vertices.size(); ++i)
            body(i);
    }
    else
    {
        for (isize i = 0; i < face_vertices.size(); ++i)
            body(i);
    }
}

/// Evaluates the Jacobian of a vector-valued function defined on mesh vertices. Returns a matrix
/// associated with each face.
template <typename Real, typename Index>
void eval_jacobian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Real> const> const& vertex_vectors,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Mat3<Real>> const& result,
    isize const num_threads = 1)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    assert(result.size() == face_vertices.size());
    assert(num_threads > 0);

    auto const body = [&](isize const i) {
        auto const& f_v = face_vertices[i];
        result[i] = eval_jacobian(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_vectors[f_v[0]],
            vertex_vectors[f_v[1]],
            vertex_vectors[f_v[2]]);
    };

    if (num_threads > 1)
    {
#pragma omp parallel for num_threads(num_threads) schedule(static)
        for (isize i = 0; i < face_vertices.size(); ++i)
            body(i);
    }
    else
    {
        for (isize i = 0; i < face_vertices.size(); ++i)
            body(i);
    }
}

/// Evaluates the (integrated) divergence of a vector-valued function defined on mesh faces. Returns
/// a scalar associated with each vertex dual cell.
template <typename Real, typename Index>
void eval_divergence(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec3<Real> const> const& face_vectors,
    Span<Real> const& result)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    assert(result.size() == vertex_positions.size());
    assert(face_vertices.size() == face_vectors.size());
    as_vec(result).setZero();

    for (Index i = 0; i < face_vertices.size(); ++i)
    {
        auto const& f_v = face_vertices[i];

        Vec3<Real> const f_v_div = eval_divergence(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            face_vectors[i]);

        result[f_v[0]] += f_v_div[0];
        result[f_v[1]] += f_v_div[1];
        result[f_v[2]] += f_v_div[2];
    }
}

/// Evaluates the Laplacian of a scalar function defined on vertices. Returns a scalar associated
/// with each vertex dual cell.
template <typename Real, typename Index>
void eval_laplacian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Real const> const& vertex_scalars,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Real> const& result)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    assert(result.size() == vertex_positions.size());
    as_vec(result).setZero();

    for (Index i = 0; i < face_vertices.size(); ++i)
    {
        auto const& f_v = face_vertices[i];

        Vec3<Real> const f_v_lap = eval_laplacian(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_scalars[f_v[0]],
            vertex_scalars[f_v[1]],
            vertex_scalars[f_v[2]]);

        result[f_v[0]] += f_v_lap[0];
        result[f_v[1]] += f_v_lap[1];
        result[f_v[2]] += f_v_lap[2];
    }
}

/// Evaluates the Laplacian of a vector-valued function defined on vertices. Returns a vector
/// associated with each vertex dual cell.
template <typename Real, typename Index, int dim>
void eval_laplacian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec<Real, dim> const> const& vertex_vectors,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec<Real, dim>> const& result)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    assert(result.size() == vertex_positions.size());
    as_vec(result).setZero();

    for (Index i = 0; i < face_vertices.size(); ++i)
    {
        auto const& f_v = face_vertices[i];

        Mat<Real, dim, 3> const f_v_lap = eval_laplacian(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_vectors[f_v[0]],
            vertex_vectors[f_v[1]],
            vertex_vectors[f_v[2]]);

        result[f_v[0]] += f_v_lap.col(0);
        result[f_v[1]] += f_v_lap.col(1);
        result[f_v[2]] += f_v_lap.col(2);
    }
}

} // namespace dr