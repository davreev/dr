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
        Span<Vec3<f32> const> vert_coords;
        Span<Vec3<i32> const> tri_verts;
        Vec3<f32> result;
    };

    using Prims = MeshPrimitives;

    TestCase const test_cases[] = {
        {
            Prims::Tetrahedron::vert_coords(),
            Prims::Tetrahedron::tri_verts(),
            {0.0, 0.0, 0.0},
        },
        {
            Prims::Octahedron::vert_coords(),
            Prims::Octahedron::tri_verts(),
            {0.0, 0.0, 0.0},
        },
        {
            Prims::Cube::vert_coords(),
            Prims::Cube::tri_verts(),
            {0.0, 0.0, 0.0},
        },
        {
            Prims::Icosahedron::vert_coords(),
            Prims::Icosahedron::tri_verts(),
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
        Span<Vec3<i32> const> tri_verts;
    };

    using Prims = MeshPrimitives;

    TestCase const test_cases[] = {
        {
            Prims::Tetrahedron::vert_coords(),
            Prims::Tetrahedron::tri_verts(),
        },
        {
            Prims::Octahedron::vert_coords(),
            Prims::Octahedron::tri_verts(),
        },
        {
            Prims::Icosahedron::vert_coords(),
            Prims::Icosahedron::tri_verts(),
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
        Span<Vec3<i32> const> tri_verts;
        f32 value;
        f32 result;
    };

    using Prims = MeshPrimitives;

    TestCase const test_cases[] = {
        {
            Prims::Cube::vert_coords(),
            Prims::Cube::tri_verts(),
            1.0,
            6.0,
        },
        {
            Prims::Cube::vert_coords(),
            Prims::Cube::tri_verts(),
            2.0,
            12.0,
        },
        {
            Prims::Cube::vert_coords(),
            Prims::Cube::tri_verts(),
            0.5,
            3.0,
        },
        {
            Prims::Cube::vert_coords(),
            Prims::Cube::tri_verts(),
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
