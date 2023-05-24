#pragma once

#include <cassert>
#include <cmath>
#include <type_traits>

#include <algorithm>

#include <dr/geometry.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/linalg_types.hpp>
#include <dr/math_types.hpp>
#include <dr/parallel.hpp>
#include <dr/span.hpp>
#include <dr/sparse_linalg_types.hpp>

#include <dr/shim/pmr/vector.hpp>

namespace dr
{

/// Creates the coefficients of the cotangent Laplace matrix for the given triangle mesh
template <typename Real, typename Index>
void make_cotan_laplacian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    std::pmr::vector<Triplet<Real, Index>>& result)
{
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
        Vec3<Real> const e_d[]{
            vertex_positions[f_v[1]] - vertex_positions[f_v[0]],
            vertex_positions[f_v[2]] - vertex_positions[f_v[1]],
            vertex_positions[f_v[0]] - vertex_positions[f_v[2]],
        };

        // The cotan weight of an edge is the ratio of its dual to primal length (aka Hodge star).
        // If t is the angle bw vectors u and v then it's given by:
        // cot(t) = cos(t) / sin(t) = dot(u, v) / |cross(u, v)|

        // Assign matrix coeffs
        Real const inv_sin = Real{-0.5} / e_d[0].cross(e_d[1]).norm();
        add_coeffs(f_v[0], f_v[1], e_d[1].dot(e_d[2]) * inv_sin);
        add_coeffs(f_v[1], f_v[2], e_d[2].dot(e_d[0]) * inv_sin);
        add_coeffs(f_v[2], f_v[0], e_d[0].dot(e_d[1]) * inv_sin);
    }
}

/// Creates the cotangent Laplace matrix for the given triangle mesh
template <typename Real, typename Index>
void make_cotan_laplacian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    std::pmr::vector<Triplet<Real, Index>>& coeffs,
    SparseMat<Real, Index>& result)
{
    cotan_laplace_coeffs(vertex_positions, face_vertices, coeffs);
    result.resize(vertex_positions.size(), vertex_positions.size());
    result.setFromTriplets(coeffs.begin(), coeffs.end());
}

/// Creates the coefficients of the incidence matrix for the given mesh elements
template <typename Scalar, typename Index, int size>
void make_incidence(
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
void make_incidence(
    Span<Vec<Index, size> const> const& elements,
    Index const rows,
    std::pmr::vector<Triplet<Scalar, Index>>& coeffs,
    SparseMat<Scalar, Index>& result)
{
    incidence_coeffs(elements, coeffs);
    result.resize(rows, elements.size());
    result.setFromTriplets(coeffs.begin(), coeffs.end());
}

/// Creates the coefficients of the vector area matrix from given boundary edges
template <typename Real, typename Index>
void make_vector_area(
    Span<Vec2<Index> const> const& boundary_edge_vertices,
    Index const num_vertices,
    std::pmr::vector<Triplet<Real, Index>>& result)
{
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
    std::pmr::vector<Triplet<Real, Index>>& coeffs,
    SparseMat<Real, Index>& result)
{
    vector_area_coeffs(boundary_edge_vertices, num_vertices, coeffs);
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
    Span<Covec3<Real>> const& result)
{
    assert(result.size() == face_vertices.size());

    for (isize i = 0; i < face_vertices.size(); ++i)
    {
        auto const f_v = face_vertices[i];

        result[i] = eval_gradient(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_scalars[f_v[0]],
            vertex_scalars[f_v[1]],
            vertex_scalars[f_v[2]]);
    }
}

/// Evaluates the gradient of a scalar function defined on mesh vertices. Returns a covector
/// associated with each face.
template <typename Real, typename Index>
void eval_gradient(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Real const> const& vertex_scalars,
    Span<Vec3<Index> const> const& face_vertices,
    ParallelFor const& parallel_for,
    Span<Covec3<Real>> const& result)
{
    assert(result.size() == face_vertices.size());

    parallel_for(face_vertices.size(), [&](isize const i, isize /*thread_id*/) {
        auto const& f_v = face_vertices[i];

        result[i] = eval_gradient(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_scalars[f_v[0]],
            vertex_scalars[f_v[1]],
            vertex_scalars[f_v[2]]);
    });
}

/// Evaluates the Jacobian of a vector-valued function defined on mesh vertices. Returns a matrix
/// associated with each face.
template <typename Real, typename Index>
void eval_jacobian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Real> const> const& vertex_vectors,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Mat3<Real>> const& result)
{
    assert(result.size() == face_vertices.size());

    for (isize i = 0; i < face_vertices.size(); ++i)
    {
        auto const& f_v = face_vertices[i];

        result[i] = eval_jacobian(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_vectors[f_v[0]],
            vertex_vectors[f_v[1]],
            vertex_vectors[f_v[2]]);
    }
}

/// Evaluates the Jacobian of a vector-valued function defined on mesh vertices. Returns a matrix
/// associated with each face.
template <typename Real, typename Index>
void eval_jacobian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Real> const> const& vertex_vectors,
    Span<Vec3<Index> const> const& face_vertices,
    ParallelFor const& parallel_for,
    Span<Mat3<Real>> const& result)
{
    assert(result.size() == face_vertices.size());

    parallel_for(face_vertices.size(), [&](isize const i, isize const /*thread_id*/) {
        auto const& f_v = face_vertices[i];

        result[i] = eval_jacobian(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_vectors[f_v[0]],
            vertex_vectors[f_v[1]],
            vertex_vectors[f_v[2]]);
    });
}

/// Evaluates the (integrated) divergence of a vector-valued function defined on mesh triangles.
/// Returns a scalar associated with each vertex dual cell.
template <typename Real, typename Index, typename FaceFunc>
void eval_divergence(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    FaceFunc&& face_vectors,
    Span<Real> const& result)
{
    static_assert(std::is_invocable_r_v<Vec3<Real>, FaceFunc, Index>);

    assert(result.size() == vertex_positions.size());
    as_vec(result).setZero();

    for (Index i = 0; i < face_vertices.size(); ++i)
    {
        auto const& f_v = face_vertices[i];

        Vec3<Real> const f_v_div = eval_divergence(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            face_vectors(i));

        result[f_v[0]] += f_v_div[0];
        result[f_v[1]] += f_v_div[1];
        result[f_v[2]] += f_v_div[2];
    }
}

/// Evaluates the (integrated) divergence of a vector-valued function defined on mesh triangles.
/// Returns a scalar associated with each vertex dual cell.
template <typename Real, typename Index>
void eval_divergence(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec3<Real> const> const& face_vectors,
    Span<Real> const& result)
{
    return eval_divergence(
        vertex_positions,
        face_vertices,
        [&](Index const i) { return face_vectors[i]; },
        result);
}

/// Evaluates the Laplacian of a scalar-valued function defined on vertices. Returns a scalar
/// associated with each vertex dual cell.
template <typename Real, typename Index>
void eval_laplacian(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Real const> const& vertex_scalars,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Real> const& result)
{
    auto eval_face_grad = [&](Index const face) {
        auto const& f_v = face_vertices[face];

        return eval_gradient(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            vertex_scalars[f_v[0]],
            vertex_scalars[f_v[1]],
            vertex_scalars[f_v[2]]);
    };

    eval_divergence(vertex_positions, face_vertices, eval_face_grad, result);
}

} // namespace dr