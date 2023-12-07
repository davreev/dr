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
        static_assert(size != Eigen::Dynamic);
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

template <typename Index>
constexpr Index invalid_element = ~Index{};

/// Rotates the vertices of a triangular element such that the one with the smallest index is first
template <typename Index>
Vec3<Index> orient_face(Vec3<Index> const& face_vertices)
{
    auto& f_v = face_vertices;
    Index const min_index = (f_v[0] < f_v[1]) //
        ? (f_v[0] < f_v[2]) ? 0 : 2
        : (f_v[1] < f_v[2]) ? 1 : 2;

    static constexpr Index perms[3][3]{
        {0, 1, 2},
        {1, 2, 0},
        {2, 0, 1},
    };

    return f_v(perms[min_index]);
}

/// Returns the vertices of an edge element in sorted order
template <typename Index>
Vec2<Index> disorient_edge(Vec2<Index> const& edge_vertices)
{
    auto& e_v = edge_vertices;
    return (e_v[0] > e_v[1]) ? e_v({1, 0}) : e_v;
}

/// Returns the vertices of a triangular element in sorted order
template <typename Index>
Vec3<Index> disorient_face(Vec3<Index> const& face_vertices)
{
    auto const order_pair = [](Index* pair) {
        if (pair[0] > pair[1])
        {
            Index const temp = pair[0];
            pair[0] = pair[1];
            pair[1] = temp;
        }
    };

    auto f_v = face_vertices;
    {
        Index* ptr = f_v.data();
        order_pair(ptr);
        order_pair(ptr + 1);
        order_pair(ptr);
    }
    return f_v;
}

/// Returns the vertices of a face in a tetrahedral element
template <typename Index>
Vec3<Index> tetra_face_vertices(Vec4<Index> const& cell_vertices, Index const face_index)
{
    static constexpr Index f_v[4][3]{
        {0, 1, 2},
        {1, 0, 3},
        {2, 3, 0},
        {3, 2, 1},
    };

    assert(face_index < 4);
    return cell_vertices(f_v[face_index]);
}

/// Finds unique edges in the given triangle mesh. Returns a map from vertex pairs to edge indices.
template <typename Index>
void find_unique_edges(Span<Vec3<Index> const> const& face_vertices, IncidenceMap<Index, 2>& result)
{
    result.clear();
    result.reserve((face_vertices.size() * 3) / 2);

    // Inserts an edge if it doesn't already exist
    auto const try_insert = [&](Vec2<Index> const& e_v, Index const e) {
        result.insert(disorient_edge(e_v), e);
    };

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        Vec3<Index> const& f_v = face_vertices[f];
        try_insert(f_v({0, 1}), size_as<Index>(result));
        try_insert(f_v({1, 2}), size_as<Index>(result));
        try_insert(f_v({2, 0}), size_as<Index>(result));
    }
}

/// Finds unique oriented edges in the given triangle mesh. Returns a map from vertex pairs to
/// oriented edge indices. By convention, oppositely oriented edges are given consecutive indices.
template <typename Index>
void find_unique_oriented_edges(
    Span<Vec3<Index> const> const& face_vertices,
    IncidenceMap<Index, 2>& result)
{
    result.clear();
    result.reserve(face_vertices.size() * 3); // 3 oriented edges per face

    // Inserts a pair of oriented edges if they don't already exist
    auto const try_insert = [&](Vec2<Index> const& e_v, Index const e) {
        if (result.emplace(e_v, e).second)
            result[e_v({1, 0})] = e + 1;
    };

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];
        try_insert(f_v({0, 1}), size_as<Index>(result));
        try_insert(f_v({1, 2}), size_as<Index>(result));
        try_insert(f_v({2, 0}), size_as<Index>(result));
    }
}

/// Finds unique faces in the given tetrahedral mesh. Returns a map from vertex triples to face
/// indices.
template <typename Index>
void find_unique_faces(Span<Vec4<Index> const> const& cell_vertices, IncidenceMap<Index, 3>& result)
{
    result.clear();
    result.reserve(cell_vertices.size() * 2);

    // Inserts a face if it doesn't already exist
    auto const try_insert = [&](Vec2<Index> const& f_v, Index const f) {
        result.insert(disorient_face(f_v), f);
    };

    for (isize c = 0; c < cell_vertices.size(); ++c)
    {
        Vec3<Index> const& c_v = cell_vertices[c];
        try_insert(tetra_face_vertices(c_v, 0), size_as<Index>(result));
        try_insert(tetra_face_vertices(c_v, 1), size_as<Index>(result));
        try_insert(tetra_face_vertices(c_v, 2), size_as<Index>(result));
        try_insert(tetra_face_vertices(c_v, 3), size_as<Index>(result));
    }
}

/// Finds unique oriented faces in the given tetrahedral mesh. Returns a map from vertex triples to
/// oriented face indices. By convention, oppositely oriented faces are given consecutive indices.
template <typename Index>
void find_unique_oriented_faces(
    Span<Vec4<Index> const> const& cell_vertices,
    IncidenceMap<Index, 3>& result)
{
    result.clear();
    result.reserve(cell_vertices.size() * 4); // 4 oriented faces per cell

    // Inserts a pair of oriented faces if they don't already exist
    auto const try_insert = [&](Vec3<Index> f_v, Index const f) {
        f_v = orient_face(f_v);
        if (result.emplace(f_v, f).second)
            result[f_v({0, 2, 1})] = f + 1;
    };

    for (isize c = 0; c < cell_vertices.size(); ++c)
    {
        auto const& c_v = cell_vertices[c];
        try_insert(tetra_face_vertices(c_v, 0), size_as<Index>(result));
        try_insert(tetra_face_vertices(c_v, 1), size_as<Index>(result));
        try_insert(tetra_face_vertices(c_v, 2), size_as<Index>(result));
        try_insert(tetra_face_vertices(c_v, 3), size_as<Index>(result));
    }
}

/// Returns the vertices incident to each edge
template <typename Index>
void collect_edge_vertices(
    IncidenceMap<Index, 2> const& vertex_to_edge,
    Span<Vec2<Index>> const result)
{
    assert(result.size() == size(vertex_to_edge));

    for (auto const& [e_v, e] : vertex_to_edge)
        result[e] = e_v;
}

/// Returns the vertex at the start of each oriented edge
template <typename Index>
void collect_oriented_edge_start_vertices(
    IncidenceMap<Index, 2> const& vertex_to_oriented_edge,
    Span<Index> const result)
{
    assert(result.size() == size(vertex_to_oriented_edge));

    for (auto const& [e_v, e] : vertex_to_oriented_edge)
        result[e] = e_v[0];
}

/// Returns the vertex at the end of each oriented edge
template <typename Index>
void collect_oriented_edge_end_vertices(
    IncidenceMap<Index, 2> const& vertex_to_oriented_edge,
    Span<Index> const result)
{
    assert(result.size() == size(vertex_to_oriented_edge));

    for (auto const& [e_v, e] : vertex_to_oriented_edge)
        result[e] = e_v[1];
}

/// Returns the vertex opposite to each oriented edge within the same triangle
template <typename Index>
void collect_oriented_edge_tip_vertices(
    IncidenceMap<Index, 2> const& vertex_to_oriented_edge,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Index> const result)
{
    assert(result.size() == size(vertex_to_oriented_edge));
    as_vec(result).setConstant(invalid_element<Index>);

    auto& map = vertex_to_oriented_edge;

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        if (auto const it = map.find(f_v({0, 1})); it != map.end())
            result[it->second] = f_v[2];

        if (auto const it = map.find(f_v({1, 2})); it != map.end())
            result[it->second] = f_v[0];

        if (auto const it = map.find(f_v({2, 0})); it != map.end())
            result[it->second] = f_v[1];
    }
}

/// Returns the face incident to each oriented edge
template <typename Index>
void collect_oriented_edge_faces(
    IncidenceMap<Index, 2> const& vertex_to_oriented_edge,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Index> const result)
{
    assert(result.size() == size(vertex_to_oriented_edge));
    as_vec(result).setConstant(invalid_element<Index>);

    auto& map = vertex_to_oriented_edge;

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        if (auto const it = map.find(f_v({0, 1})); it != map.end())
            result[it->second] = f;

        if (auto const it = map.find(f_v({1, 2})); it != map.end())
            result[it->second] = f;

        if (auto const it = map.find(f_v({2, 0})); it != map.end())
            result[it->second] = f;
    }
}

/// Returns the edges incident to each face
template <typename Index>
void collect_face_edges(
    IncidenceMap<Index, 2> const& vertex_to_edge,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec3<Index>> const result)
{
    assert(result.size() == face_vertices.size());
    as_mat(result).setConstant(invalid_element<Index>);

    auto& map = vertex_to_edge;

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        if (auto const it = map.find(disorient_edge(f_v({0, 1}))); it != map.end())
            result[f][0] = it->second;

        if (auto const it = map.find(disorient_edge(f_v({1, 2}))); it != map.end())
            result[f][1] = it->second;

        if (auto const it = map.find(disorient_edge(f_v({2, 0}))); it != map.end())
            result[f][2] = it->second;
    }
}

/// Returns the oriented edges incident to each face
template <typename Index>
void collect_face_oriented_edges(
    IncidenceMap<Index, 2> const& vertex_to_oriented_edge,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec3<Index>> const result)
{
    assert(result.size() == face_vertices.size());
    as_mat(result).setConstant(invalid_element<Index>);

    auto& map = vertex_to_oriented_edge;

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];

        if (auto const it = map.find(f_v({0, 1})); it != map.end())
            result[f][0] = it->second;

        if (auto const it = map.find(f_v({1, 2})); it != map.end())
            result[f][1] = it->second;

        if (auto const it = map.find(f_v({2, 0})); it != map.end())
            result[f][2] = it->second;
    }
}

} // namespace dr