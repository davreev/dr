#pragma once

#include <dr/span.hpp>

namespace dr
{

struct TriMeshPrims
{
    struct Tetrahedron
    {
        static Span<f32 const[3]> vertex_positions();
        static Span<i16 const[2]> edge_vertices();
        static Span<i16 const[3]> face_vertices();
        static Span<i16 const[3]> face_edges();
    };

    struct Octahedron
    {
        static Span<f32 const[3]> vertex_positions();
        static Span<i16 const[2]> edge_vertices();
        static Span<i16 const[3]> face_vertices();
        static Span<i16 const[3]> face_edges();
    };

    struct Icosahedron
    {
        static Span<f32 const[3]> vertex_positions();
        static Span<i16 const[2]> edge_vertices();
        static Span<i16 const[3]> face_vertices();
        static Span<i16 const[3]> face_edges();
    };

    struct Cube
    {
        static Span<f32 const[3]> vertex_positions();
        static Span<i16 const[2]> edge_vertices();
        static Span<i16 const[3]> face_vertices();
        static Span<i16 const[3]> face_edges();
    };
};

struct QuadMeshPrims
{
    struct Cube
    {
        static Span<f32 const[3]> vertex_positions();
        static Span<i16 const[2]> edge_vertices();
        static Span<i16 const[4]> face_vertices();
        static Span<i16 const[4]> face_edges();
    };
};

struct TetMeshPrims
{
    struct Cube
    {
        static Span<f32 const[3]> vertex_positions();
        static Span<i16 const[2]> edge_vertices();
        static Span<i16 const[3]> face_vertices();
        static Span<i16 const[3]> face_edges();
        static Span<i16 const[4]> cell_vertices();
        static Span<i16 const[6]> cell_edges();
        static Span<i16 const[4]> cell_faces();
    };
};

} // namespace dr