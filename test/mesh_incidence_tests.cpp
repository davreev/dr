#include <utest.h>

#include <dr/dynamic_array.hpp>
#include <dr/memory.hpp>
#include <dr/mesh_incidence.hpp>
#include <dr/mesh_primitives.hpp>

UTEST(mesh_incidence, sanity)
{
    using namespace dr;

    VertexToEdge<i16>::Map vert_to_edge{};
    vert_to_edge[Vec2<i16>{0, 1}] = 0;

    VertexToTri<i16>::Map vert_to_tri{};
    vert_to_tri[Vec3<i16>{0, 1, 2}] = 0;

    VertexToTet<i16>::Map vert_to_tet{};
    vert_to_tet[Vec4<i16>{0, 1, 2, 3}] = 0;

    ASSERT_TRUE(true);
}

UTEST(mesh_incidence, make_vertex_to_edge)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec3<i16> const> tri_verts;
        struct
        {
            i16 num_edges;
        } expect;
    };

    using Prims = TriMeshPrims;

    TestCase const test_cases[] = {
        {
            as<Vec3<i16>>(Prims::Tetrahedron::face_vertices()),
            {6},
        },
        {
            as<Vec3<i16>>(Prims::Cube::face_vertices()),
            {18},
        },
        {
            as<Vec3<i16>>(Prims::Icosahedron::face_vertices()),
            {30},
        },
    };

    VertexToEdge<i16>::Map vert_to_edge{};

    for (auto const& [tri_verts, expect] : test_cases)
    {
        VertexToEdge<i16>::make_from_tris(tri_verts, vert_to_edge);
        ASSERT_EQ(expect.num_edges * 2, size(vert_to_edge));

        // Check that opposite edges are consecutive
        for (auto const& [k, e] : vert_to_edge)
        {
            i16 const e_op = vert_to_edge[k.opposite()];
            ASSERT_EQ(e ^ 1, e_op);
        }
    }
}

UTEST(mesh_incidence, make_vertex_to_tri)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec4<i16> const> tet_verts;
        struct
        {
            i16 num_faces;
        } result;
    };

    using Prims = TetMeshPrims;

    TestCase const test_cases[] = {
        {
            as<Vec4<i16>>(Prims::Cube::cell_vertices()),
            {16},
        },
    };

    VertexToTri<i16>::Map vert_to_tri{};

    for (auto const& [tet_verts, result] : test_cases)
    {
        VertexToTri<i16>::make_from_tets(tet_verts, vert_to_tri);
        ASSERT_EQ(result.num_faces * 2, size(vert_to_tri));

        // Check that opposite faces are consecutive
        for (auto const& [k, f] : vert_to_tri)
        {
            i16 const f_op = vert_to_tri[k.opposite()];
            ASSERT_EQ(f ^ 1, f_op);
        }
    }
}

UTEST(mesh_incidence, collect_edge_opposite_vertices)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec3<i16> const> tri_verts;
        struct
        {
            i16 num_edges;
        } expect;
    };

    using Prims = TriMeshPrims;

    TestCase const test_cases[] = {
        {
            as<Vec3<i16>>(Prims::Tetrahedron::face_vertices()),
            {6},
        },
        {
            as<Vec3<i16>>(Prims::Cube::face_vertices()),
            {18},
        },
        {
            as<Vec3<i16>>(Prims::Icosahedron::face_vertices()),
            {30},
        },
    };

    VertexToEdge<i16>::Map vert_to_edge{};
    DynamicArray<i16> edge_op_verts{};

    for (auto const& [tri_verts, expect] : test_cases)
    {
        VertexToEdge<i16>::make_from_tris(tri_verts, vert_to_edge);
        ASSERT_EQ(expect.num_edges * 2, size(vert_to_edge));

        edge_op_verts.resize(vert_to_edge.size());
        collect_edge_opposite_vertices(tri_verts, vert_to_edge, as_span(edge_op_verts));

        for (auto const& f_v : tri_verts)
        {
            Vec2<i16> const e_v[]{
                f_v({0, 1}),
                f_v({1, 2}),
                f_v({2, 0}),
            };

            i16 const e_v_op[]{f_v[2], f_v[0], f_v[1]};
            for (i8 i = 0; i < 3; ++i)
            {
                auto const it = vert_to_edge.find(e_v[i]);
                ASSERT_TRUE(it != vert_to_edge.end());
                ASSERT_EQ(e_v_op[i], edge_op_verts[it->second]);
            }
        }
    }
}

UTEST(mesh_incidence, collect_edge_tris)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec3<i16> const> tri_verts;
        struct
        {
            i16 num_edges;
        } expect;
    };

    using Prims = TriMeshPrims;

    TestCase const test_cases[] = {
        {
            as<Vec3<i16>>(Prims::Tetrahedron::face_vertices()),
            {6},
        },
        {
            as<Vec3<i16>>(Prims::Cube::face_vertices()),
            {18},
        },
        {
            as<Vec3<i16>>(Prims::Icosahedron::face_vertices()),
            {30},
        },
    };

    VertexToEdge<i16>::Map vert_to_edge{};
    DynamicArray<i16> edge_tris{};

    for (auto const& [tri_verts, expect] : test_cases)
    {
        VertexToEdge<i16>::make_from_tris(tri_verts, vert_to_edge);
        ASSERT_EQ(expect.num_edges * 2, size(vert_to_edge));

        edge_tris.resize(vert_to_edge.size());
        collect_edge_tris(tri_verts, vert_to_edge, as_span(edge_tris));

        for (isize f = 0; f < tri_verts.size(); ++f)
        {
            Vec3<i16> const f_v = tri_verts[f];
            Vec2<i16> const e_v[]{
                f_v({0, 1}),
                f_v({1, 2}),
                f_v({2, 0}),
            };

            for (i8 i = 0; i < 3; ++i)
            {
                auto const it = vert_to_edge.find(e_v[i]);
                ASSERT_TRUE(it != vert_to_edge.end());
                ASSERT_EQ(f, edge_tris[it->second]);
            }
        }
    }
}

UTEST(mesh_incidence, collect_tri_edges)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec3<i16> const> tri_verts;
        struct
        {
            i16 num_edges;
        } expect;
    };

    using Prims = TriMeshPrims;

    TestCase const test_cases[] = {
        {
            as<Vec3<i16>>(Prims::Tetrahedron::face_vertices()),
            {6},
        },
        {
            as<Vec3<i16>>(Prims::Cube::face_vertices()),
            {18},
        },
        {
            as<Vec3<i16>>(Prims::Icosahedron::face_vertices()),
            {30},
        },
    };

    VertexToEdge<i16>::Map vert_to_edge{};
    DynamicArray<Vec3<i16>> tri_edges{};

    for (auto const& [tri_verts, expect] : test_cases)
    {
        VertexToEdge<i16>::make_from_tris(tri_verts, vert_to_edge);
        ASSERT_EQ(expect.num_edges * 2, size(vert_to_edge));

        tri_edges.resize(tri_verts.size());
        collect_tri_edges(tri_verts, vert_to_edge, as_span(tri_edges));

        for (isize f = 0; f < tri_verts.size(); ++f)
        {
            Vec3<i16> const f_v = tri_verts[f];
            Vec2<i16> const e_v[]{
                f_v({0, 1}),
                f_v({1, 2}),
                f_v({2, 0}),
            };

            Vec3<i16> const f_e = tri_edges[f];
            for (i8 i = 0; i < 3; ++i)
            {
                auto const it = vert_to_edge.find(e_v[i]);
                ASSERT_TRUE(it != vert_to_edge.end());
                ASSERT_EQ(f_e[i], it->second);
            }
        }
    }
}

UTEST(mesh_incidence, collect_tet_tris)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec4<i16> const> tet_verts;
        struct
        {
            i16 num_faces;
        } expect;
    };

    using Prims = TetMeshPrims;

    TestCase const test_cases[] = {
        {
            as<Vec4<i16>>(Prims::Cube::cell_vertices()),
            {16},
        },
    };

    VertexToTri<i16>::Map vert_to_tri{};
    DynamicArray<Vec4<i16>> tet_tris{};

    for (auto const& [tet_verts, expect] : test_cases)
    {
        VertexToTri<i16>::make_from_tets(tet_verts, vert_to_tri);
        ASSERT_EQ(expect.num_faces * 2, size(vert_to_tri));

        tet_tris.resize(tet_verts.size());
        collect_tet_tris(tet_verts, vert_to_tri, as_span(tet_tris));

        for (isize c = 0; c < tet_verts.size(); ++c)
        {
            auto const& c_v = tet_verts[c];
            Vec3<i16> const f_v[]{
                c_v({0, 1, 2}),
                c_v({1, 0, 3}),
                c_v({2, 3, 0}),
                c_v({3, 2, 1}),
            };

            Vec4<i16> const& c_f = tet_tris[c];
            for (i8 i = 0; i < 4; ++i)
            {
                auto const it = vert_to_tri.find(f_v[i]);
                ASSERT_TRUE(it != vert_to_tri.end());
                ASSERT_EQ(c_f[i], it->second);
            }
        }
    }
}
