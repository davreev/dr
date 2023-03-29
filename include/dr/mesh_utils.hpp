#pragma once

#include <dr/bitwise.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/math.hpp>
#include <dr/span.hpp>

#include <dr/shim/pmr/vector.hpp>

namespace dr
{

/// Returns the number of vertices in a surface mesh based on L'Huilier's polyhedral formula
template <typename Size>
constexpr Size num_vertices(Size const num_edges, Size const num_faces, Size const num_holes)
{
    // V = 2 - 2n + E - F
    return Size{2} - Size{2} * num_holes + num_edges - num_faces;
}

/// Returns the number of edges in a surface mesh based on L'Huilier's polyhedral formula
template <typename Size>
constexpr Size num_edges(Size const num_vertices, Size const num_faces, Size const num_holes)
{
    // E = V + F - 2 + 2n
    return num_vertices + num_faces - Size{2} + Size{2} * num_holes;
}

/// Returns the number of faces in a surface mesh based on L'Huilier's polyhedral formula
template <typename Size>
constexpr Size num_faces(Size const num_vertices, Size const num_edges, Size const num_holes)
{
    // F = 2 - 2n - V + E
    return Size{2} - Size{2} * num_holes - num_vertices + num_edges;
}

/// Returns the number of holes in a surface mesh based on L'Huilier's polyhedral formula
template <typename Size>
constexpr Size num_holes(Size const num_vertices, Size const num_edges, Size const num_faces)
{
    // n = (2 + E - V - F) / 2
    return (2 + num_edges - num_vertices - num_faces) / 2;
}

template <typename Index, int dim>
void append_elements(
    Span<Vec<Index, dim> const> const& src,
    Index const offset,
    std::pmr::vector<Vec<Index, dim>>& dst)
{
    usize const num_faces = dst.size() + src.size();
    dst.reserve(next_pow2(num_faces));
    dst.resize(num_faces);
    as_mat(as_span(dst).back(src.size())).array() = as_mat(src).array() + offset;
}

template <typename Real, int dim>
void append_attributes(
    Span<Vec<Real, dim> const> const& src,
    std::pmr::vector<Vec<Real, dim>>& dst)
{
    usize const num_verts = dst.size() + src.size();
    dst.reserve(next_pow2(num_verts));
    dst.resize(num_verts);
    as_mat(as_span(dst).back(src.size())) = as_mat(src);
}

} // namespace dr
