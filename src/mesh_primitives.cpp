#include <dr/mesh_primitives.hpp>

namespace dr
{

MeshPrimitives::Tri::Mesh const& MeshPrimitives::Tri::tetrahedron()
{
    static constexpr f32 vert_positions[][3]{
        {-0.5, -0.5, -0.5},
        {0.5, 0.5, -0.5},
        {0.5, -0.5, 0.5},
        {-0.5, 0.5, 0.5},
    };

    static constexpr i16 edge_verts[][2]{
        {0, 1},
        {1, 0},
        {1, 2},
        {2, 1},
        {2, 0},
        {0, 2},
        {0, 3},
        {3, 0},
        {3, 1},
        {1, 3},
        {2, 3},
        {3, 2},
    };

    static constexpr i16 face_verts[][3]{
        {0, 1, 2},
        {1, 0, 3},
        {2, 3, 0},
        {3, 2, 1},
    };

    static constexpr i16 face_edges[][3]{
        {0, 2, 4},
        {1, 6, 8},
        {10, 7, 5},
        {11, 3, 9},
    };

    static constexpr Mesh mesh{
        vert_positions,
        edge_verts,
        face_verts,
        face_edges,
    };

    return mesh;
}

MeshPrimitives::Tri::Mesh const& MeshPrimitives::Tri::octahedron()
{
    static constexpr f32 vert_positions[][3]{
        {-0.5, -0.5, 0.0},
        {0.5, -0.5, 0.0},
        {-0.5, 0.5, 0.0},
        {0.5, 0.5, 0.0},
        {0.0, 0.0, -0.5},
        {0.0, 0.0, 0.5},
    };

    // clang-format off
    static constexpr i16 edge_verts[][2]{
        {0, 1},
        {1, 0},
        {1, 5},
        {5, 1},
        {5, 0},
        {0, 5},
        {1, 3},
        {3, 1},
        {3, 5},
        {5, 3},
        {3, 2},
        {2, 3},
        {2, 5},
        {5, 2},
        {2, 0},
        {0, 2},
        {0, 4},
        {4, 0},
        {4, 1},
        {1, 4},
        {4, 3},
        {3, 4},
        {4, 2},
        {2, 4},
    };
    // clang-format on

    static constexpr i16 face_verts[][3]{
        {0, 1, 5},
        {1, 3, 5},
        {3, 2, 5},
        {2, 0, 5},
        {1, 0, 4},
        {3, 1, 4},
        {2, 3, 4},
        {0, 2, 4},
    };

    static constexpr i16 face_edges[][3]{
        {0, 2, 4},
        {6, 8, 3},
        {10, 12, 9},
        {14, 5, 13},
        {1, 16, 18},
        {7, 19, 20},
        {11, 21, 22},
        {15, 23, 17},
    };

    static constexpr Mesh mesh{
        vert_positions,
        edge_verts,
        face_verts,
        face_edges,
    };

    return mesh;
}

MeshPrimitives::Tri::Mesh const& MeshPrimitives::Tri::icosahedron()
{
    constexpr f64 x = 0.525731112119133606;
    constexpr f64 z = 0.850650808352039932;

    static constexpr f32 vert_positions[][3] = {
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

    // clang-format off
    static constexpr i16 edge_verts[][2]{
        {1, 4},
        {4, 1},
        {4, 0},
        {0, 4},
        {0, 1},
        {1, 0},
        {4, 9},
        {9, 4},
        {9, 0},
        {0, 9},
        {4, 5},
        {5, 4},
        {5, 9},
        {9, 5},
        {8, 5},
        {5, 8},
        {4, 8},
        {8, 4},
        {1, 8},
        {8, 1},
        {1, 10},
        {10, 1},
        {10, 8},
        {8, 10},
        {10, 3},
        {3, 10},
        {3, 8},
        {8, 3},
        {3, 5},
        {5, 3},
        {3, 2},
        {2, 3},
        {2, 5},
        {5, 2},
        {3, 7},
        {7, 3},
        {7, 2},
        {2, 7},
        {10, 7},
        {7, 10},
        {10, 6},
        {6, 10},
        {6, 7},
        {7, 6},
        {6, 11},
        {11, 6},
        {11, 7},
        {7, 11},
        {6, 0},
        {0, 6},
        {0, 11},
        {11, 0},
        {6, 1},
        {1, 6},
        {9, 11},
        {11, 9},
        {2, 11},
        {11, 2},
        {9, 2},
        {2, 9},
    };
    // clang-format on

    // clang-format off
    static constexpr i16 face_verts[][3] = {
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
    // clang-format on

    // clang-format off
    static constexpr i16 face_edges[][3] = {
        {0, 2, 4},
        {6, 8, 3},
        {10, 12, 7},
        {14, 11, 16},
        {18, 17, 1},
        {20, 22, 19},
        {24, 26, 23},
        {27, 28, 15},
        {30, 32, 29},
        {34, 36, 31},
        {25, 38, 35},
        {40, 42, 39},
        {44, 46, 43},
        {48, 50, 45},
        {52, 5, 49},
        {21, 53, 41},
        {51, 9, 54},
        {56, 55, 58},
        {33, 59, 13},
        {57, 37, 47},
    };
    // clang-format on

    static constexpr Mesh mesh{
        vert_positions,
        edge_verts,
        face_verts,
        face_edges,
    };

    return mesh;
}

MeshPrimitives::Tri::Mesh const& MeshPrimitives::Tri::cube()
{
    static constexpr f32 vert_positions[][3] = {
        {-0.5, -0.5, -0.5},
        {0.5, -0.5, -0.5},
        {-0.5, 0.5, -0.5},
        {0.5, 0.5, -0.5},
        {-0.5, -0.5, 0.5},
        {0.5, -0.5, 0.5},
        {-0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5},
    };

    // clang-format off
    static constexpr i16 edge_verts[][2]{
        {0, 1},
        {1, 0},
        {1, 4},
        {4, 1},
        {4, 0},
        {0, 4},
        {5, 4},
        {4, 5},
        {1, 5},
        {5, 1},
        {3, 2},
        {2, 3},
        {2, 7},
        {7, 2},
        {7, 3},
        {3, 7},
        {6, 7},
        {7, 6},
        {2, 6},
        {6, 2},
        {2, 0},
        {0, 2},
        {0, 6},
        {6, 0},
        {4, 6},
        {6, 4},
        {1, 3},
        {3, 1},
        {3, 5},
        {5, 3},
        {7, 5},
        {5, 7},
        {2, 1},
        {1, 2},
        {6, 5},
        {5, 6},
    };
    // clang-format on

    static constexpr i16 face_verts[][3]{
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

    static constexpr i16 face_edges[][3]{
        {0, 2, 4},
        {6, 3, 8},
        {10, 12, 14},
        {16, 13, 18},
        {20, 22, 19},
        {24, 23, 5},
        {26, 28, 9},
        {30, 29, 15},
        {21, 32, 1},
        {27, 33, 11},
        {17, 34, 31},
        {7, 35, 25},
    };

    static constexpr Mesh mesh{
        vert_positions,
        edge_verts,
        face_verts,
        face_edges,
    };

    return mesh;
}

MeshPrimitives::Quad::Mesh const& MeshPrimitives::Quad::cube()
{
    // clang-format off
    static constexpr i16 edge_verts[][2]{
        {0, 1},
        {1, 0},
        {1, 5},
        {5, 1},
        {5, 4},
        {4, 5},
        {4, 0},
        {0, 4},
        {3, 2},
        {2, 3},
        {2, 6},
        {6, 2},
        {6, 7},
        {7, 6},
        {7, 3},
        {3, 7},
        {2, 0},
        {0, 2},
        {4, 6},
        {6, 4},
        {1, 3},
        {3, 1},
        {7, 5},
        {5, 7},
    };
    // clang-format on

    static constexpr i16 face_verts[][4] = {
        {0, 1, 5, 4},
        {3, 2, 6, 7},
        {2, 0, 4, 6},
        {1, 3, 7, 5},
        {0, 2, 3, 1},
        {7, 6, 4, 5},
    };

    static constexpr i16 face_edges[][4] = {
        {0, 2, 4, 6},
        {8, 10, 12, 14},
        {16, 7, 18, 11},
        {20, 15, 22, 3},
        {17, 9, 21, 1},
        {13, 19, 5, 23},
    };

    static Mesh const mesh{
        Tri::cube().vertex_positions,
        edge_verts,
        face_verts,
        face_edges,
    };

    return mesh;
}

MeshPrimitives::Tet::Mesh const& MeshPrimitives::Tet::cube()
{
    // clang-format off
    static constexpr i16 edge_verts[][2]{
        {0, 3},
        {3, 0},
        {3, 5},
        {5, 3},
        {5, 0},
        {0, 5},
        {0, 6},
        {6, 0},
        {6, 3},
        {3, 6},
        {5, 6},
        {6, 5},
        {0, 1},
        {1, 0},
        {1, 5},
        {5, 1},
        {3, 1},
        {1, 3},
        {2, 6},
        {6, 2},
        {3, 2},
        {2, 3},
        {0, 2},
        {2, 0},
        {0, 4},
        {4, 0},
        {4, 6},
        {6, 4},
        {4, 5},
        {5, 4},
        {3, 7},
        {7, 3},
        {7, 5},
        {5, 7},
        {7, 6},
        {6, 7},
    };
    // clang-format on

    // clang-format off
    static constexpr i16 face_verts[][3]{
        {0, 3, 5},
        {0, 5, 3},
        {0, 6, 3},
        {0, 3, 6},
        {0, 5, 6},
        {0, 6, 5},
        {3, 6, 5},
        {3, 5, 6},
        {0, 1, 5},
        {0, 5, 1},
        {0, 3, 1},
        {0, 1, 3},
        {1, 3, 5},
        {1, 5, 3},
        {2, 6, 3},
        {2, 3, 6},
        {0, 2, 3},
        {0, 3, 2},
        {0, 6, 2},
        {0, 2, 6},
        {0, 4, 6},
        {0, 6, 4},
        {4, 5, 6},
        {4, 6, 5},
        {0, 5, 4},
        {0, 4, 5},
        {3, 7, 5},
        {3, 5, 7},
        {5, 7, 6},
        {5, 6, 7},
        {3, 6, 7},
        {3, 7, 6},
    };
    // clang-format on

    // clang-format off
    static constexpr i16 face_edges[][3]{
        {0, 2, 4},
        {5, 3, 1},
        {6, 8, 1},
        {0, 9, 7},
        {5, 10, 7},
        {6, 11, 4},
        {9, 11, 3},
        {2, 10, 8},
        {12, 14, 4},
        {5, 15, 13},
        {0, 16, 13},
        {12, 17, 1},
        {17, 2, 15},
        {14, 3, 16},
        {18, 8, 20},
        {21, 9, 19},
        {22, 21, 1},
        {0, 20, 23},
        {6, 19, 23},
        {22, 18, 7},
        {24, 26, 7},
        {6, 27, 25},
        {28, 10, 27},
        {26, 11, 29},
        {5, 29, 25},
        {24, 28, 4},
        {30, 32, 3},
        {2, 33, 31},
        {33, 34, 11},
        {10, 35, 32},
        {9, 35, 31},
        {30, 34, 8},
    };
    // clang-format on

    static constexpr i16 cell_verts[][4] = {
        {0, 3, 5, 6},
        {0, 1, 5, 3},
        {3, 2, 6, 0},
        {4, 6, 0, 5},
        {7, 5, 3, 6},
    };

    static constexpr i16 cell_edges[][6] = {
        {0, 2, 4, 6, 8, 10},
        {12, 14, 4, 0, 16, 3},
        {20, 18, 8, 1, 22, 7},
        {26, 7, 24, 28, 10, 5},
        {32, 3, 30, 34, 11, 9},
    };

    static constexpr i16 cell_faces[][4] = {
        {0, 2, 4, 6},
        {8, 10, 1, 12},
        {14, 16, 3, 18},
        {20, 22, 24, 5},
        {26, 28, 30, 7},
    };

    static Mesh const mesh{
        Tri::cube().vertex_positions,
        edge_verts,
        face_verts,
        face_edges,
        cell_verts,
        cell_edges,
        cell_faces,
    };

    return mesh;
}

} // namespace dr