#pragma once

#include <dr/container_utils.hpp>
#include <dr/hash_map.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/linalg_types.hpp>
#include <dr/math_types.hpp>
#include <dr/span.hpp>

namespace dr
{

struct ElementHash
{
    template <typename Index, int size>
    constexpr usize operator()(Vec<Index, size> const& indices) const
    {
        return hash<size>(indices.data());
    }

  private:
    template <int size, typename Index>
    static constexpr usize hash(Index const* const indices)
    {
        usize result{17};

        for (int i = 0; i < size; ++i)
            result = 31 * result + static_cast<usize>(indices[i]);

        return result;
    }
};

struct ElementEqual
{
    template <typename Index, int size>
    bool operator()(Vec<Index, size> const& a, Vec<Index, size> const& b) const
    {
        return a == b;
    }
};

template <typename Index, int size>
using IncidenceMap = HashMap<Vec<Index, size>, Index, ElementHash, ElementEqual>;

/// Creates a map from vertex pairs to oriented edges. By convention, oppositely oriented edges are
/// given consecutive indices.
template <typename Index>
void make_vertex_to_edge(
    Span<Vec3<Index> const> const& face_vertices,
    IncidenceMap<Index, 2>& result)
{
    result.clear();
    result.reserve(face_vertices.size() * 3); // Accounts for 3 edges per face
    Index n_e = 0;

    // Inserts a pair of oriented edges between the given pair of vertices
    auto const try_insert = [&](Vec2<Index> const& e_v, Index const e) -> bool {
        if (result.emplace(e_v, e).second)
        {
            // Assign the next index to the oppositely oriented edge
            result[e_v({1, 0})] = e + 1;
            return true;
        }

        return false;
    };

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        if (try_insert(f_v({0, 1}), n_e))
            n_e += 2;

        if (try_insert(f_v({1, 2}), n_e))
            n_e += 2;

        if (try_insert(f_v({2, 0}), n_e))
            n_e += 2;
    }
}

/// Returns the vertex at the start of each oriented edge
template <typename Index>
void collect_edge_start_vertices(
    IncidenceMap<Index, 2> const& vertex_to_edge,
    Span<Index> const result)
{
    assert(result.size() == size(vertex_to_edge));

    for (auto const& [e_v, e] : vertex_to_edge)
        result[e] = e_v[0];
}

/// Returns the vertex opposite to each oriented edge
template <typename Index>
void collect_edge_opposite_vertices(
    IncidenceMap<Index, 2> const& vertex_to_edge,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Index> const result)
{
    assert(result.size() == size(vertex_to_edge));
    as_vec(result).setConstant(-1);

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        if (auto const it = vertex_to_edge.find(f_v({0, 1})); it != vertex_to_edge.end())
            result[it->second] = f_v[2];

        if (auto const it = vertex_to_edge.find(f_v({1, 2})); it != vertex_to_edge.end())
            result[it->second] = f_v[0];

        if (auto const it = vertex_to_edge.find(f_v({2, 0})); it != vertex_to_edge.end())
            result[it->second] = f_v[1];
    }
}

/// Returns the face incident to each oriented edge
template <typename Index>
void collect_edge_faces(
    IncidenceMap<Index, 2> const& vertex_to_edge,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Index> const result)
{
    assert(result.size() == size(vertex_to_edge));
    as_vec(result).setConstant(-1);

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        if (auto const it = vertex_to_edge.find(f_v({0, 1})); it != vertex_to_edge.end())
            result[it->second] = f;

        if (auto const it = vertex_to_edge.find(f_v({1, 2})); it != vertex_to_edge.end())
            result[it->second] = f;

        if (auto const it = vertex_to_edge.find(f_v({2, 0})); it != vertex_to_edge.end())
            result[it->second] = f;
    }
}

/// Returns the oriented edges incident to each face
template <typename Index>
void collect_face_edges(
    IncidenceMap<Index, 2> const& vertex_to_edge,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec3<Index>> const result)
{
    assert(result.size() == face_vertices.size());
    as_mat(result).setConstant(-1);

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        if (auto const it = vertex_to_edge.find(f_v({0, 1})); it != vertex_to_edge.end())
            result[f][0] = it->second;

        if (auto const it = vertex_to_edge.find(f_v({1, 2})); it != vertex_to_edge.end())
            result[f][1] = it->second;

        if (auto const it = vertex_to_edge.find(f_v({2, 0})); it != vertex_to_edge.end())
            result[f][2] = it->second;
    }
}

} // namespace dr