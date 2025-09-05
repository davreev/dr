#pragma once

#include <dr/span.hpp>

namespace dr
{

struct MeshPrimitives
{
    struct Tri
    {
        struct Mesh
        {
            Span<f32 const[3]> vertex_positions;
            Span<i16 const[2]> edge_vertices;
            Span<i16 const[3]> face_vertices;
            Span<i16 const[3]> face_edges;
        };

        static Mesh const& tetrahedron();
        static Mesh const& octahedron();
        static Mesh const& icosahedron();
        static Mesh const& cube();
        // ...
        // ...
        // ...
    };

    struct Quad
    {
        struct Mesh
        {
            Span<f32 const[3]> vertex_positions;
            Span<i16 const[2]> edge_vertices;
            Span<i16 const[4]> face_vertices;
            Span<i16 const[4]> face_edges;
        };

        static Mesh const& cube();
        // ...
        // ...
        // ...
    };

    struct Tet
    {
        struct Mesh
        {
            Span<f32 const[3]> vertex_positions;
            Span<i16 const[2]> edge_vertices;
            Span<i16 const[3]> face_vertices;
            Span<i16 const[3]> face_edges;
            Span<i16 const[4]> cell_vertices;
            Span<i16 const[6]> cell_edges;
            Span<i16 const[4]> cell_faces;
        };

        static Mesh const& cube();
        // ...
        // ...
        // ...
    };

    struct Hex
    {
        struct Mesh
        {
            Span<f32 const[3]> vertex_positions;
            Span<i16 const[2]> edge_vertices;
            Span<i16 const[4]> face_vertices;
            Span<i16 const[4]> face_edges;
            Span<i16 const[8]> cell_vertices;
            Span<i16 const[12]> cell_edges;
            Span<i16 const[6]> cell_faces;
        };

        // ...
        // ...
        // ...
    };
};

} // namespace dr