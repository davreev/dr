#include <utest.h>

#include <dr/dynamic_array.hpp>
#include <dr/mesh_attributes.hpp>
#include <dr/mesh_elements.hpp>
#include <dr/mesh_primitives.hpp>
#include <dr/mesh_utils.hpp>
#include <dr/triangulate.hpp>

#include "test_utils.hpp"

UTEST(mesh, append)
{
    using namespace dr;

    auto const src_verts = MeshCube::vertex_positions();
    auto const src_faces = MeshCube::face_vertices();

    DynamicArray<Vec3<f32>> dst_verts{};
    DynamicArray<Vec3<i32>> dst_faces{};

    append_elements<i32>(src_faces, dst_verts.size(), dst_faces);
    append_attributes(src_verts, dst_verts);

    append_elements<i32>(src_faces, dst_verts.size(), dst_faces);
    append_attributes(src_verts, dst_verts);

    ASSERT_EQ(size(dst_verts), src_verts.size() * 2);
    for (isize i = 0, j = src_verts.size(); i < src_verts.size(); ++i, ++j)
    {
        Vec3<f32> const& v0 = dst_verts[i];
        Vec3<f32> const& v1 = dst_verts[j];

        ASSERT_EQ(v0[0], v1[0]);
        ASSERT_EQ(v0[1], v1[1]);
        ASSERT_EQ(v0[2], v1[2]);
    }

    ASSERT_EQ(size(dst_faces), src_faces.size() * 2);
    for (isize i = 0, j = src_faces.size(); i < src_faces.size(); ++i, ++j)
    {
        Vec3<i32> const& f0 = dst_faces[i];
        Vec3<i32> const& f1 = dst_faces[j];

        isize const offset = src_verts.size();
        ASSERT_EQ(f0[0], f1[0] - offset);
        ASSERT_EQ(f0[1], f1[1] - offset);
        ASSERT_EQ(f0[2], f1[2] - offset);
    }
}

UTEST(mesh, fan_triangulator)
{
    using namespace dr;

    struct TestCase
    {
        i32 num_indices;
        struct
        {
            Vec3<i32> tris[16];
            i32 num_tris;
        } result;
    };

    TestCase const test_cases[] = {
        {
            3,
            {
                {{0, 1, 2}},
                1,
            },
        },
        {
            4,
            {
                {{0, 1, 2}, {0, 2, 3}},
                2,
            },
        },
        {
            5,
            {
                {{0, 1, 2}, {0, 2, 3}, {0, 3, 4}},
                3,
            },
        },
        {
            6,
            {
                {{0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 5}},
                4,
            },
        },
    };

    for (auto const& [num_indices, result] : test_cases)
    {
        DynamicArray<i32> poly(num_indices);

        for (i32 i = 0; i < num_indices; ++i)
            poly[i] = i;

        FanTriangulator<i32> tri_it{as_span(poly)};
        i32 tri_count = 0;

        for (; tri_it.is_valid(); tri_it.advance(), ++tri_count)
        {
            Vec3<i32> const a = result.tris[tri_count];
            Vec3<i32> const b = tri_it.current();
            ASSERT_EQ(a[0], b[0]);
            ASSERT_EQ(a[1], b[1]);
            ASSERT_EQ(a[2], b[2]);
        }

        ASSERT_EQ(result.num_tris, tri_count);
    }
}

UTEST(mesh, strip_triangulator)
{
    using namespace dr;

    struct TestCase
    {
        i32 num_indices;
        struct
        {
            Vec3<i32> tris[16];
            i32 num_tris;
        } result;
    };

    TestCase const test_cases[] = {
        {
            3,
            {
                {{0, 1, 2}},
                1,
            },
        },
        {
            4,
            {
                {{0, 1, 3}, {2, 3, 1}},
                2,
            },
        },
        {
            5,
            {
                {{0, 1, 4}, {2, 4, 1}, {4, 2, 3}},
                3,
            },
        },
        {
            6,
            {
                {{0, 1, 5}, {2, 5, 1}, {5, 2, 4}, {3, 4, 2}},
                4,
            },
        },
    };

    for (auto const& [num_indices, result] : test_cases)
    {
        DynamicArray<i32> poly(num_indices);

        for (i32 i = 0; i < num_indices; ++i)
            poly[i] = i;

        StripTriangulator<i32> tri_it{as_span(poly)};
        i32 tri_count = 0;

        for (; tri_it.is_valid(); tri_it.advance(), ++tri_count)
        {
            Vec3<i32> const a = result.tris[tri_count];
            Vec3<i32> const b = tri_it.current();
            ASSERT_EQ(a[0], b[0]);
            ASSERT_EQ(a[1], b[1]);
            ASSERT_EQ(a[2], b[2]);
        }

        ASSERT_EQ(result.num_tris, tri_count);
    }
}

UTEST(mesh, element_count)
{
    using namespace dr;

    struct TestCase
    {
        i32 num_vertices;
        i32 num_edges;
        i32 num_faces;
        i32 num_holes;
    };

    TestCase const test_cases[] = {
        {4, 6, 4, 0}, // Tet
        {8, 12, 6, 0}, // Cube
        {16, 32, 16, 1}, // Low-poly torus
        {26, 56, 28, 2}, // Low-poly double torus
    };

    for (auto const& [nv, ne, nf, nh] : test_cases)
    {
        ASSERT_EQ(nv, num_vertices(ne, nf, nh));
        ASSERT_EQ(ne, num_edges(nv, nf, nh));
        ASSERT_EQ(nf, num_faces(nv, ne, nh));
        ASSERT_EQ(nh, num_holes(nv, ne, nf));
    }
}

UTEST(mesh, make_vertex_to_edge)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec3<i32> const> face_vertices;
        struct
        {
            i32 num_edges;
        } result;
    };

    TestCase const test_cases[] = {
        {
            MeshTetrahedron::face_vertices(),
            {6},
        },
        {
            MeshCube::face_vertices(),
            {18},
        },
        {
            MeshIcosahedron::face_vertices(),
            {30},
        },
    };

    IncidenceMap<i32, 2> verts_to_edge{};
    DynamicArray<i32> edge_verts{};

    for (auto const& [f_v, result] : test_cases)
    {
        make_vertex_to_edge(f_v, verts_to_edge);

        edge_verts.resize(verts_to_edge.size());
        collect_edge_start_vertices(verts_to_edge, as_span(edge_verts));

        ASSERT_EQ(result.num_edges * 2, size_as<i32>(edge_verts));

        for (auto const& [e_v, e] : verts_to_edge)
        {
            ASSERT_EQ(e_v[0], edge_verts[e]);

            i32 const e_op = verts_to_edge[{e_v[1], e_v[0]}];

            if (e_op != -1)
            {
                ASSERT_EQ(e_v[1], edge_verts[e_op]);
                ASSERT_EQ(e ^ 1, e_op);
            }
        }
    }
}

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

        vertex_normals_area_weighted(
            vertex_positions,
            face_vertices,
            as_span(vertex_normals));

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
