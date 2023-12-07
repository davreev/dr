#include <dr/mesh_primitives.hpp>

#include <dr/container_utils.hpp>

namespace dr
{

Span<Vec3<f32> const> MeshTetrahedron::vertex_positions()
{
    static Vec3<f32> const result[] = {
        {-0.5, -0.5, -0.5},
        {0.5, 0.5, -0.5},
        {0.5, -0.5, 0.5},
        {-0.5, 0.5, 0.5},
    };

    return as_span(result);
}

Span<Vec3<i32> const> MeshTetrahedron::face_vertices()
{
    static Vec3<i32> const result[] = {
        {0, 1, 2},
        {1, 0, 3},
        {2, 3, 0},
        {3, 2, 1},
    };

    return as_span(result);
}

Span<Vec3<f32> const> MeshOctahedron::vertex_positions()
{
    static Vec3<f32> const result[] = {
        {-0.5, -0.5, 0.0},
        {0.5, -0.5, 0.0},
        {-0.5, 0.5, 0.0},
        {0.5, 0.5, 0.0},
        {0.0, 0.0, -0.5},
        {0.0, 0.0, 0.5},
    };

    return as_span(result);
}

Span<Vec3<i32> const> MeshOctahedron::face_vertices()
{
    static Vec3<i32> const result[] = {
        {0, 1, 5},
        {1, 3, 5},
        {3, 2, 5},
        {2, 0, 5},
        {1, 0, 4},
        {3, 1, 4},
        {2, 3, 4},
        {0, 2, 4},
    };

    return as_span(result);
}

Span<Vec3<f32> const> MeshCube::vertex_positions()
{
    static Vec3<f32> const result[8] = {
        {-0.5, -0.5, -0.5},
        {0.5, -0.5, -0.5},
        {-0.5, 0.5, -0.5},
        {0.5, 0.5, -0.5},
        {-0.5, -0.5, 0.5},
        {0.5, -0.5, 0.5},
        {-0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5},
    };

    return as_span(result);
}

Span<Vec3<i32> const> MeshCube::face_vertices()
{
    static Vec3<i32> const result[12] = {
        {0, 1, 4},
        {5, 4, 1},
        {3, 2, 7},
        {6, 7, 2},
        {2, 0, 6},
        {4, 6, 0},
        {1, 3, 5},
        {7, 5, 3},
        {0, 2, 1},
        {3, 1, 2},
        {7, 6, 5},
        {4, 5, 6},
    };

    return as_span(result);
}

Span<Vec4<i32> const> MeshCube::cell_vertices()
{
    static Vec4<i32> const result[5] = {
        {0, 3, 5, 6},
        {0, 1, 5, 3},
        {3, 2, 6, 0},
        {4, 6, 0, 5},
        {7, 5, 3, 6},
    };

    return as_span(result);
}

Span<Vec3<f32> const> MeshIcosahedron::vertex_positions()
{
    static constexpr f64 x = 0.525731112119133606;
    static constexpr f64 z = 0.850650808352039932;

    static Vec3<f32> const result[] = {
        {-x, 0.0, z},
        {x, 0.0, z},
        {-x, 0.0, -z},
        {x, 0.0, -z},
        {0.0, z, x},
        {0.0, z, -x},
        {0.0, -z, x},
        {0.0, -z, -x},
        {z, x, 0.0},
        {-z, x, 0.0},
        {z, -x, 0.0},
        {-z, -x, 0.0},
    };

    return as_span(result);
}

Span<Vec3<i32> const> MeshIcosahedron::face_vertices()
{
    static Vec3<i32> const result[] = {
        {1, 4, 0},
        {4, 9, 0},
        {4, 5, 9},
        {8, 5, 4},
        {1, 8, 4},
        {1, 10, 8},
        {10, 3, 8},
        {8, 3, 5},
        {3, 2, 5},
        {3, 7, 2},
        {3, 10, 7},
        {10, 6, 7},
        {6, 11, 7},
        {6, 0, 11},
        {6, 1, 0},
        {10, 1, 6},
        {11, 0, 9},
        {2, 11, 9},
        {5, 2, 9},
        {11, 2, 7},
    };

    return as_span(result);
}

} // namespace dr