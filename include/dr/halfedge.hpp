#pragma once

#include <cassert>

#include <dr/basic_types.hpp>
#include <dr/container_utils.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/hash_map.hpp>
#include <dr/math_types.hpp>
#include <dr/memory.hpp>
#include <dr/sliced_array.hpp>
#include <dr/span.hpp>

namespace dr
{

struct HalfedgeMesh : AllocatorAware
{
    using Index = i32;

    enum Tag : u8
    {
        Tag_Halfedge = 0,
        Tag_Vertex,
        Tag_Edge,
        Tag_Face,
        Tag_Hole,
    };

    template <Tag>
    struct Element
    {
        constexpr Element() = default;

        constexpr explicit Element(Index const index) : index{index} {}

        constexpr bool is_valid() const { return index != invalid_index_; }

        constexpr operator Index() const { return index; }

        constexpr Element& operator++() { return ++index, *this; }
        constexpr Element& operator--() { return --index, *this; }

        constexpr Element operator++(int) { return {index++}; }
        constexpr Element operator--(int) { return {index--}; }

        Index index{invalid_index_};
    };

    using Halfedge = Element<Tag_Halfedge>;
    using Vertex = Element<Tag_Vertex>;
    using Edge = Element<Tag_Edge>;
    using Face = Element<Tag_Face>;
    using Hole = Element<Tag_Hole>;

    /// Iterates over the outgoing halfedges around a vertex
    struct VertexCirculator
    {
        VertexCirculator(HalfedgeMesh const& mesh, Halfedge const start) :
            hedge_next_{as_span(mesh.hedge_next_)}, start_{start}, curr_{start}
        {
        }

        /// Returns the current halfedge
        Halfedge current() const { return Halfedge{curr_}; }

        /// Returns true if the current halfedge is valid
        bool is_valid() const { return curr_ != invalid_index_; }

        /// Advances to the next halfedge around the vertex
        void advance()
        {
            Index const next = hedge_next_[curr_ ^ 1];
            curr_ = (next == start_) ? invalid_index_ : next;
        }

        /// Advances to the next halfedge around the vertex
        void operator++() { advance(); }

      private:
        Span<Index const> hedge_next_;
        Index start_;
        Index curr_;
    };

    /// Iterates over the halfedges within a face
    struct FaceCirculator
    {
        FaceCirculator(HalfedgeMesh const& mesh, Halfedge const start) :
            hedge_next_{as_span(mesh.hedge_next_)}, start_{start}, curr_{start}
        {
        }

        /// Returns the current halfedge
        Halfedge current() const { return Halfedge{curr_}; }

        /// Returns true if the current halfedge is valid
        bool is_valid() const { return curr_ != invalid_index_; }

        /// Advances to the next halfedge in the face
        void advance()
        {
            Index const next = hedge_next_[curr_];
            curr_ = (next == start_) ? invalid_index_ : next;
        }

        /// Advances to the next halfedge in the face
        void operator++() { advance(); }

      private:
        Span<Index const> hedge_next_;
        Index start_;
        Index curr_;
    };

    /// Iterates over the halfedges within a hole
    using HoleCirculator = FaceCirculator;

    /// Creates halfedge meshes from other surface mesh representations
    struct Builder : AllocatorAware
    {
        enum Error : u8
        {
            Error_None = 0,
            Error_NonManifoldVertex,
            Error_NonManifoldEdge,
            Error_DegenerateEdge,
            Error_DegenerateFace,
            Error_DegenerateHole,
            _Error_Count
        };

        Builder(Allocator const alloc = {}) : v_to_he_(alloc) {}

        Builder(Builder const& other, Allocator const alloc = {}) : v_to_he_(other.v_to_he_, alloc)
        {
        }

        Builder(Builder&& other) noexcept = default;
        Builder& operator=(Builder const& other) = default;
        Builder& operator=(Builder&& other) noexcept = default;

        /// Returns the allocator used by this instance
        Allocator allocator() const { return v_to_he_.get_allocator(); }

        static char const* error_message(Error err);

        template <typename SrcIndex>
        Error make_from_face_vertex(
            SlicedArray<SrcIndex> const& face_vertices,
            HalfedgeMesh& result,
            bool include_previous = true,
            bool include_holes = true);

        template <typename SrcIndex, int size>
        Error make_from_face_vertex(
            Span<Vec<SrcIndex, size> const> const& face_vertices,
            HalfedgeMesh& result,
            bool include_previous = true,
            bool include_holes = true);

        template <typename SrcIndex, typename FaceVertices>
        Error make_from_face_vertex(
            FaceVertices&& face_vertices,
            Index num_faces,
            HalfedgeMesh& result,
            bool include_previous = true,
            bool include_holes = true);

      private:
        struct VerticesToHalfedge
        {
            struct Hash : HighQualityHash
            {
                usize operator()(Vec2<Index> const& key) const { return hash(as_bytes(key)); }
            };

            using Map = HashMap<Vec2<Index>, Index, Hash>;
        };

        VerticesToHalfedge::Map v_to_he_;
    };

    HalfedgeMesh(Allocator const alloc = {}) :
        hedge_next_(alloc),
        hedge_prev_(alloc),
        hedge_vert_(alloc),
        hedge_face_(alloc),
        hedge_hole_(alloc),
        vert_hedge_(alloc),
        face_hedge_(alloc),
        hole_hedge_(alloc)
    {
    }

    HalfedgeMesh(HalfedgeMesh const& other, Allocator const alloc = {}) :
        hedge_next_(other.hedge_next_, alloc),
        hedge_prev_(other.hedge_prev_, alloc),
        hedge_vert_(other.hedge_vert_, alloc),
        hedge_face_(other.hedge_face_, alloc),
        hedge_hole_(other.hedge_hole_, alloc),
        vert_hedge_(other.vert_hedge_, alloc),
        face_hedge_(other.face_hedge_, alloc),
        hole_hedge_(other.hole_hedge_, alloc)
    {
    }

    HalfedgeMesh(HalfedgeMesh&& other) noexcept = default;
    HalfedgeMesh& operator=(HalfedgeMesh const& other) = default;
    HalfedgeMesh& operator=(HalfedgeMesh&& other) noexcept = default;

    /// Returns the allocator used by this data structure
    Allocator allocator() const { return hedge_next_.get_allocator(); }

    /// Returns the oppositely oriented twin of the given halfedge
    static Halfedge twin(Halfedge const he)
    {
        return Halfedge{(assert(he.is_valid()), he.index ^ 1)};
    }

    /// Returns the halfedge after the given halfedge
    Halfedge next(Halfedge const he) const { return Halfedge{hedge_next_[he]}; }

    /// Returns the halfedge before the given halfedge
    Halfedge previous(Halfedge const he) const { return Halfedge{hedge_prev_[he]}; }

    /// Returns the halfedge at the given index
    static Halfedge halfedge(Index const i) { return Halfedge{i}; }

    /// Returns the first outgoing halfedge at the given vertex. If the vertex is on the mesh
    /// boundary, then the returned halfedge will be as well.
    Halfedge halfedge(Vertex const e) const { return Halfedge{vert_hedge_[e]}; }

    /// Returns the first halfedge of the given edge
    static Halfedge halfedge(Edge const e)
    {
        return Halfedge{(assert(e.is_valid()), e << 1)};
    }

    /// Returns the first halfedge in the given face
    Halfedge halfedge(Face const f) const { return Halfedge{face_hedge_[f]}; }

    /// Returns the first halfedge in the given hole
    Halfedge halfedge(Hole const h) const { return Halfedge{hole_hedge_[h]}; }

    /// Returns the vertex at the given index
    static Vertex vertex(Index const i) { return Vertex{i}; }

    /// Returns the vertex at the start of the given halfegde
    Vertex vertex(Halfedge const he) const { return Vertex{hedge_vert_[he]}; }

    /// Returns the edge at the given index
    static Edge edge(Index const i) { return Edge{i}; }

    /// Returns the edge incident to the given halfedge
    static Edge edge(Halfedge const he)
    {
        return Edge{(assert(he.is_valid()), he >> 1)};
    }

    /// Returns the face at the given index
    static Face face(Index const i) { return Face{i}; }

    /// Returns the face incident to the given halfedge. If the halfedge is on the mesh boundary,
    /// the returned face will be invalid.
    Face face(Halfedge const he) const { return Face{hedge_face_[he]}; }

    /// Returns the hole at the given index
    static Hole hole(Index const i) { return Hole{i}; }

    /// Returns the hole incident to the given halfedge. If the halfedge isn't on the mesh boundary,
    /// the returned hole will be invalid.
    Hole hole(Halfedge const he) const { return Hole{hedge_hole_[he]}; }

    /// Returns the number of edges incident to the given vertex
    Index vertex_degree(Vertex const v) const
    {
        Index result = 0;

        for (auto itr = circulate_vertex(v); itr.is_valid(); ++itr)
            ++result;

        return result;
    }

    /// Returns the number of edges incident to the given face
    Index face_degree(Face const f) const
    {
        Index result = 0;

        for (auto itr = circulate_face(f); itr.is_valid(); ++itr)
            ++result;

        return result;
    }

    /// Returns the halfedge between a pair of vertices. If the given vertices aren't adjacent, the
    /// returned halfedge will be invalid.
    Halfedge find_halfedge(Vertex const v0, Vertex const v1) const
    {
        for (auto itr = circulate_vertex(v0); itr.is_valid(); ++itr)
        {
            Halfedge const he = itr.current();
            if (vertex(twin(he)) == v1)
                return he;
        }

        return {};
    }

    /// Returns the halfedge between a pair of faces. If the given faces aren't adjacent, the
    /// returned halfedge will be invalid.
    Halfedge find_halfedge(Face const f0, Face const f1) const
    {
        for (auto itr = circulate_face(f0); itr.is_valid(); ++itr)
        {
            const Halfedge he = itr.current();
            if (face(twin(he)) == f1)
                return he;
        }

        return {};
    }

    /// Returns the number of vertices in the mesh
    Index num_vertices() const { return size_as<Index>(vert_hedge_); }

    /// Returns the number of halfedges in the mesh
    Index num_halfedges() const { return size_as<Index>(hedge_next_); }

    /// Returns the number of edges in the mesh
    Index num_edges() const { return num_halfedges() >> 1; }

    /// Returns the number of faces in the mesh
    Index num_faces() const { return size_as<Index>(face_hedge_); }

    /// Returns the number of holes in the mesh
    Index num_holes() const { return size_as<Index>(hole_hedge_); }

    /// Returns true if the given halfedge is on the mesh boundary
    bool is_boundary(Halfedge const he) const
    {
        return hedge_face_[he] == invalid_index_;
    }

    // Returns true if the given vertex is on the mesh boundary
    bool is_boundary(Vertex const v) const { return is_boundary(halfedge(v)); }

    /// Returns true if the given edge is on the mesh boundary
    bool is_boundary(Edge const e) const
    {
        Halfedge const he = halfedge(e);
        return is_boundary(he) || is_boundary(twin(he));
    }

    /// Returns true if this mesh is empty
    bool is_empty() const { return hedge_next_.size() == 0; }

    /// Returns true if this mesh includes previous halfedges
    bool includes_previous() const { return hedge_prev_.size() > 0; }

    /// Returns true if this mesh includes holes
    bool includes_holes() const { return hole_hedge_.size() > 0; }

    /// Returns a circulator for the given vertex
    VertexCirculator circulate_vertex(Vertex const v) const
    {
        return {*this, halfedge(v)};
    }

    /// Returns a circulator for the vertex at the start of the given halfedge. When used,
    /// circulation will start from the given halfedge.
    VertexCirculator circulate_vertex(Halfedge const he) const { return {*this, he}; }

    /// Returns a circulator for the given face
    FaceCirculator circulate_face(Face const f) const { return {*this, halfedge(f)}; }

    /// Returns a circulator for the face containing the given halfedge. When used, circulation will
    /// start from the given halfedge.
    FaceCirculator circulate_face(Halfedge const he) const { return {*this, he}; }

    /// Returns a circulator for the given hole
    HoleCirculator circulate_hole(Hole const h) const { return {*this, halfedge(h)}; }

    /// Returns a circulator for the hole containing the given halfedge. When used, circulation will
    /// start from the given halfedge.
    HoleCirculator circulate_hole(Halfedge const he) const { return {*this, he}; }

  private:
    static constexpr Index invalid_index_{-1};

    DynamicArray<Index> hedge_next_;
    DynamicArray<Index> hedge_prev_;
    DynamicArray<Index> hedge_vert_;
    DynamicArray<Index> hedge_face_;
    DynamicArray<Index> hedge_hole_;

    DynamicArray<Index> vert_hedge_;
    DynamicArray<Index> face_hedge_;
    DynamicArray<Index> hole_hedge_;
};

} // namespace dr
