#pragma once

#include <cassert>

#include <dr/basic_types.hpp>
#include <dr/container_utils.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/memory.hpp>
#include <dr/mesh_elements.hpp>
#include <dr/sliced_array.hpp>
#include <dr/span.hpp>

namespace dr
{

struct HalfedgeMesh : AllocatorAware
{
    using Index = i32;

    struct Element
    {
        Index index{invalid_index_};

        constexpr Element() = default;

        constexpr explicit Element(Index const index) :
            index{index} {}

        constexpr operator Index() const { return index; }

        constexpr bool is_valid() const { return index != invalid_index_; }
    };

    struct Halfedge : public Element
    {
        using Element::Element;
    };

    struct Vertex : public Element
    {
        using Element::Element;
    };

    struct Edge : public Element
    {
        using Element::Element;
    };

    struct Face : public Element
    {
        using Element::Element;
    };

    struct Hole : public Element
    {
        using Element::Element;
    };

    /// Iterates over the outgoing halfedges around a vertex
    struct VertexCirculator
    {
        VertexCirculator(HalfedgeMesh const& mesh, Halfedge const start) :
            halfedge_next_{as_span(mesh.halfedge_next_)},
            start_{start.index},
            current_{start.index}
        {
        }

        // Returns the current halfedge
        Halfedge current() const
        {
            return Halfedge{current_};
        }

        // Returns true if the current halfedge is valid
        bool is_valid() const
        {
            return current_ != invalid_index_;
        }

        // Advances to the next halfedge around the vertex
        void advance()
        {
            Index const next = halfedge_next_[current_ ^ 1];
            current_ = (next == start_) ? invalid_index_ : next;
        }

        // Advances to the next halfedge around the vertex
        void operator++() { advance(); }

      private:
        Span<Index const> halfedge_next_;
        Index start_;
        Index current_;
    };

    /// Iterates over the halfedges within a face
    struct FaceCirculator
    {
        FaceCirculator(HalfedgeMesh const& mesh, Halfedge const start) :
            halfedge_next_{as_span(mesh.halfedge_next_)},
            start_{start.index},
            current_{start.index}
        {
        }

        // Returns the current halfedge
        Halfedge current() const
        {
            return Halfedge{current_};
        }

        // Returns true if the current halfedge is valid
        bool is_valid() const
        {
            return current_ != invalid_index_;
        }

        // Advances to the next halfedge in the face
        void advance()
        {
            Index const next = halfedge_next_[current_];
            current_ = (next == start_) ? invalid_index_ : next;
        }

        // Advances to the next halfedge in the face
        void operator++() { advance(); }

      private:
        Span<Index const> halfedge_next_;
        Index start_;
        Index current_;
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

        Builder(Allocator const alloc = {}) :
            v_to_he_(alloc)
        {
        }

        Builder(Builder const& other, Allocator const alloc = {}) :
            v_to_he_(other.v_to_he_, alloc)
        {
        }

        Builder(Builder&& other) noexcept = default;
        Builder& operator=(Builder const& other) = default;
        Builder& operator=(Builder&& other) noexcept = default;

        /// Returns the allocator used by this instance
        Allocator allocator() const
        {
            return v_to_he_.get_allocator();
        }

        static char const* error_message(Error err);

        template <typename SrcIndex>
        Error make_from_face_vertex(
            SlicedArray<SrcIndex> const& face_vertices,
            HalfedgeMesh& result,
            bool include_previous = true,
            bool include_holes = true);

      private:
        IncidenceMap<Index, 2> v_to_he_;
    };

    HalfedgeMesh(Allocator const alloc = {}) :
        halfedge_next_(alloc),
        halfedge_prev_(alloc),
        halfedge_vertex_(alloc),
        halfedge_face_(alloc),
        halfedge_hole_(alloc),
        vertex_halfedge_(alloc),
        face_halfedge_(alloc),
        hole_halfedge_(alloc)
    {
    }

    HalfedgeMesh(HalfedgeMesh const& other, Allocator const alloc = {}) :
        halfedge_next_(other.halfedge_next_, alloc),
        halfedge_prev_(other.halfedge_prev_, alloc),
        halfedge_vertex_(other.halfedge_vertex_, alloc),
        halfedge_face_(other.halfedge_face_, alloc),
        halfedge_hole_(other.halfedge_hole_, alloc),
        vertex_halfedge_(other.vertex_halfedge_, alloc),
        face_halfedge_(other.face_halfedge_, alloc),
        hole_halfedge_(other.hole_halfedge_, alloc)
    {
    }

    HalfedgeMesh(HalfedgeMesh&& other) noexcept = default;
    HalfedgeMesh& operator=(HalfedgeMesh const& other) = default;
    HalfedgeMesh& operator=(HalfedgeMesh&& other) noexcept = default;

    /// Returns the allocator used by this data structure
    Allocator allocator() const
    {
        return halfedge_next_.get_allocator();
    }

    /// Returns the oppositely oriented twin of the given halfedge
    static Halfedge twin(Halfedge const halfedge)
    {
        return Halfedge{(assert(halfedge.is_valid()), halfedge.index ^ 1)};
    }

    /// Returns the halfedge after the given halfedge
    Halfedge next(Halfedge const halfedge) const
    {
        return Halfedge{halfedge_next_[halfedge]};
    }

    /// Returns the halfedge before the given halfedge
    Halfedge previous(Halfedge const halfedge) const
    {
        return Halfedge{halfedge_prev_[halfedge]};
    }

    /// Returns the halfedge at the given index
    static Halfedge halfedge(Index const index) { return Halfedge{index}; }

    /// Returns the first outgoing halfedge at the given vertex. If the vertex is on the mesh
    /// boundary, then the returned halfedge will be as well.
    Halfedge halfedge(Vertex const vertex) const
    {
        return Halfedge{vertex_halfedge_[vertex]};
    }

    /// Returns the first halfedge of the given edge
    static Halfedge halfedge(Edge const edge)
    {
        return Halfedge{(assert(edge.is_valid()), edge << 1)};
    }

    /// Returns the first halfedge in the given face
    Halfedge halfedge(Face const face) const
    {
        return Halfedge{face_halfedge_[face]};
    }

    /// Returns the first halfedge in the given hole
    Halfedge halfedge(Hole const hole) const
    {
        return Halfedge{hole_halfedge_[hole]};
    }

    /// Returns the vertex at the given index
    static Vertex vertex(Index const index) { return Vertex{index}; }

    /// Returns the vertex at the start of the given halfegde
    Vertex vertex(Halfedge const halfedge) const
    {
        return Vertex{halfedge_vertex_[halfedge]};
    }

    /// Returns the edge at the given index
    static Edge edge(Index const index) { return Edge{index}; }

    /// Returns the edge incident to the given halfedge
    static Edge edge(Halfedge const halfedge)
    {
        return Edge{(assert(halfedge.is_valid()), halfedge >> 1)};
    }

    /// Returns the face at the given index
    static Face face(Index const index) { return Face{index}; }

    /// Returns the face incident to the given halfedge. If the halfedge is on the mesh boundary,
    /// the returned face will be invalid.
    Face face(Halfedge const halfedge) const
    {
        return Face{halfedge_face_[halfedge]};
    }

    /// Returns the hole at the given index
    static Hole hole(Index const index) { return Hole{index}; }

    /// Returns the hole incident to the given halfedge. If the halfedge isn't on the mesh boundary,
    /// the returned hole will be invalid.
    Hole hole(Halfedge const halfedge) const
    {
        return Hole{halfedge_hole_[halfedge]};
    }

    /// Returns the number of edges incident to the given vertex
    Index vertex_degree(Vertex const vertex) const
    {
        Index result = 0;

        for (auto itr = circulate_vertex(vertex); itr.is_valid(); ++itr)
            ++result;

        return result;
    }

    /// Returns the number of edges incident to the given face
    Index face_degree(Face const face) const
    {
        Index result = 0;

        for (auto itr = circulate_face(face); itr.is_valid(); ++itr)
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
    Index num_vertices() const
    {
        return size<Index>(vertex_halfedge_);
    }

    /// Returns the number of halfedges in the mesh
    Index num_halfedges() const
    {
        return size<Index>(halfedge_next_);
    }

    /// Returns the number of edges in the mesh
    Index num_edges() const
    {
        return num_halfedges() >> 1;
    }

    /// Returns the number of faces in the mesh
    Index num_faces() const
    {
        return size<Index>(face_halfedge_);
    }

    /// Returns the number of holes in the mesh
    Index num_holes() const
    {
        return size<Index>(hole_halfedge_);
    }

    /// Returns true if the given halfedge is on the mesh boundary
    bool is_boundary(Halfedge const halfedge) const
    {
        return halfedge_face_[halfedge] == invalid_index_;
    }

    // Returns true if the given vertex is on the mesh boundary
    bool is_boundary(Vertex const vertex) const
    {
        return is_boundary(halfedge(vertex));
    }

    /// Returns true if the given edge is on the mesh boundary
    bool is_boundary(Edge const edge) const
    {
        Halfedge const he = halfedge(edge);
        return is_boundary(he) || is_boundary(twin(he));
    }

    /// Returns true if this mesh is empty
    bool is_empty() const
    {
        return halfedge_next_.size() == 0;
    }

    /// Returns true if this mesh includes previous halfedges
    bool includes_previous() const
    {
        return halfedge_prev_.size() > 0;
    }

    /// Returns true if this mesh includes holes
    bool includes_holes() const
    {
        return hole_halfedge_.size() > 0;
    }

    /// Returns a circulator for the given vertex
    VertexCirculator circulate_vertex(Vertex const vertex) const
    {
        return {*this, halfedge(vertex)};
    }

    /// Returns a circulator for the vertex at the start of the given halfedge. When used,
    /// circulation will start from the given halfedge.
    VertexCirculator circulate_vertex(Halfedge const halfedge) const
    {
        return {*this, halfedge};
    }

    /// Returns a circulator for the given face
    FaceCirculator circulate_face(Face const face) const
    {
        return {*this, halfedge(face)};
    }

    /// Returns a circulator for the face containing the given halfedge. When used, circulation will
    /// start from the given halfedge.
    FaceCirculator circulate_face(const Halfedge halfedge) const
    {
        return {*this, halfedge};
    }

    /// Returns a circulator for the given hole
    HoleCirculator circulate_hole(Hole const hole) const
    {
        return {*this, halfedge(hole)};
    }

    /// Returns a circulator for the hole containing the given halfedge. When used, circulation will
    /// start from the given halfedge.
    HoleCirculator circulate_hole(const Halfedge halfedge) const
    {
        return {*this, halfedge};
    }

  private:
    static constexpr Index invalid_index_{-1};

    DynamicArray<Index> halfedge_next_;
    DynamicArray<Index> halfedge_prev_;
    DynamicArray<Index> halfedge_vertex_;
    DynamicArray<Index> halfedge_face_;
    DynamicArray<Index> halfedge_hole_;

    DynamicArray<Index> vertex_halfedge_;
    DynamicArray<Index> face_halfedge_;
    DynamicArray<Index> hole_halfedge_;
};

} // namespace dr
