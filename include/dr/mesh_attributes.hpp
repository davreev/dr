#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>

#include <dr/geometry.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/linalg_types.hpp>
#include <dr/math.hpp>
#include <dr/span.hpp>

namespace dr
{

/// Computes the vector area of each vertex dual cell in a triange mesh
template <typename Real, typename Index>
void vertex_vector_areas(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec3<Real>> const& result)
{
    assert(result.size() == vertex_positions.size());
    std::fill(begin(result), end(result), Vec3<Real>::Zero());

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];
        constexpr Real inv3{1.0 / 3.0};

        // clang-format off
        Vec3<Real> const area = inv3 * vector_area(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]]);
        // clang-format on

        result[f_v[0]] += area;
        result[f_v[1]] += area;
        result[f_v[2]] += area;
    }
}

/// Computes the area-weighted normal of each vertex in a triange mesh
template <typename Real, typename Index>
void vertex_normals_area_weighted(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec3<Real>> const& result)
{
    assert(result.size() == vertex_positions.size());
    vertex_vector_areas(vertex_positions, face_vertices, result);
    as_mat(result).colwise().normalize();
}

/// Computes the barycentric dual area of each vertex in a triangle mesh
template <typename Real, typename Index>
void vertex_areas_barycentric(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Real> const& result)
{
    assert(result.size() == vertex_positions.size());
    std::fill(begin(result), end(result), Real{0.0});

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];
        constexpr Real inv3{1.0 / 3.0};

        // clang-format off
        Real const area = inv3 * vector_area(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]]).norm();
        // clang-format on

        result[f_v[0]] += area;
        result[f_v[1]] += area;
        result[f_v[2]] += area;
    }
}

/// Computes the degree of each vertex in a triangle mesh
template <typename Scalar, typename Index>
void vertex_degrees(Span<Vec3<Index> const> const& face_vertices, Span<Scalar> const& result)
{
    std::fill(begin(result), end(result), Scalar{0});

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];
        result[f_v[0]] += Scalar{1};
        result[f_v[1]] += Scalar{1};
        result[f_v[2]] += Scalar{1};
    }
}

/// Computes the vector area of each face in a triangle mesh
template <typename Real, typename Index>
void face_vector_areas(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec3<Real>> const& result,
    isize const num_threads = 1)
{
    assert(result.size() == face_vertices.size());

    auto const loop_body = [&](isize const f) {
        auto const& f_v = face_vertices[f];
        result[f] = vector_area(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]]);
    };

    if (num_threads > 1)
    {
#pragma omp parallel for num_threads(num_threads) schedule(static)
        for (isize f = 0; f < face_vertices.size(); ++f)
            loop_body(f);
    }
    else
    {
        for (isize f = 0; f < face_vertices.size(); ++f)
            loop_body(f);
    }
}

/// Computes the normal of each face in a triangle mesh
template <typename Real, typename Index>
void face_normals(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    Span<Vec3<Real>> const& result,
    isize const num_threads = 1)
{
    assert(result.size() == face_vertices.size());
    face_vector_areas(vertex_positions, face_vertices, result, num_threads);
    as_mat(result).colwise().normalize();
}

/// Returns the integral of a function defined on vertices of a discretized volume
template <typename Real, typename Value, typename Index>
Value integrate_vertex_func(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Value const> const& vertex_values,
    Span<Vec4<Index> const> const& cell_vertices,
    Real& volume)
{
    Value result{};
    volume = Real{0.0};

    for (isize c = 0; c < cell_vertices.size(); ++c)
    {
        auto const& c_v = cell_vertices[c];

        // clang-format off
        Value const c_val = Real{0.25} * (
            vertex_values[c_v[0]] + 
            vertex_values[c_v[1]] + 
            vertex_values[c_v[2]] + 
            vertex_values[c_v[3]]);
        // clang-format on

        Real const c_vol = signed_volume(
            vertex_positions[c_v[0]],
            vertex_positions[c_v[1]],
            vertex_positions[c_v[2]],
            vertex_positions[c_v[3]]);

        // NOTE: Scaling each vertex value by the volume of its dual cell gives an integrated dual
        // 3-form
        result += c_val * c_vol;
        volume += c_vol;
    }

    // NOTE: Result is a dual 3-form integrated over all vertex dual cells. Caller can divide by the
    // returned volume to convert back to primal 0-form.
    return result;
}

/// Returns the integral of a function defined on vertices of a discretized surface
template <typename Real, typename Value, typename Index>
Value integrate_vertex_func(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Value const> const& vertex_values,
    Span<Vec3<Index> const> const& face_vertices,
    Real& area)
{
    Value result{};
    area = Real{0.0};

    for (isize f = 0; f < face_vertices.size(); ++f)
    {
        auto const& f_v = face_vertices[f];
        constexpr Real inv3{1.0 / 3.0};

        // clang-format off
        Value const f_val = inv3 * (
            vertex_values[f_v[0]] + 
            vertex_values[f_v[1]] + 
            vertex_values[f_v[2]]);

        Real const f_area = vector_area(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]]).norm();
        // clang-format on

        // NOTE: Scaling each vertex value by the area of its dual cell gives an integrated dual
        // 2-form
        result += f_val * f_area;
        area += f_area;
    }

    // NOTE: Result is a dual 2-form integrated over all vertex dual cells. Caller can divide by the
    // returned area to convert back to primal 0-form.
    return result;
}

/// Returns the integral of a function defined on vertices of a discretized curve
template <typename Real, typename Value, typename Index>
Value integrate_vertex_func(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Value const> const& vertex_values,
    Span<Vec2<Index> const> const& edge_vertices,
    Real& length)
{
    Value result{};
    length = Real{0.0};

    for (isize e = 0; e < edge_vertices.size(); ++e)
    {
        auto const& e_v = edge_vertices[e];

        Value const e_val = Real{0.5} * (vertex_values[e_v[0]] + vertex_values[e_v[1]]);
        Real const e_len = (vertex_positions[e_v[0]] - vertex_positions[e_v[1]]).norm();

        // NOTE: Scaling each vertex value by the length of its dual cell gives an integrated dual
        // 1-form
        result += e_val * e_len;
        length += e_len;
    }

    // NOTE: Result is a dual 1-form integrated over all vertex dual cells. Caller can divide by the
    // returned length to convert back to primal 0-form.
    return result;
}

/// Computes the area-weighted centroid of a discretized surface
template <typename Real, typename Index>
Vec3<Real> area_centroid(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices)
{
    Real area{};
    Vec3<Real> const sum = integrate_vertex_func(
        vertex_positions,
        vertex_positions,
        face_vertices,
        area);

    return sum / area;
}

/// Computes the length-weighted centroid of a discretized curve
template <typename Real, typename Index>
Vec3<Real> length_centroid(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec2<Index> const> const& edge_vertices)
{
    Real length{};
    Vec3<Real> const sum = integrate_vertex_func(
        vertex_positions,
        vertex_positions,
        edge_vertices,
        length);

    return sum / length;
}

/// Returns the winding number of a triangle mesh at a sample point
template <typename Real, typename Index>
Real winding_number(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec3<Index> const> const& face_vertices,
    Vec3<Real> const& point)
{
    Real sum{0.0};

    for (auto const& f_v : face_vertices)
    {
        sum += solid_angle(
            vertex_positions[f_v[0]],
            vertex_positions[f_v[1]],
            vertex_positions[f_v[2]],
            point);
    }

    return sum * (0.25 * inv_pi<Real>);
}

namespace impl
{

template <typename Real, typename Value, typename EvalFace>
Value interpolate_mean_value(EvalFace&& eval_face, isize const num_faces, isize const num_threads)
{
    Value sum{};
    Real weight_sum{};

    if (num_threads > 1)
    {
        bool done = false;

#pragma omp parallel num_threads(num_threads)
        {
            struct
            {
                Value sum{};
                Real weight_sum{};
                bool done = false;
            } local;

#pragma omp for schedule(static)
            for (isize i = 0; i < num_faces; ++i)
            {
                if (!local.done)
                    local.done = eval_face(i, local.sum, local.weight_sum);
            }

#pragma omp critical
            {
                if (!done)
                {
                    if (local.done)
                    {
                        sum = local.sum;
                        weight_sum = local.weight_sum;
                        done = true;
                    }
                    else
                    {
                        sum += local.sum;
                        weight_sum += local.weight_sum;
                    }
                }
            }
        }
    }
    else
    {
        for (isize i = 0; i < num_faces; ++i)
        {
            if (eval_face(i, sum, weight_sum))
                break;
        }
    }

    return sum / weight_sum;
}

} // namespace impl

/// Returns the interpolated value at a point inside a triangle mesh using mean value coordinates
template <typename Real, typename Value, typename Index>
Value interpolate_mean_value_robust(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Value const> const& vertex_values,
    Span<Vec3<Index> const> const& face_vertices,
    Vec3<Real> const& point,
    Real const tolerance = Real{1.0e-5},
    isize const num_threads = 1)
{
    // https://www.cse.wustl.edu/~taoju/research/meanvalue.pdf (section 3.3)

    assert(vertex_positions.size() == vertex_values.size());

    auto const eval_face = [&](isize const index, Value& sum, Real& weight_sum) -> bool //
    {
        Vec3<Index> const& f_v = face_vertices[index];

        Value const& f0 = vertex_values[f_v[0]];
        Value const& f1 = vertex_values[f_v[1]];
        Value const& f2 = vertex_values[f_v[2]];

        Vec3<Real> u0 = vertex_positions[f_v[0]] - point;
        Vec3<Real> u1 = vertex_positions[f_v[1]] - point;
        Vec3<Real> u2 = vertex_positions[f_v[2]] - point;

        Real const d0 = u0.norm();
        Real const d1 = u1.norm();
        Real const d2 = u2.norm();

        // If the point is coincident with any vertex, return the corresponding value
        if (d0 <= tolerance)
        {
            sum = f0;
            weight_sum = Real{1.0};
            return true;
        }
        if (d1 <= tolerance)
        {
            sum = f1;
            weight_sum = Real{1.0};
            return true;
        }
        if (d2 <= tolerance)
        {
            sum = f2;
            weight_sum = Real{1.0};
            return true;
        }

        u0 /= d0;
        u1 /= d1;
        u2 /= d2;

        Real const t0 = acos_safe(u1.dot(u2));
        Real const t1 = acos_safe(u2.dot(u0));
        Real const t2 = acos_safe(u0.dot(u1));

        Real const sin_t0 = std::sin(t0);
        Real const sin_t1 = std::sin(t1);
        Real const sin_t2 = std::sin(t2);

        Real h = Real{0.5} * (t0 + t1 + t2);
        h = clamp(h, Real{0.0}, pi<Real>);

        // Using a fixed epsilon for quantities relating to the unit sphere
        constexpr Real eps = 1.0e-5;

        // NOTE: If angle sum is pi then the point lies on the triangle's interior and we can use
        // barycentric coords
        if (abs(pi<Real> - h) <= eps)
        {
            Real const w0 = sin_t0 * d2 * d1;
            Real const w1 = sin_t1 * d0 * d2;
            Real const w2 = sin_t2 * d1 * d0;

            sum = (w0 * f0 + w1 * f1 + w2 * f2);
            weight_sum = w0 + w1 + w2;
            return true;
        }

        Real const two_sin_h = Real{2.0} * std::sin(h);

        // Cosine of the dihedral angles between planar faces of spherical pyramid
        Real const cos_p0 = (two_sin_h * std::sin(h - t0)) / (sin_t1 * sin_t2) - Real{1.0};
        Real const cos_p1 = (two_sin_h * std::sin(h - t1)) / (sin_t2 * sin_t0) - Real{1.0};
        Real const cos_p2 = (two_sin_h * std::sin(h - t2)) / (sin_t0 * sin_t1) - Real{1.0};

        // Sine of the dihedral angles between planar faces of spherical pyramid
        Real const sin_p0 = sqrt_safe(Real{1.0} - cos_p0 * cos_p0);
        Real const sin_p1 = sqrt_safe(Real{1.0} - cos_p1 * cos_p1);
        Real const sin_p2 = sqrt_safe(Real{1.0} - cos_p2 * cos_p2);

        // NOTE: If point lies outside the triangle but in the same plane then the triangle
        // can be ignored since its projected area onto the unit sphere is zero
        if (sin_p0 > eps && sin_p1 > eps && sin_p2 > eps)
        {
            Real const sign_det_U = sign(mat(u0, u1, u2).determinant());
            Real const w0 = sign_det_U * (t0 - cos_p1 * t2 - cos_p2 * t1) / (d0 * sin_t1 * sin_p2);
            Real const w1 = sign_det_U * (t1 - cos_p2 * t0 - cos_p0 * t2) / (d1 * sin_t2 * sin_p0);
            Real const w2 = sign_det_U * (t2 - cos_p0 * t1 - cos_p1 * t0) / (d2 * sin_t0 * sin_p1);

            sum += w0 * f0 + w1 * f1 + w2 * f2;
            weight_sum += w0 + w1 + w2;
        }

        return false;
    };

    return impl::interpolate_mean_value<Real, Value>(eval_face, face_vertices.size(), num_threads);
}

/// Returns the interpolated value at a point inside a triangle mesh using mean value coordinates.
///
/// Compared to the robust implementation, this version is less expensive but also less numerically
/// stable for non-convex shapes and exteriors of convex shapes. It offers *much* better accuracy
/// for interiors of convex shapes however.
///
template <typename Real, typename Value, typename Index>
Value interpolate_mean_value_naive(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Value const> const& vertex_values,
    Span<Vec3<Index> const> const& face_vertices,
    Vec3<Real> const& point,
    Real const tolerance = Real{1.0e-5},
    isize const num_threads = 1)
{
    // https://www.cse.wustl.edu/~taoju/research/meanvalue.pdf (section 3.2)

    assert(vertex_positions.size() == vertex_values.size());

    auto const eval_face = [&](isize const index, Value& sum, Real& weight_sum) -> bool //
    {
        Vec3<Index> const& f_v = face_vertices[index];

        Vec3<Real> const& p0 = vertex_positions[f_v[0]];
        Vec3<Real> const& p1 = vertex_positions[f_v[1]];
        Vec3<Real> const& p2 = vertex_positions[f_v[2]];

        Value const& f0 = vertex_values[f_v[0]];
        Value const& f1 = vertex_values[f_v[1]];
        Value const& f2 = vertex_values[f_v[2]];

        Vec3<Real> const d0 = p0 - point;
        Vec3<Real> const d1 = p1 - point;
        Vec3<Real> const d2 = p2 - point;

        Vec3<Real> const tri_norm = (p1 - p0).cross(p2 - p0).normalized();
        if (abs(d0.dot(tri_norm)) <= tolerance)
        {
            // Point is on the plane of the triangle
            Vec3<Real> const w = to_barycentric(point, p0, p1, p2);
            if ((w.array() < Real{0.0}).any())
            {
                // Point is outside the triangle, can ignore contribution from triangle
                return false;
            }
            else
            {
                // Point is inside the triangle, can use barycentric interpolation
                sum = w[0] * f0 + w[1] * f1 + w[2] * f2;
                weight_sum = w.sum();
                return true;
            }
        }

        // Normals of planar faces of spherical pyramid
        Vec3<Real> const n0 = d1.cross(d2).normalized();
        Vec3<Real> const n1 = d2.cross(d0).normalized();
        Vec3<Real> const n2 = d0.cross(d1).normalized();

        // Areas of planar faces of spherical pyramid
        Real const a0 = angle(d1, d2);
        Real const a1 = angle(d2, d0);
        Real const a2 = angle(d0, d1);

        // Integral of unit normal over spherical triangle (eqn. 8)
        Vec3<Real> const m = Real{0.5} * (a0 * n0 + a1 * n1 + a2 * n2);

        // Compute weights (eqn. 9)
        Real const w0 = n0.dot(m) / n0.dot(d0);
        Real const w1 = n1.dot(m) / n1.dot(d1);
        Real const w2 = n2.dot(m) / n2.dot(d2);

        // Take weighted sum
        sum += w0 * f0 + w1 * f1 + w2 * f2;
        weight_sum += w0 + w1 + w2;

        return false;
    };

    return impl::interpolate_mean_value<Real, Value>(eval_face, face_vertices.size(), num_threads);
}

} // namespace dr