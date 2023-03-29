#pragma once

#include <algorithm>

#include <dr/constants.hpp>
#include <dr/container_utils.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/linalg_types.hpp>
#include <dr/math_types.hpp>
#include <dr/span.hpp>

#include <dr/shim/pmr/unordered_map.hpp>

namespace dr
{

struct ElementHash
{
    template <typename Index>
    constexpr usize operator()(Span<Index const> const& indices) const
    {
        return hash(indices.data(), indices.size());
    }

    template <typename Index>
    constexpr usize operator()(Span<Index> const& indices) const
    {
        return hash(indices.data(), indices.size());
    }

    template <typename Index, int size>
    constexpr usize operator()(Vec<Index, size> const& indices) const
    {
        return hash<size>(indices.data());
    }

  private:
    template <typename Index>
    static constexpr usize hash(Index const* const indices, isize const size)
    {
        usize result{17};

        for (isize i = 0; i < size; ++i)
            result = 31 * result + static_cast<usize>(indices[i]);

        return result;
    }

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
    template <typename Index>
    bool operator()(Span<Index const> const& a, Span<Index const> const& b) const
    {
        return std::equal(begin(a), end(a), begin(b));
    }

    template <typename Index>
    bool operator()(Span<Index> const& a, Span<Index> const& b) const
    {
        return std::equal(begin(a), end(a), begin(b));
    }

    template <typename Index, int size>
    bool operator()(Vec<Index, size> const& a, Vec<Index, size> const& b) const
    {
        return std::equal(a.data(), a.data() + size, b.data());
    }
};

template <typename Index, int size>
using IncidenceMap = std::pmr::unordered_map<Vec<Index, size>, Index, ElementHash, ElementEqual>;

template <typename Index>
Index opposite_edge(Index const edge)
{
    assert(edge != invalid_index);
    return edge ^ 1; // Oppositely oriented edges are given consecutive indices
}

/// Creates a map from vertex pairs to directed edges. By convention, oppositely oriented edges are given
/// consecutive indices.
template <typename Index>
void make_vertex_edge_map(
    Span<Vec3<Index> const> const& face_vertices,
    IncidenceMap<Index, 2>& result)
{
    isize const num_faces = face_vertices.size();

    result.clear();
    result.reserve(num_faces * 3); // Avoids further allocs if mesh is genus 0

    Index index = 0;

    // Inserts a pair of directed edges between the given pair of vertices if they're not already
    // connected
    auto try_insert = [&](Index const v0, Index const v1) -> bool {
        // Ensures oppositely oriented edges have consecutive indices
        if (result.insert({{v0, v1}, index}).second)
        {
            result[{v1, v0}] = index + 1;
            return true;
        }

        return false;
    };

    for (isize i = 0; i < num_faces; ++i)
    {
        auto const& f_v = face_vertices[i];

        if (try_insert(f_v[0], f_v[1]))
            index += 2;

        if (try_insert(f_v[1], f_v[2]))
            index += 2;

        if (try_insert(f_v[2], f_v[0]))
            index += 2;
    }
}

/// Returns the vertex at the start of each directed edge
template <typename Index>
void collect_edge_start_vertices(
    IncidenceMap<Index, 2> const& vertex_edge_map,
    Span<Index> const result)
{
    assert(result.size() == size<isize>(vertex_edge_map));

    for (auto const& [e_v, e] : vertex_edge_map)
        result[e] = e_v[0];
}

/// Returns the vertex opposite to each directed edge
template <typename Index>
void collect_edge_opposite_vertices(
    IncidenceMap<Index, 2> const& vertex_edge_map,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Index> const result)
{
    assert(result.size() == size<isize>(vertex_edge_map));

    as_vec(result).setConstant(invalid_index);

    auto try_assign = [&](Vec2<Index> const& e_v, Index const v) -> bool {
        auto const it = vertex_edge_map.find(e_v);

        if (it != vertex_edge_map.end())
        {
            result[it->second] = v;
            return true;
        }

        return false;
    };

    isize const num_faces = face_vertices.size();

    for (isize i = 0; i < num_faces; ++i)
    {
        auto const& f_v = face_vertices[i];
        try_assign({f_v[0], f_v[1]}, f_v[2]);
        try_assign({f_v[1], f_v[2]}, f_v[0]);
        try_assign({f_v[2], f_v[0]}, f_v[1]);
    }
}

/// Returns the face incident to each directed edge
template <typename Index>
void collect_edge_faces(
    IncidenceMap<Index, 2> const& vertex_edge_map,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Index> const result)
{
    assert(result.size() == size<isize>(vertex_edge_map));

    as_vec(result).setConstant(invalid_index);

    auto try_assign = [&](Vec2<Index> const& e_v, Index const f) -> bool {
        auto const it = vertex_edge_map.find(e_v);

        if (it != vertex_edge_map.end())
        {
            result[it->second] = f;
            return true;
        }

        return false;
    };

    isize const num_faces = face_vertices.size();

    for (isize i = 0; i < num_faces; ++i)
    {
        auto const& f_v = face_vertices[i];
        try_assign({f_v[0], f_v[1]}, i);
        try_assign({f_v[1], f_v[2]}, i);
        try_assign({f_v[2], f_v[0]}, i);
    }
}

} // namespace dr