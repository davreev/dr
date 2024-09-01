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
        Span<Vec3<i16> const> face_vertices;
        Vec3<f32> result;
    };

    using Prims = TriMeshPrims;

    TestCase const test_cases[] = {
        {
            as<Vec3<f32>>(Prims::Tetrahedron::vertex_positions()),
            as<Vec3<i16>>(Prims::Tetrahedron::face_vertices()),
            {0.0, 0.0, 0.0},
        },
        {
            as<Vec3<f32>>(Prims::Octahedron::vertex_positions()),
            as<Vec3<i16>>(Prims::Octahedron::face_vertices()),
            {0.0, 0.0, 0.0},
        },
        {
            as<Vec3<f32>>(Prims::Cube::vertex_positions()),
            as<Vec3<i16>>(Prims::Cube::face_vertices()),
            {0.0, 0.0, 0.0},
        },
        {
            as<Vec3<f32>>(Prims::Icosahedron::vertex_positions()),
            as<Vec3<i16>>(Prims::Icosahedron::face_vertices()),
            {0.0, 0.0, 0.0},
        },
    };

    for (auto const& [vert_coords, tri_verts, result] : test_cases)
    {
        Vec3<f32> const p = area_centroid(vert_coords, tri_verts);
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
        Span<Vec3<f32> const> vert_coords;
        Span<Vec3<i16> const> tri_verts;
    };

    using Prims = TriMeshPrims;

    TestCase const test_cases[] = {
        {
            as<Vec3<f32>>(Prims::Tetrahedron::vertex_positions()),
            as<Vec3<i16>>(Prims::Tetrahedron::face_vertices()),
        },
        {
            as<Vec3<f32>>(Prims::Octahedron::vertex_positions()),
            as<Vec3<i16>>(Prims::Octahedron::face_vertices()),
        },
        {
            as<Vec3<f32>>(Prims::Icosahedron::vertex_positions()),
            as<Vec3<i16>>(Prims::Icosahedron::face_vertices()),
        },
    };

    for (auto const& [vert_coords, tri_verts] : test_cases)
    {
        isize const num_verts = vert_coords.size();
        DynamicArray<Vec3<f32>> vertex_normals(num_verts);

        vertex_normals_area_weighted(vert_coords, tri_verts, as_span(vertex_normals));

        for (isize i = 0; i < num_verts; ++i)
        {
            Vec3<f32> const d = vert_coords[i].normalized();
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
        Span<Vec3<f32> const> vert_coords;
        Span<Vec3<i16> const> tri_verts;
        f32 value;
        f32 result;
    };

    using Prims = TriMeshPrims;

    TestCase const test_cases[] = {
        {
            as<Vec3<f32>>(Prims::Cube::vertex_positions()),
            as<Vec3<i16>>(Prims::Cube::face_vertices()),
            1.0,
            6.0,
        },
        {
            as<Vec3<f32>>(Prims::Cube::vertex_positions()),
            as<Vec3<i16>>(Prims::Cube::face_vertices()),
            2.0,
            12.0,
        },
        {
            as<Vec3<f32>>(Prims::Cube::vertex_positions()),
            as<Vec3<i16>>(Prims::Cube::face_vertices()),
            0.5,
            3.0,
        },
        {
            as<Vec3<f32>>(Prims::Cube::vertex_positions()),
            as<Vec3<i16>>(Prims::Cube::face_vertices()),
            0.0,
            0.0,
        },
    };

    for (auto const& [vert_coords, tri_verts, value, result] : test_cases)
    {
        isize const num_verts = vert_coords.size();
        DynamicArray<f32> vertex_values(num_verts, value);

        f32 area{};
        f32 const sum = integrate_vertex_values(
            vert_coords,
            as_span(vertex_values).as_const(),
            tri_verts,
            area);

        ASSERT_NEAR(result, sum, eps);
        ASSERT_NEAR(result, area * value, eps);
    }
}
