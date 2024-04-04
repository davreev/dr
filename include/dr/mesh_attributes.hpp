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
    assert(num_threads > 0);

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
Value integrate_vertex_values(
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
Value integrate_vertex_values(
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
Value integrate_vertex_values(
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
    Vec3<Real> const sum =
        integrate_vertex_values(vertex_positions, vertex_positions, face_vertices, area);

    return sum / area;
}

/// Computes the length-weighted centroid of a discretized curve
template <typename Real, typename Index>
Vec3<Real> length_centroid(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Vec2<Index> const> const& edge_vertices)
{
    Real length{};
    Vec3<Real> const sum =
        integrate_vertex_values(vertex_positions, vertex_positions, edge_vertices, length);

    return sum / length;
}

/// Computes the bounding radius of a point cloud with a given centroid
template <typename Real>
Real bounding_radius(Span<Vec3<Real> const> const& points, Vec3<Real> const& centroid)
{
    Real sqr_rad{0.0};

    for (isize i = 0; i < points.size(); ++i)
    {
        auto const& p = points[i];
        sqr_rad = max((centroid - p).squaredNorm(), sqr_rad);
    }

    return std::sqrt(sqr_rad);
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

/// Returns the interpolated value at a point inside a triangle mesh using mean value coordinates
template <typename Real, typename Value, typename Index>
Value interp_mean_value(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Value const> const& vertex_values,
    Span<Vec3<Index> const> const& face_vertices,
    Vec3<Real> const& point,
    Real const distance_tol = Real{1.0e-6},
    Real const angle_tol = Real{1.0e-6})
{
    // https://www.cse.wustl.edu/~taoju/research/meanvalue.pdf

    Value sum{};
    Real weight_sum{};

    for (auto const& f_v : face_vertices)
    {
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
        {
            if (d0 <= distance_tol)
                return f0;

            if (d1 <= distance_tol)
                return f1;

            if (d2 <= distance_tol)
                return f2;
        }

        u0 /= d0;
        u1 /= d1;
        u2 /= d2;

        Real const l0 = (u1 - u2).norm();
        Real const l1 = (u2 - u0).norm();
        Real const l2 = (u0 - u1).norm();

        Real const t0 = Real{2.0} * asin_safe(l0* Real{0.5});
        Real const t1 = Real{2.0} * asin_safe(l1* Real{0.5});
        Real const t2 = Real{2.0} * asin_safe(l2* Real{0.5});

        Real const h = Real{0.5} * (t0 + t1 + t2);
        Real const sin_t0 = std::sin(t0);
        Real const sin_t1 = std::sin(t1);
        Real const sin_t2 = std::sin(t2);

        // If the point lies on the triangle, use barycentric interpolation
        if (abs(pi<Real> - h) <= angle_tol)
        {
            Real const w0 = sin_t0 * d2 * d1;
            Real const w1 = sin_t1 * d0 * d2;
            Real const w2 = sin_t2 * d1 * d0;
            return (w0 * f0 + w1 * f1 + w2 * f2) / (w0 + w1 + w2);
        }
        else
        {
            Real const two_sin_h = Real{2.0} * std::sin(h);
            Real const c0 = (two_sin_h * std::sin(h - t0)) / (sin_t1 * sin_t2) - Real{1.0};
            Real const c1 = (two_sin_h * std::sin(h - t1)) / (sin_t2 * sin_t0) - Real{1.0};
            Real const c2 = (two_sin_h * std::sin(h - t2)) / (sin_t0 * sin_t1) - Real{1.0};

            Real const s0 = sqrt_safe(Real{1.0} - c0 * c0);
            Real const s1 = sqrt_safe(Real{1.0} - c1 * c1);
            Real const s2 = sqrt_safe(Real{1.0} - c2 * c2);

            // If the point lies outside the triangle but in the same plane then the triangle is
            // ignored since its area when projected onto the unit sphere is zero
            if (s0 <= angle_tol || s1 <= angle_tol || s2 <= angle_tol)
                continue;

            Real const sign_det_U = sign(mat(u0, u1, u2).determinant());
            Real const w0 = (t0 - c1 * t2 - c2 * t1) / (d0 * sin_t1 * s2 * sign_det_U);
            Real const w1 = (t1 - c2 * t0 - c0 * t2) / (d1 * sin_t2 * s0 * sign_det_U);
            Real const w2 = (t2 - c0 * t1 - c1 * t0) / (d2 * sin_t0 * s1 * sign_det_U);

            sum += w0 * f0 + w1 * f1 + w2 * f2;
            weight_sum += w0 + w1 + w2;
        }
    }

    return sum / weight_sum;
}

/// Returns the interpolated value at a point inside a triangle mesh using mean value coordinates.
/// The naive implementation is faster but less numerically robust particularly when evaluated near
/// the boundary.
template <typename Real, typename Value, typename Index>
Value interp_mean_value_naive(
    Span<Vec3<Real> const> const& vertex_positions,
    Span<Value const> const& vertex_values,
    Span<Vec3<Index> const> const& face_vertices,
    Vec3<Real> const& point)
{
    // https://www.cse.wustl.edu/~taoju/research/meanvalue.pdf

    Value sum{};
    Real weight_sum{};

    for (auto const& f_v : face_vertices)
    {
        Value const& f0 = vertex_values[f_v[0]];
        Value const& f1 = vertex_values[f_v[1]];
        Value const& f2 = vertex_values[f_v[2]];

        Vec3<Real> const d0 = vertex_positions[f_v[0]] - point;
        Vec3<Real> const d1 = vertex_positions[f_v[1]] - point;
        Vec3<Real> const d2 = vertex_positions[f_v[2]] - point;

        // Wedge unit normals
        Vec3<Real> const n0 = d1.cross(d2).normalized();
        Vec3<Real> const n1 = d2.cross(d0).normalized();
        Vec3<Real> const n2 = d0.cross(d1).normalized();

        // Wedge areas
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
    }

    return sum / weight_sum;
}

} // namespace dr