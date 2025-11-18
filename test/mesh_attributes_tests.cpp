#include <utest.h>

#include <dr/dynamic_array.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/mesh_attributes.hpp>
#include <dr/mesh_primitives.hpp>
#include <dr/random.hpp>

UTEST(mesh, area_centroid)
{
    using namespace dr;

    constexpr f32 eps = 1.0e-5;

    struct TestCase
    {
        Span<Vec3<f32> const> vertex_positions;
        Span<Vec3<i16> const> face_vertices;
        Vec3<f32> expect;
    };

    using MeshPrims = MeshPrimitives::Tri;

    TestCase const test_cases[] = {
        {
            as<Vec3<f32>>(MeshPrims::tetrahedron().vertex_positions),
            as<Vec3<i16>>(MeshPrims::tetrahedron().face_vertices),
            {0.0, 0.0, 0.0},
        },
        {
            as<Vec3<f32>>(MeshPrims::octahedron().vertex_positions),
            as<Vec3<i16>>(MeshPrims::octahedron().face_vertices),
            {0.0, 0.0, 0.0},
        },
        {
            as<Vec3<f32>>(MeshPrims::cube().vertex_positions),
            as<Vec3<i16>>(MeshPrims::cube().face_vertices),
            {0.0, 0.0, 0.0},
        },
        {
            as<Vec3<f32>>(MeshPrims::icosahedron().vertex_positions),
            as<Vec3<i16>>(MeshPrims::icosahedron().face_vertices),
            {0.0, 0.0, 0.0},
        },
    };

    for (auto const& [vert_coords, tri_verts, expect] : test_cases)
    {
        Vec3<f32> const p = area_centroid(vert_coords, tri_verts);
        ASSERT_NEAR(expect[0], p[0], eps);
        ASSERT_NEAR(expect[1], p[1], eps);
        ASSERT_NEAR(expect[2], p[2], eps);
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

    using MeshPrims = MeshPrimitives::Tri;

    TestCase const test_cases[] = {
        {
            as<Vec3<f32>>(MeshPrims::tetrahedron().vertex_positions),
            as<Vec3<i16>>(MeshPrims::tetrahedron().face_vertices),
        },
        {
            as<Vec3<f32>>(MeshPrims::octahedron().vertex_positions),
            as<Vec3<i16>>(MeshPrims::octahedron().face_vertices),
        },
        {
            as<Vec3<f32>>(MeshPrims::icosahedron().vertex_positions),
            as<Vec3<i16>>(MeshPrims::icosahedron().face_vertices),
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

UTEST(mesh, integrate_vertex_func)
{
    using namespace dr;

    constexpr f32 eps = 1.0e-5;

    struct TestCase
    {
        Span<Vec3<f32> const> vert_coords;
        Span<Vec3<i16> const> tri_verts;
        f32 value;
        f32 expect;
    };

    auto const mesh = MeshPrimitives::Tri::cube();

    TestCase const test_cases[] = {
        {
            as<Vec3<f32>>(mesh.vertex_positions),
            as<Vec3<i16>>(mesh.face_vertices),
            1.0,
            6.0,
        },
        {
            as<Vec3<f32>>(mesh.vertex_positions),
            as<Vec3<i16>>(mesh.face_vertices),
            2.0,
            12.0,
        },
        {
            as<Vec3<f32>>(mesh.vertex_positions),
            as<Vec3<i16>>(mesh.face_vertices),
            0.5,
            3.0,
        },
        {
            as<Vec3<f32>>(mesh.vertex_positions),
            as<Vec3<i16>>(mesh.face_vertices),
            0.0,
            0.0,
        },
    };

    for (auto const& [vert_coords, tri_verts, value, expect] : test_cases)
    {
        isize const num_verts = vert_coords.size();
        DynamicArray<f32> vertex_values(num_verts, value);

        f32 area{};
        f32 const sum = integrate_vertex_func(
            vert_coords,
            as_span(vertex_values).as_const(),
            tri_verts,
            area);

        ASSERT_NEAR(expect, sum, eps);
        ASSERT_NEAR(expect, area * value, eps);
    }
}

UTEST(mesh, interpolate_mean_value)
{
    using namespace dr;

    constexpr isize num_samples = 1000;
    constexpr f32 eps = 1.0e-5;

    constexpr auto do_test = //
        [](Span<Vec3<f32> const> const& v_p,
           Span<Vec3<i16> const> const& f_v) -> isize //
    {
        Interval3<f32> box = bounding_interval(v_p);

        Random rand{1};
        auto gen = rand.generator(0.0f, 1.0f);

        auto const box_point_at = [&](Vec3<f32> const& t) -> Vec3<f32> {
            return box.from.array() + box.delta().array() * t.array();
        };

        // NOTE: Mean value interpolation of the coordinate function should be equal to the sample
        // point. This is checked at random sample points within the bounding box of the mesh.
        isize num_equal = 0;

        for (isize i = 0; i < num_samples; ++i)
        {
            Vec3<f32> const p = box_point_at({gen(), gen(), gen()});

            // NOTE: Naive impl has better accuracy on convex shape interiors
            Vec3<f32> const result = interpolate_mean_value_naive(v_p, v_p, f_v, p);
            // Vec3<f32> const result = interpolate_mean_value_robust(v_p, v_p, f_v, p);

            if (near_equal(p, result, eps))
                ++num_equal;
        }

        return num_equal;
    };

    {
        auto const& mesh = MeshPrimitives::Tri::icosahedron();
        isize const num_eq = do_test(
            as<Vec3<f32>>(mesh.vertex_positions),
            as<Vec3<i16>>(mesh.face_vertices));

        ASSERT_EQ(num_samples, num_eq);
    }

    {
        auto const& mesh = MeshPrimitives::Tri::octahedron();
        isize const num_eq = do_test(
            as<Vec3<f32>>(mesh.vertex_positions),
            as<Vec3<i16>>(mesh.face_vertices));

        ASSERT_EQ(num_samples, num_eq);
    }

    {
        auto const& mesh = MeshPrimitives::Tri::cube();
        isize const num_eq = do_test(
            as<Vec3<f32>>(mesh.vertex_positions),
            as<Vec3<i16>>(mesh.face_vertices));

        ASSERT_EQ(num_samples, num_eq);
    }
}