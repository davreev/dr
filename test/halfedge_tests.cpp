#include <utest/utest.h>

#include <dr/halfedge.hpp>
#include <dr/memory.hpp>

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

    // clang-format off
    SlicedArray<i32> cube_face_vertices{
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

    HalfedgeMesh cube_mesh{};
    HalfedgeMesh::Builder builder{};

    auto const err = builder.make_from_face_vertex(cube_face_vertices, cube_mesh);
    ASSERT_EQ(HalfedgeMesh::Builder::Error_None, err);
    ASSERT_EQ(8, cube_mesh.num_vertices());
    ASSERT_EQ(12, cube_mesh.num_edges());
    ASSERT_EQ(6, cube_mesh.num_faces());
    ASSERT_EQ(0, cube_mesh.num_holes());
}