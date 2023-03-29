#pragma once

#include <dr/math_types.hpp>
#include <dr/span.hpp>

namespace dr
{

struct MeshTetrahedron
{
    static Span<Vec3<f32> const> vertex_positions();
    static Span<Vec3<i32> const> face_vertices();
};

struct MeshOctahedron
{
    static Span<Vec3<f32> const> vertex_positions();
    static Span<Vec3<i32> const> face_vertices();
};

struct MeshCube
{
    static Span<Vec3<f32> const> vertex_positions();
    static Span<Vec3<i32> const> face_vertices();
};

struct MeshIcosahedron
{
    static Span<Vec3<f32> const> vertex_positions();
    static Span<Vec3<i32> const> face_vertices();
};

}