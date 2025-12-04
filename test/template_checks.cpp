/*
    Explicitly instantiating templated types and functions to catch compile time errors
*/

#include <dr/mesh_attributes.hpp>
#include <dr/mesh_operators.hpp>

namespace dr
{

template void make_cotan_laplacian(
    Span<Vec3<f32> const> const&,
    Span<Vec3<i32> const> const&,
    DynamicArray<Triplet<f32, i32>>&);

template void make_incidence_matrix(
    Span<Vec<i32, 2> const> const&,
    DynamicArray<Triplet<f32, i32>>&);

template void make_vector_area_matrix(
    Span<Vec2<i32> const> const&,
    DynamicArray<Triplet<f32, i32>>&,
    i32);

template void eval_gradient(
    Span<Vec3<f32> const> const&,
    Span<f32 const> const&,
    Span<Vec3<i32> const> const&,
    Span<Covec3<f32>> const&,
    isize);

template void eval_jacobian(
    Span<Vec3<f32> const> const&,
    Span<Vec3<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Span<Mat3<f32>> const&,
    isize);

template void eval_divergence(
    Span<Vec3<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Span<Vec3<f32> const> const&,
    Span<f32> const&);

template void eval_laplacian(
    Span<Vec3<f32> const> const&,
    Span<f32 const> const&,
    Span<Vec3<i32> const> const&,
    Span<f32> const&);

template void eval_laplacian(
    Span<Vec3<f32> const> const&,
    Span<Vec<f32, 2> const> const&,
    Span<Vec3<i32> const> const&,
    Span<Vec<f32, 2>> const&);

template void vertex_vector_areas(
    Span<Vec3<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Span<Vec3<f32>> const&);

template void vertex_normals_area_weighted(
    Span<Vec3<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Span<Vec3<f32>> const&);

template void vertex_areas_barycentric(
    Span<Vec3<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Span<f32> const&);

template void vertex_degrees(Span<Vec3<i32> const> const&, Span<f32> const&);

template void face_vector_areas(
    Span<Vec3<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Span<Vec3<f32>> const&,
    isize const);

template void face_normals(
    Span<Vec3<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Span<Vec3<f32>> const&,
    isize const);

template Vec2<f32> integrate_vertex_func(
    Span<Vec3<f32> const> const&,
    Span<Vec2<f32> const> const&,
    Span<Vec4<i32> const> const&,
    f32&);

template Vec2<f32> integrate_vertex_func(
    Span<Vec3<f32> const> const&,
    Span<Vec2<f32> const> const&,
    Span<Vec3<i32> const> const&,
    f32&);

template Vec2<f32> integrate_vertex_func(
    Span<Vec3<f32> const> const&,
    Span<Vec2<f32> const> const&,
    Span<Vec2<i32> const> const&,
    f32&);

template Vec3<f32> area_centroid(Span<Vec3<f32> const> const&, Span<Vec3<i32> const> const&);

template Vec3<f32> length_centroid(Span<Vec3<f32> const> const&, Span<Vec2<i32> const> const&);

template f32 winding_number(
    Span<Vec3<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Vec3<f32> const&);

template Vec2<f32> interpolate_mean_value_robust(
    Span<Vec3<f32> const> const&,
    Span<Vec2<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Vec3<f32> const&,
    f32 const,
    isize const);

template Vec2<f32> interpolate_mean_value_naive(
    Span<Vec3<f32> const> const&,
    Span<Vec2<f32> const> const&,
    Span<Vec3<i32> const> const&,
    Vec3<f32> const&,
    f32 const,
    isize const);

} // namespace dr