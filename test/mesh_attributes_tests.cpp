#include <utest.h>

#include <dr/dynamic_array.hpp>
#include <dr/mesh_attributes.hpp>
#include <dr/mesh_primitives.hpp>

UTEST(mesh, area_centroid)
{
    using namespace dr;

    constexpr f32 eps = 1.0e-5;

    struct TestCase
    {
        Span<Vec3<f32> const> vertex_positions;
        Span<Vec3<i32> const> face_vertices;
        Vec3<f32> result;
    };

    TestCase const test_cases[] = {
        {
            MeshTetrahedron::vertex_positions(),
            MeshTetrahedron::face_vertices(),
            {0.0, 0.0, 0.0},
        },
        {
            MeshOctahedron::vertex_positions(),
            MeshOctahedron::face_vertices(),
            {0.0, 0.0, 0.0},
        },
        {
            MeshCube::vertex_positions(),
            MeshCube::face_vertices(),
            {0.0, 0.0, 0.0},
        },
        {
            MeshIcosahedron::vertex_positions(),
            MeshIcosahedron::face_vertices(),
            {0.0, 0.0, 0.0},
        },
    };

    for (auto const& [vertex_positions, face_vertices, result] : test_cases)
    {
        Vec3<f32> const p = area_centroid(vertex_positions, face_vertices);
        ASSERT_NEAR(result[0], p[0], eps);
        ASSERT_NEAR(result[1], p[1], eps);
        ASSERT_NEAR(result[2], p[2], eps);
    }
}

UTEST(mesh, vertex_normals_area_weighted)
{
    using namespace dr;

    constexpr f32 eps = 1.0e-5;

    struct TestCase
    {
        Span<Vec3<f32> const> vertex_positions;
        Span<Vec3<i32> const> face_vertices;
    };

    TestCase const test_cases[] = {
        {
            MeshTetrahedron::vertex_positions(),
            MeshTetrahedron::face_vertices(),
        },
        {
            MeshOctahedron::vertex_positions(),
            MeshOctahedron::face_vertices(),
        },
        {
            MeshIcosahedron::vertex_positions(),
            MeshIcosahedron::face_vertices(),
        },
    };

    for (auto const& [vertex_positions, face_vertices] : test_cases)
    {
        isize const num_verts = vertex_positions.size();
        DynamicArray<Vec3<f32>> vertex_normals(num_verts);

        vertex_normals_area_weighted(vertex_positions, face_vertices, as_span(vertex_normals));

        for (isize i = 0; i < num_verts; ++i)
        {
            Vec3<f32> const d = vertex_positions[i].normalized();
            Vec3<f32> const n = vertex_normals[i];
            ASSERT_NEAR(d[0], n[0], eps);
            ASSERT_NEAR(d[1], n[1], eps);
            ASSERT_NEAR(d[2], n[2], eps);
        }
    }
}

UTEST(mesh, integrate_vertex_values)
{
    using namespace dr;

    constexpr f32 eps = 1.0e-5;

    struct TestCase
    {
        Span<Vec3<f32> const> vertex_positions;
        Span<Vec3<i32> const> face_vertices;
        f32 value;
        f32 result;
    };

    TestCase const test_cases[] = {
        {
            MeshCube::vertex_positions(),
            MeshCube::face_vertices(),
            1.0,
            6.0,
        },
        {
            MeshCube::vertex_positions(),
            MeshCube::face_vertices(),
            2.0,
            12.0,
        },
        {
            MeshCube::vertex_positions(),
            MeshCube::face_vertices(),
            0.5,
            3.0,
        },
        {
            MeshCube::vertex_positions(),
            MeshCube::face_vertices(),
            0.0,
            0.0,
        },
    };

    for (auto const& [vertex_positions, face_vertices, value, result] : test_cases)
    {
        isize const num_verts = vertex_positions.size();
        DynamicArray<f32> vertex_values(num_verts, value);

        f32 area{};
        f32 const sum = integrate_vertex_values(
            vertex_positions,
            as_span(vertex_values).as_const(),
            face_vertices,
            area);

        ASSERT_NEAR(result, sum, eps);
        ASSERT_NEAR(result, area * value, eps);
    }
}
