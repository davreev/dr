#pragma once

#include <algorithm>

#include <dr/dynamic_array.hpp>
#include <dr/hash_grid.hpp>
#include <dr/math.hpp>
#include <dr/math_traits.hpp>
#include <dr/span.hpp>

namespace dr
{

/// Iteratively gathers points within a given range. Returns true if converged.
template <typename Real, int dim>
bool gather_points(
    Span<Vec<Real, dim>> const& points,
    HashGrid<Real, dim>& grid,
    Real const radius_start,
    Real const radius_end,
    isize const max_iters = 5,
    Allocator const alloc = {})
{
    static_assert(is_real<Real>);

    DynamicArray<i32> found{alloc};

    // Returns true if converged
    auto const step = [&](Real const radius) -> bool {
        constexpr Real rad_scale{8.0};
        grid.set_grid_scale(radius * rad_scale);

        // Insert points
        for (isize i = 0; i < points.size(); ++i)
            grid.insert(points[i], i);

        Real max_sqr_dist{0.0};

        // Set each point to the weighted average of others within range
        for (isize i = 0; i < points.size(); ++i)
        {
            Vec<Real, dim> const p = points[i];

            found.clear();
            grid.find({p.array() - radius, p.array() + radius}, found);

            Vec<Real, dim> p_sum{p};
            Real w_sum = Real{1.0};

            for (i32 const j : found)
            {
                if (j != i)
                {
                    Vec<Real, dim> const p_adj = points[j];
                    Real const dist = (p - p_adj).norm();

                    // TODO(dr): Test different weighting functions
                    // Real const w = ramp(radius, Real{0.5} * radius, dist);
                    Real const w = smooth_step(radius, Real{0.5} * radius, dist);

                    p_sum += p_adj * w;
                    w_sum += w;
                }
            }

            Vec<Real, dim> const p_new = p_sum / w_sum;
            max_sqr_dist = std::max(max_sqr_dist, (p_new - p).squaredNorm());
            points[i] = p_new;
        }

        // Converged if largest move was within tolerance
        constexpr Real rel_tol{1.0e-3};
        Real const tol = radius * rel_tol;
        return max_sqr_dist <= tol * tol;
    };

    Real const t = Real{1.0} / (max_iters - 1);
    for (int i = 0; i < max_iters; ++i)
    {
        Real const radius = lerp(radius_start, radius_end, t * i);
        if (step(radius))
            return true;
    }

    return false;
}

template <typename Real, typename Index, int dim>
void find_unique_points(
    Span<Vec<Real, dim> const> const& points,
    HashGrid<Real, dim>& grid,
    Real const tolerance,
    DynamicArray<Index>& unique_points,
    Span<Index> const& point_to_unique,
    Allocator const alloc = {})
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    constexpr Real tol_scale{8.0};
    grid.set_grid_scale(tolerance * tol_scale);

    unique_points.clear();

    DynamicArray<i32> found{alloc};
    Real const sqr_tol = tolerance * tolerance;
    isize num_unique = 0;

    for (isize i = 0; i < points.size(); ++i)
    {
        Vec<Real, dim> const p = points[i];

        // Search for existing points within range
        grid.find({p.array() - tolerance, p.array() + tolerance}, found);
        Index unique_idx = -1;

        for (i32 const j : found)
        {
            if ((points[j] - p).squaredNorm() <= sqr_tol)
            {
                unique_idx = point_to_unique[j];
                break;
            }
        }

        // If the point is unique, store its index and add it to the grid
        if (unique_idx == -1)
        {
            unique_idx = num_unique++;
            unique_points.push_back(i);
            grid.insert(p, i);
        }

        point_to_unique[i] = unique_idx;
        found.clear();
    }
}

template <typename Real, typename Index, int dim>
void find_unique_points(
    Span<Vec<Real, dim> const> const& points,
    Real const tolerance,
    DynamicArray<Index>& unique_points,
    Span<Index> const& point_to_unique)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    assert(points.size() == point_to_unique.size());

    unique_points.resize(points.size());
    auto const sorted_pts = as_span(unique_points);

    for (isize i = 0; i < points.size(); ++i)
        sorted_pts[i] = i;

    auto const compare = [&](Index const a, Index const b) {
        auto const& pa = points[a];
        auto const& pb = points[b];

        for (int i = 0; i < dim; ++i)
        {
            if (pa[i] < pb[i])
                return true;
            if (pa[i] > pb[i])
                return false;
        }

        return false;
    };

    std::sort(begin(sorted_pts), end(sorted_pts), compare);

    auto const are_equal = [&](Index const a, Index const b) {
        auto const& pa = points[a];
        auto const& pb = points[b];

        for (int i = 0; i < dim; ++i)
        {
            if (abs(pa[i] - pb[i]) > tolerance)
                return false;
        }

        return true;
    };

    // Find unique points and create map from original to unique points
    {
        isize unique_pt = sorted_pts[0];
        isize unique_idx = 0;
        point_to_unique[unique_pt] = 0;

        for (isize i = 1; i < points.size(); ++i)
        {
            isize const pt = sorted_pts[i];

            // If the point is unique, store its index
            if (!are_equal(unique_pt, pt))
                unique_points[++unique_idx] = (unique_pt = pt);

            point_to_unique[pt] = unique_idx;
        }

        isize const num_unique = unique_idx + 1;
        unique_points.erase(unique_points.begin() + num_unique, unique_points.end());
    }
}

template <typename Scalar, typename Index, int dim>
void merge_vertices(
    Span<Vec<Scalar, dim> const> const& vertex_positions,
    Span<Vec3<Index>> const& face_vertices,
    Span<Index const> const& unique_vertices,
    Span<Index const> const& vertex_to_unique,
    Span<Vec<Scalar, dim>> const& vertex_positions_out)
{
    static_assert(is_integer<Index> || is_natural<Index>);

    // Collect unique vertex positions
    {
        assert(vertex_positions_out.size() == unique_vertices.size());

        for (isize i = 0; i < unique_vertices.size(); ++i)
            vertex_positions_out[i] = vertex_positions[unique_vertices[i]];
    }

    // Reindex faces in-place
    for (isize i = 0; i < face_vertices.size(); ++i)
    {
        Vec3<Index>& f_v = face_vertices[i];
        f_v[0] = vertex_to_unique[f_v[0]];
        f_v[1] = vertex_to_unique[f_v[1]];
        f_v[2] = vertex_to_unique[f_v[2]];
    }
}

template <typename Index>
Span<Vec3<Index>> remove_degenerate_faces(Span<Vec3<Index>> const& face_vertices)
{
    isize num_valid = 0;

    for (isize i = 0; i < face_vertices.size(); ++i)
    {
        Vec3<Index> const f_v = face_vertices[i];

        if (f_v[0] != f_v[1] && f_v[1] != f_v[2] && f_v[2] != f_v[0])
        {
            face_vertices[num_valid] = f_v;
            ++num_valid;
        }
    }

    return face_vertices.front(num_valid);
}

} // namespace dr