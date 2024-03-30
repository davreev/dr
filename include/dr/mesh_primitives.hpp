#pragma once

#include <dr/math_types.hpp>
#include <dr/span.hpp>

namespace dr
{

struct MeshPrimitives
{
    struct Tetrahedron
    {
        static Span<Vec3<f32> const> vert_coords();
        static Span<Vec3<i32> const> tri_verts();
    };

    struct Octahedron
    {
        static Span<Vec3<f32> const> vert_coords();
        static Span<Vec3<i32> const> tri_verts();
    };

    struct Cube
    {
        static Span<Vec3<f32> const> vert_coords();
        static Span<Vec3<i32> const> tri_verts();
        static Span<Vec4<i32> const> quad_verts();
        static Span<Vec4<i32> const> tet_verts();
    };

    struct Icosahedron
    {
        static Span<Vec3<f32> const> vert_coords();
        static Span<Vec3<i32> const> tri_verts();
    };
};

} // namespace dr