#include <dr/mesh_repair.hpp>

namespace dr
{

void remove_degenerate_faces(Span<Vec3<i32>>& face_vertices)
{
    isize num_valid = 0;

    for (isize i = 0; i < face_vertices.size(); ++i)
    {
        Vec3<i32> const f_v = face_vertices[i];

        if (f_v[0] != f_v[1] && f_v[1] != f_v[2] && f_v[2] != f_v[0])
        {
            face_vertices[num_valid] = f_v;
            ++num_valid;
        }
    }

    face_vertices = face_vertices.front(num_valid);
}

}