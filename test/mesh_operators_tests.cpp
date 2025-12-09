#include <dr/mesh_operators.hpp>

/*
    Compile-time checks
*/

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

} // namespace dr