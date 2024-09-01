#include <utest.h>

#include <dr/dynamic_array.hpp>
#include <dr/memory.hpp>
#include <dr/mesh_incidence.hpp>
#include <dr/mesh_primitives.hpp>

UTEST(mesh_incidence, make_verts_to_edge)
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

    VertsToEdge<i16>::Map verts_to_edge{};

    for (auto const& [tri_verts, expect] : test_cases)
    {
        VertsToEdge<i16>::make_from_tris(tri_verts, verts_to_edge);
        ASSERT_EQ(expect.num_edges * 2, size(verts_to_edge));

        // Check that opposite edges are consecutive
        for (auto const& [k, e] : verts_to_edge)
        {
            i16 const e_op = verts_to_edge[k.opposite()];
            ASSERT_EQ(e ^ 1, e_op);
        }
    }
}

UTEST(mesh_incidence, make_verts_to_tri)
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

    VertsToTri<i16>::Map verts_to_tri{};

    for (auto const& [tet_verts, result] : test_cases)
    {
        VertsToTri<i16>::make_from_tets(tet_verts, verts_to_tri);
        ASSERT_EQ(result.num_faces * 2, size(verts_to_tri));

        // Check that opposite faces are consecutive
        for (auto const& [k, f] : verts_to_tri)
        {
            i16 const f_op = verts_to_tri[k.opposite()];
            ASSERT_EQ(f ^ 1, f_op);
        }
    }
}

UTEST(mesh_incidence, collect_edge_opposite_verts)
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

    VertsToEdge<i16>::Map verts_to_edge{};
    DynamicArray<i16> edge_op_verts{};

    for (auto const& [tri_verts, expect] : test_cases)
    {
        VertsToEdge<i16>::make_from_tris(tri_verts, verts_to_edge);
        ASSERT_EQ(expect.num_edges * 2, size(verts_to_edge));

        edge_op_verts.resize(verts_to_edge.size());
        collect_edge_opposite_verts(tri_verts, verts_to_edge, as_span(edge_op_verts));

        for (auto const& f_v : tri_verts)
        {
            Vec2<i16> const e_v[]{
                f_v({0, 1}),
                f_v({1, 2}),
                f_v({2, 0}),
            };

            {
                auto const it = verts_to_edge.find(e_v[0]);
                ASSERT_TRUE(it != verts_to_edge.end());
                ASSERT_EQ(f_v[2], edge_op_verts[it->second]);
            }

            {
                auto const it = verts_to_edge.find(e_v[1]);
                ASSERT_TRUE(it != verts_to_edge.end());
                ASSERT_EQ(f_v[0], edge_op_verts[it->second]);
            }

            {
                auto const it = verts_to_edge.find(e_v[2]);
                ASSERT_TRUE(it != verts_to_edge.end());
                ASSERT_EQ(f_v[1], edge_op_verts[it->second]);
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

    VertsToEdge<i16>::Map verts_to_edge{};
    DynamicArray<i16> edge_tris{};

    for (auto const& [tri_verts, expect] : test_cases)
    {
        VertsToEdge<i16>::make_from_tris(tri_verts, verts_to_edge);
        ASSERT_EQ(expect.num_edges * 2, size(verts_to_edge));

        edge_tris.resize(verts_to_edge.size());
        collect_edge_tris(tri_verts, verts_to_edge, as_span(edge_tris));

        for (isize f = 0; f < tri_verts.size(); ++f)
        {
            Vec3<i16> const f_v = tri_verts[f];
            Vec2<i16> const e_v[]{
                f_v({0, 1}),
                f_v({1, 2}),
                f_v({2, 0}),
            };

            {
                auto const it = verts_to_edge.find(e_v[0]);
                ASSERT_TRUE(it != verts_to_edge.end());
                ASSERT_EQ(f, edge_tris[it->second]);
            }

            {
                auto const it = verts_to_edge.find(e_v[1]);
                ASSERT_TRUE(it != verts_to_edge.end());
                ASSERT_EQ(f, edge_tris[it->second]);
            }

            {
                auto const it = verts_to_edge.find(e_v[2]);
                ASSERT_TRUE(it != verts_to_edge.end());
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

    VertsToEdge<i16>::Map verts_to_edge{};
    DynamicArray<Vec3<i16>> tri_edges{};

    for (auto const& [tri_verts, expect] : test_cases)
    {
        VertsToEdge<i16>::make_from_tris(tri_verts, verts_to_edge);
        ASSERT_EQ(expect.num_edges * 2, size(verts_to_edge));

        tri_edges.resize(tri_verts.size());
        collect_tri_edges(tri_verts, verts_to_edge, as_span(tri_edges));

        for (isize f = 0; f < tri_verts.size(); ++f)
        {
            Vec3<i16> const f_v = tri_verts[f];
            Vec2<i16> const e_v[]{
                f_v({0, 1}),
                f_v({1, 2}),
                f_v({2, 0}),
            };

            Vec3<i16> const f_e = tri_edges[f];

            {
                auto const it = verts_to_edge.find(e_v[0]);
                ASSERT_TRUE(it != verts_to_edge.end());
                ASSERT_EQ(f_e[0], it->second);
            }

            {
                auto const it = verts_to_edge.find(e_v[1]);
                ASSERT_TRUE(it != verts_to_edge.end());
                ASSERT_EQ(f_e[1], it->second);
            }

            {
                auto const it = verts_to_edge.find(e_v[2]);
                ASSERT_TRUE(it != verts_to_edge.end());
                ASSERT_EQ(f_e[2], it->second);
            }
        }
    }
}
