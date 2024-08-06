#include <dr/halfedge.hpp>

#include <dr/linalg_reshape.hpp>
#include <dr/math_traits.hpp>
#include <dr/meta.hpp>

namespace dr
{
namespace
{

using Index = HalfedgeMesh::Index;
using Builder = HalfedgeMesh::Builder;

using SupportedIndexTypes = TypePack<i16, i32, i64, u16, u32, u64>;
using SupportedFaceSizes = ValuePack<int, 3, 4>;

} // namespace

char const* Builder::error_message(Builder::Error const err)
{
    static constexpr char const* messages[]{
        "",
        "The surface contains one or more non-manifold vertices",
        "The surface contains one or more non-manifold edges",
        "The surface contains one or more degenerate edges",
        "The surface contains one or more degenerate faces",
        "The surface contains one or more degenerate holes",
    };
    static_assert(size(messages) == Builder::_Error_Count);

    assert(err < Builder::_Error_Count);
    return messages[err];
}

template <typename SrcIndex>
Builder::Error Builder::make_from_face_vertex(
    SlicedArray<SrcIndex> const& face_vertices,
    HalfedgeMesh& result,
    bool const include_previous,
    bool const include_holes)
{
    static_assert(SupportedIndexTypes::includes<SrcIndex>);

    struct FaceVertexSrc
    {
        SlicedArray<SrcIndex> const* src;
        Index size() const { return src->num_slices(); }
        Span<SrcIndex const> operator[](Index const face) const { return (*src)[face]; };
    };

    return make_from_face_vertex_impl<SrcIndex>(
        FaceVertexSrc{&face_vertices},
        result,
        include_previous,
        include_holes);
}

template <typename SrcIndex, int n>
Builder::Error Builder::make_from_face_vertex(
    Span<Vec<SrcIndex, n> const> const& face_vertices,
    HalfedgeMesh& result,
    bool const include_previous,
    bool const include_holes)
{
    static_assert(SupportedIndexTypes::includes<SrcIndex>);
    static_assert(SupportedFaceSizes::includes<n>);

    struct FaceVertexSrc
    {
        Span<Vec<SrcIndex, n> const> src;
        Index size() const { return src.size(); }
        Span<SrcIndex const> operator[](Index const face) const { return as_span(src[face]); }
    };

    return make_from_face_vertex_impl<SrcIndex>(
        FaceVertexSrc{face_vertices},
        result,
        include_previous,
        include_holes);
}

template <typename SrcIndex, typename FaceVertexSrc>
Builder::Error Builder::make_from_face_vertex_impl(
    FaceVertexSrc&& face_vertices,
    HalfedgeMesh& result,
    bool const include_previous,
    bool const include_holes)
{
    static_assert(is_integer<SrcIndex> || is_natural<SrcIndex>);

    v_to_he_.clear();
    Index const num_faces = face_vertices.size();
    Index num_hedges = 0;
    Index num_verts = 0;

    // Create map from vertex pairs to halfedges
    for (Index f = 0; f < num_faces; ++f)
    {
        Span<SrcIndex const> const f_v = face_vertices[f];

        if (f_v.size() < 3)
            return Error_DegenerateFace;

        for (Index i = 0, n = f_v.size(); i < n; ++i)
        {
            auto const v0 = static_cast<Index>(f_v[i]);
            auto const v1 = static_cast<Index>(f_v[(i + 1) % n]);

            if (v0 == v1)
                return Error_DegenerateEdge;

            // Try inserting a halfedge between the vertex pair. If one already exists, then we have
            // a non-manifold edge.
            auto const [it0, ok] = v_to_he_.try_emplace({v0, v1}, invalid_index_);
            if (!ok)
                return Error_NonManifoldEdge;

            // Infer the index of the new halfedge if its twin already exists. Otherwise, treat it
            // as the first of a new pair.
            auto const it1 = v_to_he_.find({v1, v0});
            if (it1 == v_to_he_.end())
            {
                it0->second = num_hedges;
                num_hedges += 2;
            }
            else
            {
                it0->second = it1->second + 1;
            }

            // Take vertex count from the largest index
            if (v0 >= num_verts)
                num_verts = v0 + 1;
        }
    }

    auto& he_next = result.halfedge_next_;
    he_next.assign(num_hedges, invalid_index_);

    auto& he_vert = result.halfedge_vertex_;
    he_vert.assign(num_hedges, invalid_index_);

    auto& he_face = result.halfedge_face_;
    he_face.assign(num_hedges, invalid_index_);

    auto& v_hedge = result.vertex_halfedge_;
    v_hedge.assign(num_verts, invalid_index_);

    auto& f_hedge = result.face_halfedge_;
    f_hedge.assign(num_faces, invalid_index_);

    // Connect faces to halfedges and halfedges to vertices, faces, and next
    for (Index f = 0; f < num_faces; ++f)
    {
        Span<SrcIndex const> const f_v = face_vertices[f];

        auto v0 = static_cast<Index>(f_v[0]);
        auto v1 = static_cast<Index>(f_v[1]);
        Index he0 = v_to_he_[{v0, v1}];

        // Assign first halfedge in face
        f_hedge[f] = he0;

        for (Index i = 0, n = f_v.size(); i < n; ++i)
        {
            // If the twin halfedge is already in the face, then we have a (partially) degenerate
            // face
            if (he_face[he0 ^ 1] == f)
                return Error_DegenerateFace;

            // Assign halfedge vertex and face
            he_vert[he0] = v0;
            he_face[he0] = f;

            // Assign next halfedge
            auto const v2 = static_cast<Index>(f_v[(i + 2) % n]);
            Index const he1 = v_to_he_[{v1, v2}];
            he_next[he0] = he1;

            v0 = v1;
            v1 = v2;
            he0 = he1;
        }
    }

    // Connect vertices to halfedges and boundary halfedges to vertices and next
    for (Index he0 = 0; he0 < num_hedges; ++he0)
    {
        if (he_face[he0] == invalid_index_)
        {
            Index he1 = he0 ^ 1;
            Index he2 = he_next[he1];

            Index const v = he_vert[he2];
            assert(v != invalid_index_);

            // Connect vertex and boundary halfedge
            he_vert[he0] = v;
            v_hedge[v] = he0;

            // Circulate vertex to next boundary halfedge
            do
            {
                he1 = he2 ^ 1;
                he2 = he_next[he1];
            } while (he2 != invalid_index_);

            // Connect boundary halfedges
            he_next[he1] = he0;
        }
        else
        {
            Index const v = he_vert[he0];
            assert(v != invalid_index_);

            // Assign vertex halfedge if not already assigned
            if (auto& v_he = v_hedge[v]; v_he == invalid_index_)
                v_he = he0;
        }
    }

    // Optionally connect halfedges to previous halfedges
    if (include_previous)
    {
        auto& he_prev = result.halfedge_prev_;
        he_prev.assign(num_hedges, invalid_index_);

        for (Index he = 0; he < num_hedges; ++he)
            he_prev[he_next[he]] = he;
    }
    else
    {
        result.halfedge_prev_.clear();
    }

    // Optionally discover holes and connect them to/from halfedges
    if (include_holes)
    {
        auto& he_hole = result.halfedge_hole_;
        he_hole.assign(num_hedges, invalid_index_);

        auto& h_hedge = result.hole_halfedge_;
        h_hedge.clear();

        Index num_holes = 0;

        for (Index he0 = 0; he0 < num_hedges; ++he0)
        {
            // If the halfedge is on the boundary but has not yet been assigned a hole, then
            // we've discovered a new hole
            if (he_face[he0] == invalid_index_ && he_hole[he0] == invalid_index_)
            {
                // Circulate halfedges in hole and assign
                Index he1 = he0;
                Index n = 0;
                do
                {
                    he_hole[he1] = num_holes;
                    he1 = he_next[he1];
                    ++n;
                } while (he1 != he0);

                if (n < 3)
                    return Error_DegenerateHole;

                // Assign first halfedge to hole
                h_hedge.push_back(he0);
                ++num_holes;
            }
        }
    }
    else
    {
        result.halfedge_hole_.clear();
        result.hole_halfedge_.clear();
    }

    // Check for non-manifold vertices
    {
        Index he_count = 0;

        // Count halfedges by circulating vertices. If the result is inconsistent, then we have one
        // or more non-manifold vertices.
        for (Index v = 0; v < num_verts; ++v)
        {
            Index he0 = v_hedge[v];
            if (he0 != invalid_index_)
            {
                Index he1 = he0;
                do
                {
                    ++he_count;
                    he1 = he_next[he1 ^ 1];
                } while (he1 != he0);
            }
        }

        if (he_count != num_hedges)
            return Error_NonManifoldVertex;
    }

    return Error_None;
}

// Explicit template instantiation

#define DR_INSTANTIATE(SrcIndex)                                                                   \
    template Builder::Error Builder::make_from_face_vertex(                                        \
        SlicedArray<SrcIndex> const& face_vertices,                                                \
        HalfedgeMesh& result,                                                                      \
        bool include_previous,                                                                     \
        bool include_holes);

DR_INSTANTIATE(i16)
DR_INSTANTIATE(i32)
DR_INSTANTIATE(i64)

DR_INSTANTIATE(u16)
DR_INSTANTIATE(u32)
DR_INSTANTIATE(u64)

#undef DR_INSTANTIATE

#define DR_INSTANTIATE(SrcIndex, size)                                                             \
    template Builder::Error Builder::make_from_face_vertex(                                        \
        Span<Vec<SrcIndex, size> const> const& face_vertices,                                      \
        HalfedgeMesh& result,                                                                      \
        bool include_previous,                                                                     \
        bool include_holes);

DR_INSTANTIATE(i16, 3)
DR_INSTANTIATE(i32, 3)
DR_INSTANTIATE(i64, 3)

DR_INSTANTIATE(u16, 3)
DR_INSTANTIATE(u32, 3)
DR_INSTANTIATE(u64, 3)

DR_INSTANTIATE(i16, 4)
DR_INSTANTIATE(i32, 4)
DR_INSTANTIATE(i64, 4)

DR_INSTANTIATE(u16, 4)
DR_INSTANTIATE(u32, 4)
DR_INSTANTIATE(u64, 4)

#undef DR_INSTANTIATE

} // namespace dr