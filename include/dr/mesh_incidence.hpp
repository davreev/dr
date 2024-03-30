#pragma once

#include <algorithm>

#include <dr/constants.hpp>
#include <dr/container_utils.hpp>
#include <dr/hash_map.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/math_traits.hpp>
#include <dr/math_types.hpp>
#include <dr/memory.hpp>
#include <dr/span.hpp>

namespace dr
{

template <typename Index, int dim>
struct VertsToSimplex;

template <typename Index>
struct VertsToSimplex<Index, 1>
{
    static_assert(is_integer<Index> || is_natural<Index>);

    struct Key
    {
        Key(Vec2<Index> const& v) : v_{v} {}

        Vec2<Index> const& verts() const { return v_; }

        /// Returns the key associated with the oppositely oriented simplex
        Key opposite() const { return {v_({1, 0})}; }

        operator Vec2<Index>() const { return v_; }

        bool operator==(Key const& other) const { return v_ == other.v_; }

      private:
        Vec2<Index> v_;
    };

    struct Hash
    {
        usize operator()(Key const& key) const { return hash(as_bytes(key)); }
    };

    using Map = HashMap<Key, Index, Hash>;

    /// Creates a map from vertex pairs to oriented edges. By convention, oppositely oriented
    /// edges are given consecutive indices.
    static void make_from_tris(Span<Vec3<Index> const> const& tri_verts, Map& result)
    {
        result.clear();
        result.reserve(tri_verts.size() * 3); // 3 oriented edges per face

        // Inserts a pair of oriented edges if they don't already exist
        auto const insert = [&](Key const& k, Index const e) {
            if (result.try_emplace(k, e).second)
                result[k.opposite()] = e + 1;
        };

        for (isize f = 0; f < tri_verts.size(); ++f)
        {
            auto const& f_v = tri_verts[f];
            Vec2<Index> const e_v[]{
                f_v({0, 1}),
                f_v({1, 2}),
                f_v({2, 0}),
            };

            insert(e_v[0], size(result));
            insert(e_v[1], size(result));
            insert(e_v[2], size(result));
        }
    }
};

template <typename Index>
struct VertsToSimplex<Index, 2>
{
    static_assert(is_integer<Index> || is_natural<Index>);

    struct Key
    {
        Key(Vec3<Index> const& v)
        {
            // Find the smallest vertex
            Index const first = (v[0] < v[1]) //
                ? (v[0] < v[2]) ? 0 : 2
                : (v[1] < v[2]) ? 1 : 2;

            // Even permuations for each possible vertex
            static constexpr Index perms[][3]{
                {0, 1, 2},
                {1, 2, 0},
                {2, 0, 1},
            };

            v_ = v(perms[first]);
        }

        Vec3<Index> const& verts() const { return v_; }

        /// Returns the key associated with the oppositely oriented simplex
        Key opposite() const { return {v_({0, 2, 1})}; }

        operator Vec3<Index>() const { return v_; }

        bool operator==(Key const& other) const { return v_ == other.v_; }

      private:
        Vec3<Index> v_;
    };

    struct Hash
    {
        usize operator()(Key const& key) const { return hash(as_bytes(key)); }
    };

    using Map = HashMap<Key, Index, Hash>;

    /// Creates a map from vertex triples to oriented triangles. By convention, oppositely
    /// oriented triangles are given consecutive indices.
    static void make_from_tets(Span<Vec4<Index> const> const& tet_verts, Map& result)
    {
        result.clear();
        result.reserve(tet_verts.size() * 4); // 4 oriented faces per cell

        // Inserts a pair of oriented faces if they don't already exist
        auto const insert = [&](Key const& k, Index const f) {
            if (result.try_emplace(k, f).second)
                result[k.opposite()] = f + 1;
        };

        for (isize c = 0; c < tet_verts.size(); ++c)
        {
            auto const& c_v = tet_verts[c];
            Vec3<Index> const f_v[]{
                c_v({0, 1, 2}),
                c_v({1, 0, 3}),
                c_v({2, 3, 0}),
                c_v({3, 2, 1}),
            };

            insert(f_v[0], size(result));
            insert(f_v[1], size(result));
            insert(f_v[2], size(result));
            insert(f_v[3], size(result));
        }
    }
};

template <typename Index>
struct VertsToSimplex<Index, 3>
{
    static_assert(is_integer<Index> || is_natural<Index>);

    struct Key
    {
        Key(Vec4<Index> const& v)
        {
            // Find the two smallest vertices
            auto const comp = [&](Index const i, Index const j) { return v[i] < v[j]; };
            Index p[]{0, 1, 2, 3};
            std::nth_element(p, p + 1, p + 4, comp);

            // Even permuations for each possible pair
            static constexpr Index perms[][4][4]{
                {
                    {},
                    {0, 1, 2, 3},
                    {0, 2, 3, 1},
                    {0, 3, 1, 2},
                },
                {
                    {1, 0, 3, 2},
                    {},
                    {1, 2, 0, 3},
                    {1, 3, 2, 0},
                },
                {
                    {2, 0, 1, 3},
                    {2, 1, 3, 0},
                    {},
                    {2, 3, 0, 1},
                },
                {
                    {3, 0, 2, 1},
                    {3, 1, 0, 2},
                    {3, 2, 1, 0},
                    {},
                },
            };

            v_ = v(perms[p[0]][p[1]]);
        }

        Vec4<Index> const& verts() const { return v_; }

        /// Returns the key associated with the oppositely oriented simplex
        Key opposite() const { return {v_({0, 1, 3, 2})}; }

        operator Vec4<Index>() const { return v_; }

        bool operator==(Key const& other) const { return v_ == other.v_; }

      private:
        Vec4<Index> v_;
    };

    struct Hash
    {
        usize operator()(Key const& key) const { return hash(as_bytes(key)); }
    };

    using Map = HashMap<Key, Index, Hash>;
};

template <typename Index>
using VertsToEdge = VertsToSimplex<Index, 1>;

template <typename Index>
using VertsToTri = VertsToSimplex<Index, 2>;

template <typename Index>
using VertsToTet = VertsToSimplex<Index, 3>;

/// Returns the vertex opposite to each oriented edge within the same triangle
template <typename Index>
void collect_edge_opposite_verts(
    Span<Vec3<Index> const> const& tri_verts,
    typename VertsToEdge<Index>::Map const& verts_to_edge,
    Span<Index> const result)
{
    assert(result.size() == size(verts_to_edge));
    as_vec(result).setConstant(invalid_index<Index>);

    for (isize f = 0; f < tri_verts.size(); ++f)
    {
        auto const& f_v = tri_verts[f];
        Vec2<Index> const e_v[] = {
            f_v({0, 1}),
            f_v({1, 2}),
            f_v({2, 0}),
        };

        if (auto const it = verts_to_edge.find(e_v[0]); it != verts_to_edge.end())
            result[it->second] = f_v[2];

        if (auto const it = verts_to_edge.find(e_v[1]); it != verts_to_edge.end())
            result[it->second] = f_v[0];

        if (auto const it = verts_to_edge.find(e_v[2]); it != verts_to_edge.end())
            result[it->second] = f_v[1];
    }
}

/// Returns the triangle incident to each oriented edge
template <typename Index>
void collect_edge_tris(
    Span<Vec3<Index> const> const& tri_verts,
    typename VertsToEdge<Index>::Map const& verts_to_edge,
    Span<Index> const result)
{
    assert(result.size() == size(verts_to_edge));
    as_vec(result).setConstant(invalid_index<Index>);

    for (isize f = 0; f < tri_verts.size(); ++f)
    {
        auto const& f_v = tri_verts[f];
        Vec2<Index> const e_v[] = {
            f_v({0, 1}),
            f_v({1, 2}),
            f_v({2, 0}),
        };

        if (auto const it = verts_to_edge.find(e_v[0]); it != verts_to_edge.end())
            result[it->second] = static_cast<Index>(f);

        if (auto const it = verts_to_edge.find(e_v[1]); it != verts_to_edge.end())
            result[it->second] = static_cast<Index>(f);

        if (auto const it = verts_to_edge.find(e_v[2]); it != verts_to_edge.end())
            result[it->second] = static_cast<Index>(f);
    }
}

/// Returns the oriented edges incident to each triangle
template <typename Index>
void collect_tri_edges(
    Span<Vec3<Index> const> const& tri_verts,
    typename VertsToEdge<Index>::Map const& verts_to_edge,
    Span<Vec3<Index>> const result)
{
    assert(result.size() == tri_verts.size());
    as_mat(result).setConstant(invalid_index<Index>);

    for (isize f = 0; f < tri_verts.size(); ++f)
    {
        auto const& f_v = tri_verts[f];
        Vec2<Index> const e_v[] = {
            f_v({0, 1}),
            f_v({1, 2}),
            f_v({2, 0}),
        };

        if (auto const it = verts_to_edge.find(e_v[0]); it != verts_to_edge.end())
            result[f][0] = it->second;

        if (auto const it = verts_to_edge.find(e_v[1]); it != verts_to_edge.end())
            result[f][1] = it->second;

        if (auto const it = verts_to_edge.find(e_v[2]); it != verts_to_edge.end())
            result[f][2] = it->second;
    }
}

} // namespace dr