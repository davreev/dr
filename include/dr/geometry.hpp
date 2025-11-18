#pragma once

#include <cmath>

#include <dr/geometry_types.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/math.hpp>
#include <dr/math_types.hpp>
#include <dr/result.hpp>

namespace dr
{

/// Returns the parameter of the closest point on the given line
template <typename Real>
Real nearest_point_line(
    Vec3<Real> const& point,
    Vec3<Real> const& line_start,
    Vec3<Real> const& line_delta)
{
    static_assert(is_real<Real>);
    return line_delta.dot(point - line_start) / line_delta.squaredNorm();
}

/// Returns the parameter of the closest point on the given line segment
template <typename Real>
Real nearest_point_segment(
    Vec3<Real> const& point,
    Vec3<Real> const& seg_start,
    Vec3<Real> const& seg_delta)
{
    static_assert(is_real<Real>);
    return saturate(nearest_point_line(point, seg_start, seg_delta));
}

/// Returns the closest point on the given plane
template <typename Real>
Vec3<Real> nearest_point_plane(
    Vec3<Real> const& point,
    Vec3<Real> const& plane_origin,
    Vec3<Real> const& plane_normal)
{
    static_assert(is_real<Real>);
    return point + project((plane_origin - point).eval(), plane_normal);
}

/// Returns parameters of the closest pair of points on the given lines
template <typename Real>
Vec2<Real> nearest_line_line(
    Vec3<Real> const& a_start,
    Vec3<Real> const& a_delta,
    Vec3<Real> const& b_start,
    Vec3<Real> const& b_delta)
{
    static_assert(is_real<Real>);

    Vec3<Real> const b = b_start - a_start;
    Vec2<Real> const x = solve_least_squares(mat(a_delta, b_delta), b);
    return std::isnan(x[0]) ? Vec2<Real>{} : vec(x[0], -x[1]);
}

/// Returns parameters of the closest pair of points on the given line segment and line
template <typename Real>
Vec2<Real> nearest_segment_line(
    Vec3<Real> const& seg_start,
    Vec3<Real> const& seg_delta,
    Vec3<Real> const& line_start,
    Vec3<Real> const& line_delta)
{
    static_assert(is_real<Real>);

    Vec2<Real> t = nearest_line_line(seg_start, seg_delta, line_start, line_delta);
    t[0] = saturate(t[0]);
    t[1] = nearest_point_line((seg_start + seg_delta * t[0]).eval(), line_start, line_delta);
    return t;
}

/// Returns parameters of the closest pair of points on the given segments
template <typename Real>
Vec2<Real> nearest_segment_segment(
    Vec3<Real> const& a_start,
    Vec3<Real> const& a_delta,
    Vec3<Real> const& b_start,
    Vec3<Real> const& b_delta)
{
    static_assert(is_real<Real>);

    Vec2<Real> t = nearest_line_line(a_start, a_delta, b_start, b_delta);
    t[0] = saturate(t[0]);
    t[1] = saturate(nearest_point_line((a_start + a_delta * t[0]).eval(), b_start, b_delta));
    t[0] = saturate(nearest_point_line((b_start + b_delta * t[1]).eval(), a_start, a_delta));
    return t;
}

/// Returns parameters of the intersection between the given lines
template <typename Real>
Maybe<Vec2<Real>> intersect_line_line(
    Vec2<Real> const& a_start,
    Vec2<Real> const& a_delta,
    Vec2<Real> const& b_start,
    Vec2<Real> const& b_delta)
{
    static_assert(is_real<Real>);

    Vec2<Real> const b = b_start - a_start;
    Vec2<Real> const x = mat(a_delta, b_delta).inverse() * b;

    if (std::isnan(x[0]))
        return {};
    else
        return vec(x[0], -x[1]);
}

/// Returns the line parameter of the intersection with a plane
template <typename Real>
Maybe<Real> intersect_line_plane(
    Vec3<Real> const& line_start,
    Vec3<Real> const& line_delta,
    Vec3<Real> const& plane_origin,
    Vec3<Real> const& plane_normal)
{
    static_assert(is_real<Real>);
    Real const d0 = plane_normal.dot(line_delta);

    if (abs(d0) > Real{0.0})
    {
        Real const d1 = plane_normal.dot(plane_origin - line_start);
        return d1 / d0;
    }

    return {};
}

/// Returns the line parameter of the intersection with a disk
template <typename Real>
Maybe<Real> intersect_line_disk(
    Vec3<Real> const& line_start,
    Vec3<Real> const& line_delta,
    Vec3<Real> const& disk_origin,
    Vec3<Real> const& disk_normal,
    Real const disk_radius)
{
    static_assert(is_real<Real>);

    if (auto t = intersect_line_plane(line_start, line_delta, disk_origin, disk_normal))
    {
        Vec3<Real> const p = line_start + line_delta * t.value();
        if ((p - disk_origin).squaredNorm() <= disk_radius * disk_radius)
            return t;
    }

    return {};
}

/// Returns the line parameters of the intersections with a sphere
template <typename Real>
Maybe<Vec2<Real>> intersect_line_sphere(
    Vec3<Real> const& line_start,
    Vec3<Real> const& line_delta,
    Vec3<Real> const& sphere_origin,
    Real const sphere_radius)
{
    static_assert(is_real<Real>);

    // Find line parameter at closest point to sphere origin
    Vec3<Real> const to_orig = sphere_origin - line_start;
    Real const len_sqr = line_delta.squaredNorm();
    Real const t = to_orig.dot(line_delta) / len_sqr;

    // Solve offset to intersection points
    Real const b_sqr = (to_orig - t * line_delta).squaredNorm();
    Real const c_sqr = sphere_radius * sphere_radius;

    // Does intersection exist?
    if (b_sqr <= c_sqr)
    {
        Real const dt = std::sqrt(c_sqr - b_sqr) / std::sqrt(len_sqr);
        return vec(t - dt, t + dt);
    }

    return {};
}

/// Returns true if the point is inside the triangle
template <typename Real>
bool is_in_tri(
    Vec2<Real> const& point,
    Vec2<Real> const& tri_a,
    Vec2<Real> const& tri_b,
    Vec2<Real> const& tri_c)
{
    static_assert(is_real<Real>);

    Vec2<Real> const d0 = tri_a - point;
    Vec2<Real> const d1 = tri_b - point;
    Vec2<Real> const d2 = tri_c - point;
    Real const dir = cross(d0, d1);
    return cross(d1, d2) * dir >= Real{0.0} && cross(d2, d0) * dir >= Real{0.0};
}

/// Returns true if the point is inside the triangle
template <typename Real>
bool is_in_tri(
    Vec3<Real> const& point,
    Vec3<Real> const& tri_a,
    Vec3<Real> const& tri_b,
    Vec3<Real> const& tri_c)
{
    static_assert(is_real<Real>);

    Vec3<Real> const d0 = tri_a - point;
    Vec3<Real> const d1 = tri_b - point;
    Vec3<Real> const d2 = tri_c - point;
    Vec3<Real> const dir = d0.cross(d1);
    return d1.cross(d2).dot(dir) >= Real{0.0} && d2.cross(d0).dot(dir) >= Real{0.0};
}

/// Returns the line parameter of the intersection with a triangle
template <typename Real>
Maybe<Real> intersect_line_tri(
    Vec3<Real> const& line_start,
    Vec3<Real> const& line_delta,
    Vec3<Real> const& tri_a,
    Vec3<Real> const& tri_b,
    Vec3<Real> const& tri_c)
{
    static_assert(is_real<Real>);
    Vec3<Real> const norm = (tri_b - tri_a).cross(tri_c - tri_a);

    if (auto t = intersect_line_plane(line_start, line_delta, tri_a, norm))
    {
        Vec3<Real> const p = line_start + line_delta * t.value();
        if (is_in_triangle(p, tri_a, tri_b, tri_c))
            return t;
    }

    return {};
}

/// Returns the barycentric coords of a point with respect to a triangle
template <typename Real>
Vec3<Real> to_barycentric(
    Vec2<Real> const& point,
    Vec2<Real> const& tri_a,
    Vec2<Real> const& tri_b,
    Vec2<Real> const& tri_c)
{
    static_assert(is_real<Real>);

    Vec2<Real> const a0 = tri_b - tri_a;
    Vec2<Real> const a1 = tri_c - tri_a;
    Vec2<Real> const b = point - tri_a;
    Vec2<Real> const x = mat(a0, a1).inverse() * b;

    return {Real{1.0} - x.sum(), x[0], x[1]};
}

/// Returns the barycentric coords of the closest point on the plane of a triangle
template <typename Real>
Vec3<Real> to_barycentric(
    Vec3<Real> const& point,
    Vec3<Real> const& tri_a,
    Vec3<Real> const& tri_b,
    Vec3<Real> const& tri_c)
{
    static_assert(is_real<Real>);

    Vec3<Real> const a0 = tri_b - tri_a;
    Vec3<Real> const a1 = tri_c - tri_a;
    Vec3<Real> const b = point - tri_a;
    Vec2<Real> const x = solve_least_squares(mat(a0, a1), b);

    return {Real{1.0} - x.sum(), x[0], x[1]};
}

/// Returns the barycentric coords of a point with respect to a tetrahedron
template <typename Real>
Vec4<Real> to_barycentric(
    Vec3<Real> const& point,
    Vec3<Real> const& tet_a,
    Vec3<Real> const& tet_b,
    Vec3<Real> const& tet_c,
    Vec3<Real> const& tet_d)
{
    static_assert(is_real<Real>);

    Vec3<Real> const a0 = tet_b - tet_a;
    Vec3<Real> const a1 = tet_c - tet_a;
    Vec3<Real> const a2 = tet_d - tet_a;
    Vec3<Real> const b = point - tet_a;
    Vec3<Real> const x = mat(a0, a1, a2).inverse() * b;

    return {Real{1.0} - x.sum(), x[0], x[1], x[2]};
}

/// Returns the solid angle of a triangle as viewed from a point
template <typename Real>
Real solid_angle(
    Vec3<Real> const& tri_a,
    Vec3<Real> const& tri_b,
    Vec3<Real> const& tri_c,
    Vec3<Real> const& point)
{
    // https://www.cs.utah.edu/~ladislav/jacobson13robust/jacobson13robust.html (paper section 4.1)

    static_assert(is_real<Real>);

    Vec3<Real> const a = tri_a - point;
    Vec3<Real> const b = tri_b - point;
    Vec3<Real> const c = tri_c - point;

    Real const len_a = a.norm();
    Real const len_b = b.norm();
    Real const len_c = c.norm();

    Real const x = len_a * len_b * len_c + a.dot(b) * len_c + b.dot(c) * len_a + c.dot(a) * len_b;
    Real const y = mat(a, b, c).determinant();
    return Real{2.0} * std::atan2(y, x);
}

/// Returns the solid angle of a closed polygon as viewed from a point
template <typename Real>
Real solid_angle(Span<Vec3<Real> const> const& polygon, Vec3<Real> const& point)
{
    // Computed as the area of a spherical polygon (https://math.stackexchange.com/a/3643176/809910)

    static_assert(is_real<Real>);

    isize const n = polygon.size();
    if (n < 3)
        return Real{0.0};

    auto const to_sphere = [&](Vec3<Real> const& p) {
        return (p - point).normalized();
    };
    Vec3<Real> p0 = to_sphere(polygon[0]);
    Vec3<Real> p1 = to_sphere(polygon[1]);
    Real sum{};

    for (isize i = 0; i < n; ++i)
    {
        Vec3<Real> const p2 = to_sphere(polygon[mod(i + 2, n)]);
        sum += signed_angle<Real>(reject<Real>(p1 - p0, p1), reject<Real>(p2 - p1, p1), p1);
        p0 = p1;
        p1 = p2;
    }

    return Real{2.0} * pi<Real> - sum;
}

/// Returns the vector area of a triangle
template <typename Real>
Vec3<Real> vector_area(Vec3<Real> const& a, Vec3<Real> const& b, Vec3<Real> const& c)
{
    static_assert(is_real<Real>);
    return (b - a).cross(c - b) * Real{0.5};
}

/// Returns the vector area of a quadrilateral. If planar, its area will be the length of the
/// resulting vector.
template <typename Real>
Vec3<Real> vector_area(
    Vec3<Real> const& a,
    Vec3<Real> const& b,
    Vec3<Real> const& c,
    Vec3<Real> const& d)
{
    static_assert(is_real<Real>);
    return (c - a).cross(d - b) * Real{0.5};
}

/// Returns the integrated surface normal over a polygon. If planar, its area will be the length of
/// the resulting vector.
template <typename Real>
Vec3<Real> vector_area(Span<Vec3<Real> const> const& polygon)
{
    static_assert(is_real<Real>);
    isize const n = polygon.size();

    if (n < 3)
    {
        return vec<3, Real>(0.0);
    }
    else if (n == 3)
    {
        return vector_area(polygon[0], polygon[1], polygon[2]);
    }
    else if (n == 4)
    {
        return vector_area(polygon[0], polygon[1], polygon[2], polygon[3]);
    }
    else
    {
        // NOTE: The vector area of any closed surface is zero so by adding up the vector area of
        // the extrusion from the polygon boundary to any point, we're left with the vector area of
        // the polygon.

        auto const eval_edge = [&](isize const i, isize const j) {
            return polygon[i].cross(polygon[j]);
        };

        auto sum = vec<3, Real>(0.0);
        isize const last = n - 1;

        for (isize i = 0; i < last; ++i)
            sum += eval_edge(i, i + 1);

        return (sum + eval_edge(last, 0)) * Real{0.5};
    }
}

/// Returns the signed area of a triangle
template <typename Real>
Real signed_area(Vec2<Real> const& a, Vec2<Real> const& b, Vec2<Real> const& c)
{
    static_assert(is_real<Real>);
    return cross((b - a).eval(), (c - b).eval()) * Real{0.5};
}

/// Returns the signed area of a quadrilateral
template <typename Real>
Real signed_area(Vec2<Real> const& a, Vec2<Real> const& b, Vec2<Real> const& c, Vec2<Real> const& d)
{
    static_assert(is_real<Real>);
    return cross((c - a).eval(), (d - b).eval()) * Real{0.5};
}

/// Returns the signed area of a polygon
template <typename Real>
Real signed_area(Span<Vec2<Real> const> const& polygon)
{
    static_assert(is_real<Real>);
    isize const n = polygon.size();

    if (n < 3)
    {
        return Real{0.0};
    }
    else if (n == 3)
    {
        return signed_area(polygon[0], polygon[1], polygon[2]);
    }
    else if (n == 4)
    {
        return signed_area(polygon[0], polygon[1], polygon[2], polygon[3]);
    }
    else
    {
        auto const eval_edge = [&](isize const i, isize const j) {
            return cross(polygon[i], polygon[j]);
        };

        Real sum{0.0};
        isize const last = n - 1;

        for (isize i = 0; i < last; ++i)
            sum += eval_edge(i, i + 1);

        return (sum + eval_edge(last, 0)) * Real{0.5};
    }
}

/// Returns the signed volume of a tetrahedron
template <typename Real>
Real signed_volume(
    Vec2<Real> const& a,
    Vec2<Real> const& b,
    Vec2<Real> const& c,
    Vec2<Real> const& d)
{
    static_assert(is_real<Real>);

    constexpr Real inv6 = Real{1.0} / Real{6.0};
    return inv6 * mat((b - a).eval(), (c - a).eval(), (d - a).eval()).determinant();
}

/// Evaluates the (constant) gradient of a function defined on vertices of a triangle
template <typename Real>
Covec3<Real> eval_gradient(
    Vec3<Real> const& p0,
    Vec3<Real> const& p1,
    Vec3<Real> const& p2,
    Real const f0,
    Real const f1,
    Real const f2)
{
    static_assert(is_real<Real>);

    // Offset s.t. coord/value at first vertex is zero (no longer contributes to result)
    Vec3<Real> const dp[]{p1 - p0, p2 - p0};
    Real const df[]{f1 - f0, f2 - f0};

    // Compute gradients of linear basis funcs
    Vec3<Real> norm = dp[0].cross(dp[1]);
    norm /= norm.squaredNorm();
    Covec3<Real> const g[]{dp[1].cross(norm), norm.cross(dp[0])};

    // Return linear combo of basis grads
    // | df0  df1 | * | g0 |
    //                | g1 |
    return as_row<2>(df) * as_mat<2>(g);
}

/// Evaluates the (constant) Jacobian of a function defined on vertices of a triangle
template <typename Real, int dim>
Mat<Real, dim, 3> eval_jacobian(
    Vec3<Real> const& p0,
    Vec3<Real> const& p1,
    Vec3<Real> const& p2,
    Vec<Real, dim> const& f0,
    Vec<Real, dim> const& f1,
    Vec<Real, dim> const& f2)
{
    static_assert(is_real<Real> && dim > 0);

    // Offset s.t. coord/value at first vertex is zero (no longer contributes to result)
    Vec3<Real> const dp[]{p1 - p0, p2 - p0};
    Vec<Real, dim> const df[]{f1 - f0, f2 - f0};

    // Compute gradients of linear basis funcs
    Vec3<Real> norm = dp[0].cross(dp[1]);
    norm /= norm.squaredNorm();
    Covec3<Real> const g[]{dp[1].cross(norm), norm.cross(dp[0])};

    // Return linear combos of basis grads as rows
    // | df0  df1 | * | g0 |
    //                | g1 |
    return as_mat<2>(df) * as_mat<2>(g);
}

/// Evaluates the (constant) gradient of a function defined on vertices of a tetrahedron
template <typename Real>
Covec3<Real> eval_gradient(
    Vec3<Real> const& p0,
    Vec3<Real> const& p1,
    Vec3<Real> const& p2,
    Vec3<Real> const& p3,
    Real const f0,
    Real const f1,
    Real const f2,
    Real const f3)
{
    static_assert(is_real<Real>);

    // Offset s.t. coord/value at first vertex is zero (no longer contributes to result)
    Vec3<Real> const dp[]{p1 - p0, p2 - p0, p3 - p0};
    Real const df[]{f1 - f0, f2 - f0, f3 - f0};

    // Compute gradients of linear basis funcs
    Covec3<Real> g[]{
        project(dp[0], dp[1].cross(dp[2])),
        project(dp[1], dp[2].cross(dp[0])),
        project(dp[2], dp[0].cross(dp[1])),
    };
    g[0] /= g[0].squaredNorm();
    g[1] /= g[1].squaredNorm();
    g[2] /= g[2].squaredNorm();

    // Return linear combo of basis grads
    // | df0  df1  df2 | * | g0 |
    //                     | g1 |
    //                     | g2 |
    return as_row<3>(df) * as_mat<3>(g);
}

/// Evaluates the (constant) Jacobian of a function defined on vertices of a tetrahedron
template <typename Real, int dim>
Mat<Real, dim, 3> eval_jacobian(
    Vec3<Real> const& p0,
    Vec3<Real> const& p1,
    Vec3<Real> const& p2,
    Vec3<Real> const& p3,
    Vec<Real, dim> const& f0,
    Vec<Real, dim> const& f1,
    Vec<Real, dim> const& f2,
    Vec<Real, dim> const& f3)
{
    static_assert(is_real<Real>);

    // Offset s.t. coord/value at first vertex is zero (no longer contributes to result)
    Vec3<Real> const dp[]{p1 - p0, p2 - p0, p3 - p0};
    Vec<Real, dim> const df[]{f1 - f0, f2 - f0, f3 - f0};

    // Compute gradients of linear basis funcs
    Covec3<Real> g[]{
        project(dp[0], dp[1].cross(dp[2])),
        project(dp[1], dp[2].cross(dp[0])),
        project(dp[2], dp[0].cross(dp[1])),
    };
    g[0] /= g[0].squaredNorm();
    g[1] /= g[1].squaredNorm();
    g[2] /= g[2].squaredNorm();

    // Return linear combos of basis grads as rows
    // | df0  df1  df2 | * | g0 |
    //                     | g1 |
    //                     | g2 |
    return as_mat<3>(df) * as_mat<3>(g);
}

/// Returns the cotangent weight for each of the given triangle edges
template <typename Real>
Vec3<Real> cotan_weights(Vec3<Real> const& e0, Vec3<Real> const& e1, Vec3<Real> const& e2)
{
    static_assert(is_real<Real>);

    // NOTE: The cotangent weight of an edge is the ratio of its dual to primal length (aka
    // Hodge star). This can be computed by taking half the cotangent of the angle opposite each
    // edge in the triangle.
    //
    // If t is the angle bw vectors u and v then
    //
    // cot(t) = cos(t) / sin(t)
    //        = dot(u, v) / |cross(u, v)|

    return (Real{-0.5} / e0.cross(e1).norm()) * vec(e1.dot(e2), e2.dot(e0), e0.dot(e1));
}

/// Evaluates a single triangle's contributions to the divergence of n vector-valued functions
/// defined on mesh faces. Returns n integrated scalar quantities associated with each vertex.
template <typename Real, int n>
Mat<Real, n, 3> eval_divergence(
    Vec3<Real> const& p0,
    Vec3<Real> const& p1,
    Vec3<Real> const& p2,
    Mat<Real, 3, n> const& f)
{
    static_assert(is_real<Real>);

    // Compute integrated 1-form over each edge
    Vec3<Real> const e[]{p1 - p0, p2 - p1, p0 - p2};
    Mat<Real, n, 3> const fe = f.transpose() * as_mat<3>(e);

    // Apply Hodge star to get dual 1-form
    Vec3<Real> const hodge = cotan_weights(e[0], e[1], e[2]);
    Mat<Real, n, 3> const dual_fe = fe * hodge.asDiagonal();

    // Integrate the dual 1-form over boundary of each vertex dual cell
    return dual_fe - dual_fe({2, 0, 1});
}

namespace impl
{

template <typename Real, int n>
Mat<Real, n, 3> eval_laplacian(
    Vec3<Real> const& p0,
    Vec3<Real> const& p1,
    Vec3<Real> const& p2,
    Mat<Real, n, 3> const& df)
{
    using namespace Eigen::indexing;

    static_assert(is_real<Real>);

    // Apply Hodge star to get dual 1-forms
    Vec3<Real> const hodge = cotan_weights<Real>(p1 - p0, p2 - p1, p0 - p2);
    Mat<Real, n, 3> const dual_df = df * hodge.asDiagonal();

    // Integrate dual 1-forms over boundary of each vertex dual cell
    return dual_df - dual_df(all, {2, 0, 1});
}

} // namespace impl

/// Evaluates a single triangle's contributions to the Laplacian of a scalar function defined on
/// mesh vertices. Returns an integrated scalar quantity associated with each vertex.
template <typename Real>
Mat<Real, 1, 3> eval_laplacian(
    Vec3<Real> const& p0,
    Vec3<Real> const& p1,
    Vec3<Real> const& p2,
    Real const f0,
    Real const f1,
    Real const f2)
{
    // Compute integrated 1-form over each edge
    auto const df = row(f1 - f0, f2 - f1, f0 - f2);
    return impl::eval_laplacian(p0, p1, p2, df);
}

/// Evaluates a single triangle's contributions to the Laplacian of a vector-valued function defined
/// on mesh vertices. Returns an integrated vector quantity associated with each vertex.
template <typename Real, int n>
Mat<Real, n, 3> eval_laplacian(
    Vec3<Real> const& p0,
    Vec3<Real> const& p1,
    Vec3<Real> const& p2,
    Vec<Real, n> const& f0,
    Vec<Real, n> const& f1,
    Vec<Real, n> const& f2)
{
    // Compute integrated 1-form over each edge
    auto const df = mat(col(f1 - f0), col(f2 - f1), col(f0 - f2));
    return impl::eval_laplacian(p0, p1, p2, df);
}

template <typename Scalar, int dim>
Interval<Scalar, dim> interval_union(Interval<Scalar, dim> const& a, Interval<Scalar, dim> const& b)
{
    return {
        a.min().array().min(b.min().array()),
        a.max().array().max(b.max().array()),
    };
}

/// Computes the bounding interval of a point cloud
template <typename Scalar, int dim>
Interval<Scalar, dim> bounding_interval(Span<Vec<Scalar, dim> const> const& points)
{
    auto m = as_mat(points);
    return {m.rowwise().minCoeff(), m.rowwise().maxCoeff()};
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

template <typename Scalar, int dim>
Interval<Scalar, dim> to_interval(Box<Scalar, dim> const& box)
{
    return {box.center - box.extent, box.center + box.extent};
}

template <typename Real, int dim>
Box<Real, dim> to_box(Interval<Real, dim> const& interval)
{
    static_assert(is_real<Real>);
    Vec<Real, dim> const d = interval.delta() * Real{0.5};
    return {interval.from + d, d};
}

} // namespace dr