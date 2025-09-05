#include <utest.h>

#include <dr/mesh_primitives.hpp>
#include <dr/mesh_utils.hpp>
#include <dr/memory.hpp>

UTEST(mesh, append)
{
    using namespace dr;

    auto& mesh = MeshPrimitives::Tri::cube();
    auto const src_verts = as<Vec3<f32>>(mesh.vertex_positions);
    auto const src_faces = as<Vec3<i16>>(mesh.face_vertices);

    DynamicArray<Vec3<f32>> dst_verts{};
    DynamicArray<Vec3<i16>> dst_faces{};

    append_elements<i16>(src_faces, dst_verts.size(), dst_faces);
    append_attributes(src_verts, dst_verts);

    append_elements<i16>(src_faces, dst_verts.size(), dst_faces);
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
        Vec3<i16> const& f0 = dst_faces[i];
        Vec3<i16> const& f1 = dst_faces[j];

        isize const offset = src_verts.size();
        ASSERT_EQ(f0[0], f1[0] - offset);
        ASSERT_EQ(f0[1], f1[1] - offset);
        ASSERT_EQ(f0[2], f1[2] - offset);
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