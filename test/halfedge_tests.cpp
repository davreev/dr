#include <utest.h>

#include <dr/halfedge.hpp>
#include <dr/memory.hpp>
#include <dr/mesh_primitives.hpp>

UTEST(halfedge, allocator_propagation)
{
    using namespace dr;

    DebugMemoryResource mr{};
    HalfedgeMesh src{&mr};

    {
        HalfedgeMesh dst{src};
        ASSERT_EQ(dst.allocator().resource(), std::pmr::get_default_resource());
    }

    {
        HalfedgeMesh dst{src, src.allocator()};
        ASSERT_EQ(dst.allocator().resource(), &mr);
    }

    {
        HalfedgeMesh dst{std::move(src)};
        ASSERT_EQ(dst.allocator().resource(), &mr);
    }
}

UTEST(halfedge, build_cube)
{
    using namespace dr;

    HalfedgeMesh::Builder builder{};

    {
        auto const face_verts = as<Vec3<i16>>(TriMeshPrims::Cube::face_vertices());

        HalfedgeMesh mesh{};
        auto const err = builder.make_from_face_vertex(face_verts, mesh);

        ASSERT_EQ(HalfedgeMesh::Builder::Error_None, err);
        ASSERT_EQ(8, mesh.num_vertices());
        ASSERT_EQ(18, mesh.num_edges());
        ASSERT_EQ(12, mesh.num_faces());
        ASSERT_EQ(0, mesh.num_holes());
    }

    {
        auto const face_verts = as<Vec4<i16>>(QuadMeshPrims::Cube::face_vertices());

        HalfedgeMesh mesh{};
        auto const err = builder.make_from_face_vertex(face_verts, mesh);

        ASSERT_EQ(HalfedgeMesh::Builder::Error_None, err);
        ASSERT_EQ(8, mesh.num_vertices());
        ASSERT_EQ(12, mesh.num_edges());
        ASSERT_EQ(6, mesh.num_faces());
        ASSERT_EQ(0, mesh.num_holes());
    }

    {
        // clang-format off
        SlicedArray<i32> const face_verts{
            {
                0, 2, 3, 1,
                0, 1, 5, 4,
                1, 3, 7, 5,
                3, 2, 6, 7,
                2, 0, 4, 6,
                7, 6, 4, 5,
            },
            { 4, 8, 12, 16, 20, 24 },
        };
        // clang-format on

        HalfedgeMesh mesh{};
        auto const err = builder.make_from_face_vertex(face_verts, mesh);

        ASSERT_EQ(HalfedgeMesh::Builder::Error_None, err);
        ASSERT_EQ(8, mesh.num_vertices());
        ASSERT_EQ(12, mesh.num_edges());
        ASSERT_EQ(6, mesh.num_faces());
        ASSERT_EQ(0, mesh.num_holes());
    }
}