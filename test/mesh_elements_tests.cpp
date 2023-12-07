#include <utest.h>

#include <dr/dynamic_array.hpp>
#include <dr/mesh_elements.hpp>
#include <dr/mesh_primitives.hpp>

UTEST(mesh, find_unique_oriented_edges)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec3<i32> const> face_vertices;
        struct
        {
            i32 num_edges;
        } expect;
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

    IncidenceMap<i32, 2> verts_to_oedge{};

    for (auto const& [face_verts, expect] : test_cases)
    {
        find_unique_oriented_edges(face_verts, verts_to_oedge);
        ASSERT_EQ(expect.num_edges * 2, size(verts_to_oedge));

        // Check that opposite edges are consecutive
        for (auto const& [e_v, e] : verts_to_oedge)
        {
            i32 const e_op = verts_to_oedge[e_v({1, 0})];
            if (e_op != -1)
                ASSERT_EQ(e ^ 1, e_op);
        }
    }
}

UTEST(mesh, collect_oriented_edge_tip_vertices)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec3<i32> const> face_vertices;
        struct
        {
            i32 num_edges;
        } expect;
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

    IncidenceMap<i32, 2> verts_to_oedge{};
    DynamicArray<i32> oedge_tip_verts{};

    for (auto const& [face_verts, expect] : test_cases)
    {
        find_unique_oriented_edges(face_verts, verts_to_oedge);
        ASSERT_EQ(expect.num_edges * 2, size(verts_to_oedge));

        oedge_tip_verts.resize(verts_to_oedge.size());
        collect_oriented_edge_tip_vertices(verts_to_oedge, face_verts, as_span(oedge_tip_verts));

        for (auto const& f_v : face_verts)
        {
            {
                auto const it = verts_to_oedge.find(f_v({0, 1}));
                ASSERT_TRUE(it != verts_to_oedge.end());
                ASSERT_EQ(f_v[2], oedge_tip_verts[it->second]);
            }

            {
                auto const it = verts_to_oedge.find(f_v({1, 2}));
                ASSERT_TRUE(it != verts_to_oedge.end());
                ASSERT_EQ(f_v[0], oedge_tip_verts[it->second]);
            }

            {
                auto const it = verts_to_oedge.find(f_v({2, 0}));
                ASSERT_TRUE(it != verts_to_oedge.end());
                ASSERT_EQ(f_v[1], oedge_tip_verts[it->second]);
            }
        }
    }
}

UTEST(mesh, collect_oriented_edge_faces)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec3<i32> const> face_vertices;
        struct
        {
            i32 num_edges;
        } expect;
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

    IncidenceMap<i32, 2> verts_to_oedge{};
    DynamicArray<i32> oedge_faces{};

    for (auto const& [face_verts, expect] : test_cases)
    {
        find_unique_oriented_edges(face_verts, verts_to_oedge);
        ASSERT_EQ(expect.num_edges * 2, size(verts_to_oedge));

        oedge_faces.resize(verts_to_oedge.size());
        collect_oriented_edge_faces(verts_to_oedge, face_verts, as_span(oedge_faces));

        for (isize f = 0; f < face_verts.size(); ++f)
        {
            Vec3<i32> const f_v = face_verts[f];

            {
                auto const it = verts_to_oedge.find(f_v({0, 1}));
                ASSERT_TRUE(it != verts_to_oedge.end());
                ASSERT_EQ(f, oedge_faces[it->second]);
            }

            {
                auto const it = verts_to_oedge.find(f_v({1, 2}));
                ASSERT_TRUE(it != verts_to_oedge.end());
                ASSERT_EQ(f, oedge_faces[it->second]);
            }

            {
                auto const it = verts_to_oedge.find(f_v({2, 0}));
                ASSERT_TRUE(it != verts_to_oedge.end());
                ASSERT_EQ(f, oedge_faces[it->second]);
            }
        }
    }
}

UTEST(mesh, collect_face_oriented_edges)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec3<i32> const> face_vertices;
        struct
        {
            i32 num_edges;
        } expect;
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

    IncidenceMap<i32, 2> verts_to_oedge{};
    DynamicArray<Vec3<i32>> face_oedges{};

    for (auto const& [face_verts, expect] : test_cases)
    {
        find_unique_oriented_edges(face_verts, verts_to_oedge);
        ASSERT_EQ(expect.num_edges * 2, size(verts_to_oedge));

        face_oedges.resize(face_verts.size());
        collect_face_oriented_edges(verts_to_oedge, face_verts, as_span(face_oedges));

        for (isize f = 0; f < face_verts.size(); ++f)
        {
            Vec3<i32> const f_v = face_verts[f];
            Vec3<i32> const f_e = face_oedges[f];

            {
                auto const it = verts_to_oedge.find(f_v({0, 1}));
                ASSERT_TRUE(it != verts_to_oedge.end());
                ASSERT_EQ(f_e[0], it->second);
            }

            {
                auto const it = verts_to_oedge.find(f_v({1, 2}));
                ASSERT_TRUE(it != verts_to_oedge.end());
                ASSERT_EQ(f_e[1], it->second);
            }

            {
                auto const it = verts_to_oedge.find(f_v({2, 0}));
                ASSERT_TRUE(it != verts_to_oedge.end());
                ASSERT_EQ(f_e[2], it->second);
            }
        }
    }
}

UTEST(mesh, find_unique_oriented_faces)
{
    using namespace dr;

    struct TestCase
    {
        Span<Vec4<i32> const> cell_vertices;
        struct
        {
            i32 num_faces;
        } result;
    };

    TestCase const test_cases[] = {
        {
            MeshCube::cell_vertices(),
            {16},
        },
    };

    IncidenceMap<i32, 3> verts_to_oface{};

    for (auto const& [cell_verts, result] : test_cases)
    {
        find_unique_oriented_faces(cell_verts, verts_to_oface);
        ASSERT_EQ(result.num_faces * 2, size(verts_to_oface));

        // Check that opposite faces are consecutive
        for (auto const& [f_v, f] : verts_to_oface)
        {
            i32 const f_op = verts_to_oface[f_v({0, 2, 1})];
            if (f_op != -1)
                ASSERT_EQ(f ^ 1, f_op);
        }
    }
}