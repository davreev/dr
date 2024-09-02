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
namespace impl
{

struct ManyToOne
{
    template <typename Index, int size>
    struct Key;

    template <typename Index>
    struct Key<Index, 2>
    {
        static_assert(is_integer<Index> || is_natural<Index>);

        Vec2<Index> indices;

        Key(Vec2<Index> const& indices) : indices{indices} {}

        /// Returns the key associated with the oppositely oriented element
        Key opposite() const { return {indices({1, 0})}; }

        bool operator==(Key const& other) const { return indices == other.indices; }

        operator Vec2<Index>() const { return indices; }
    };

    template <typename Index>
    struct Key<Index, 3>
    {
        static_assert(is_integer<Index> || is_natural<Index>);

        Vec3<Index> indices;

        Key(Vec3<Index> const& indices)
        {
            // Find index of the smallest element
            Index const p = (indices[0] < indices[1]) //
                ? (indices[0] < indices[2]) ? 0 : 2
                : (indices[1] < indices[2]) ? 1 : 2;

            // Even permuations for each possible result
            static constexpr Index perms[][3]{
                {0, 1, 2},
                {1, 2, 0},
                {2, 0, 1},
            };

            this->indices = indices(perms[p]);
        }

        /// Returns the key associated with the oppositely oriented element
        Key opposite() const { return {indices({0, 2, 1})}; }

        bool operator==(Key const& other) const { return indices == other.indices; }

        operator Vec3<Index>() const { return indices; }
    };

    template <typename Index>
    struct Key<Index, 4>
    {
        static_assert(is_integer<Index> || is_natural<Index>);

        Vec4<Index> indices;

        Key(Vec4<Index> const& indices)
        {
            // Find the indices of the two smallest elements
            auto const comp = [&](Index const i, Index const j) { return indices[i] < indices[j]; };
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

            this->indices = indices(perms[p[0]][p[1]]);
        }

        /// Returns the key associated with the oppositely oriented element
        Key opposite() const { return {indices({0, 1, 3, 2})}; }

        bool operator==(Key const& other) const { return indices == other.indices; }

        operator Vec4<Index>() const { return indices; }
    };

    struct KeyHash : HighQualityHash
    {
        template <typename Index, int size>
        usize operator()(Key<Index, size> const& key) const
        {
            return hash(as_bytes(key));
        }
    };

    template <typename Index, int size>
    using Map = HashMap<Key<Index, size>, Index, KeyHash>;
};

} // namespace impl

template <typename Index>
struct VertsToEdge
{
    using Map = impl::ManyToOne::Map<Index, 2>;

    /// Creates a map from vertex pairs to oriented edges. By convention, oppositely oriented edges
    /// have consecutive indices.
    static void make_from_tris(Span<Vec3<Index> const> const& tri_verts, Map& result)
    {
        using Key = typename Map::key_type;

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

            for (i8 i = 0; i < 3; ++i)
                insert(e_v[i], size(result));
        }
    }
};

template <typename Index>
struct VertsToTri
{
    using Map = impl::ManyToOne::Map<Index, 3>;

    /// Creates a map from vertex triples to oriented triangles. By convention, oppositely oriented
    /// triangles have consecutive indices.
    static void make_from_tets(Span<Vec4<Index> const> const& tet_verts, Map& result)
    {
        using Key = typename Map::key_type;

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

            for (i8 i = 0; i < 4; ++i)
                insert(f_v[i], size(result));
        }
    }
};

template <typename Index>
struct VertsToTet
{
    using Map = impl::ManyToOne::Map<Index, 4>;
};

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

        Index v_op[] = {f_v[2], f_v[0], f_v[1]};
        for (i8 i = 0; i < 3; ++i)
        {
            if (auto const it = verts_to_edge.find(e_v[i]); it != verts_to_edge.end())
                result[it->second] = v_op[i];
        }
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

        for (i8 i = 0; i < 3; ++i)
        {
            if (auto const it = verts_to_edge.find(e_v[i]); it != verts_to_edge.end())
                result[it->second] = static_cast<Index>(f);
        }
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

        auto& f_e = result[f];
        for (i8 i = 0; i < 3; ++i)
        {
            if (auto const it = verts_to_edge.find(e_v[i]); it != verts_to_edge.end())
                f_e[i] = it->second;
        }
    }
}

/// Returns the oriented triangles incident to each tetrahedron
template <typename Index>
void collect_tet_tris(
    Span<Vec4<Index> const> const& tet_verts,
    typename VertsToTri<Index>::Map const& verts_to_tri,
    Span<Vec4<Index>> const result)
{
    assert(result.size() == tet_verts.size());
    as_mat(result).setConstant(invalid_index<Index>);

    for (isize c = 0; c < tet_verts.size(); ++c)
    {
        auto const& c_v = tet_verts[c];
        Vec3<Index> const f_v[]{
            c_v({0, 1, 2}),
            c_v({1, 0, 3}),
            c_v({2, 3, 0}),
            c_v({3, 2, 1}),
        };

        auto& c_f = result[c];
        for (i8 i = 0; i < 4; ++i)
        {
            if (auto const it = verts_to_tri.find(f_v[i]); it != verts_to_tri.end())
                c_f[i] = it->second;
        }
    }
}

} // namespace dr