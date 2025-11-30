#pragma once

#include <cassert>

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

    for (isize f = 0; f < num_faces; ++f)
    {
        auto const& f_v = face_vertices[f];
        Vec3<Real> const& p0 = vertex_positions[f_v[0]];
        Vec3<Real> const& p1 = vertex_positions[f_v[1]];
        Vec3<Real> const& p2 = vertex_positions[f_v[2]];

        Real w[3];
        cotan_weights<Real>(p1 - p0, p2 - p1, p0 - p2, w[0], w[1], w[2]);

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
void make_incidence_matrix(
    Span<Vec<Index, size> const> const& elements,
    DynamicArray<Triplet<Scalar, Index>>& result)
{
    static_assert(is_integer<Index> || is_natural<Index>);

    result.clear();
    result.reserve(elements.size() * size);

    for (isize i = 0; i < elements.size(); ++i)
    {
        auto const& e = elements[i];

        for (int j = 0; j < size; ++j)
            result.emplace_back(e[j], i, Scalar{1});
    }
}

/// Creates the incidence matrix for the given elements
template <typename Scalar, typename Index, int size>
void make_incidence_matrix(
    Span<Vec<Index, size> const> const& elements,
    SparseMat<Scalar, Index>& result,
    Index rows = -1,
    Allocator alloc = {})
{
    static_assert(is_integer<Index> || is_natural<Index>);

    DynamicArray<Triplet<Scalar, Index>> coeffs{alloc};
    make_incidence_matrix(elements, coeffs);

    if (rows == -1)
        rows = as_mat(elements).maxCoeff();

    result.resize(rows, elements.size());
    result.setFromTriplets(coeffs.begin(), coeffs.end());
}

/// Creates the coefficients of the vector area matrix from given boundary edges
template <typename Real, typename Index>
void make_vector_area_matrix(
    Span<Vec2<Index> const> const& boundary_edge_vertices,
    DynamicArray<Triplet<Real, Index>>& result,
    Index num_vertices = -1)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    result.clear();

    isize const num_edges = boundary_edge_vertices.size();
    result.reserve(num_edges * 2);

    if (num_vertices == -1)
        num_vertices = as_mat(boundary_edge_vertices).maxCoeff();

    for (auto const& e_v : boundary_edge_vertices)
    {
        result.emplace_back(e_v[0], e_v[1] + num_vertices, Real{0.5});
        result.emplace_back(e_v[0] + num_vertices, e_v[1], Real{-0.5});
    }
}

/// Creates the vector area matrix from given boundary edges
template <typename Real, typename Index>
void make_vector_area_matrix(
    Span<Vec2<Index> const> const& boundary_edge_vertices,
    SparseMat<Real, Index>& result,
    Index num_vertices = -1,
    Allocator alloc = {})
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    if (num_vertices == -1)
        num_vertices = as_mat(boundary_edge_vertices).maxCoeff();

    DynamicArray<Triplet<Real, Index>> coeffs{alloc};
    make_vector_area_matrix(boundary_edge_vertices, coeffs, num_vertices);

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

    auto const loop_body = [&](isize const f) {
        auto const f_v = face_vertices[f];
        result[f] = eval_gradient(
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
        for (isize f = 0; f < face_vertices.size(); ++f)
            loop_body(f);
    }
    else
    {
        for (isize f = 0; f < face_vertices.size(); ++f)
            loop_body(f);
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

    auto const loop_body = [&](isize const f) {
        auto const& f_v = face_vertices[f];
        result[f] = eval_jacobian(
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
        for (isize f = 0; f < face_vertices.size(); ++f)
            loop_body(f);
    }
    else
    {
        for (isize f = 0; f < face_vertices.size(); ++f)
            loop_body(f);
    }
}

/// Evaluates the divergence of a vector-valued function defined on mesh faces. Returns an
/// integrated scalar quantity associated with each vertex dual cell.
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

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        Real div[3]{};
        eval_divergence(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            face_vectors[f],
            div[0],
            div[1],
            div[2]);

        result[f_v[0]] += div[0];
        result[f_v[1]] += div[1];
        result[f_v[2]] += div[2];
    }
}

/// Evaluates the Laplacian of a scalar function defined on mesh vertices. Returns an integrated
/// scalar quantity associated with each vertex dual cell.
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

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        Real lap[3];
        eval_laplacian(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_scalars[f_v[0]],
            vertex_scalars[f_v[1]],
            vertex_scalars[f_v[2]],
            lap[0],
            lap[1],
            lap[2]);

        result[f_v[0]] += lap[0];
        result[f_v[1]] += lap[1];
        result[f_v[2]] += lap[2];
    }
}

/// Evaluates the Laplacian of a vector-valued function defined on mesh vertices. Returns an
/// integrated vector quantity associated with each vertex dual cell.
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

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        Vec<Real, dim> lap[3];
        eval_laplacian(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_vectors[f_v[0]],
            vertex_vectors[f_v[1]],
            vertex_vectors[f_v[2]],
            lap[0],
            lap[1],
            lap[2]);

        result[f_v[0]] += lap[0];
        result[f_v[1]] += lap[1];
        result[f_v[2]] += lap[2];
    }
}

} // namespace dr