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
    Span<Index> const sorted_pts = as_span(unique_points);

    for (isize i = 0; i < points.size(); ++i)
        sorted_pts[i] = i;

    auto const compare = [&](Index const a, Index const b) {
        Vec<Real, dim> const& p_a = points[a];
        Vec<Real, dim> const& p_b = points[b];

        for (int i = 0; i < dim; ++i)
        {
            Real const d = p_a[i] - p_b[i];
            if (d < -tolerance)
                return true;
            if (d > tolerance)
                return false;
        }

        return false;
    };

    std::sort(begin(sorted_pts), end(sorted_pts), compare);

    // Create maps to/from unique points
    {
        struct
        {
            Vec<Real, dim> coords_sum;
            isize count;
            isize start;
            isize index;
            Vec<Real, dim> coords() { return coords_sum / count; }
        } unique_pt{};

        auto const assign_unique = [&]() {
            Vec<Real, dim> const cen = unique_pt.coords();
            Real min_dist = tolerance;
            isize nearest_pt = -1;

            // Assign the unique point to each coincident original point. For the reverse map, we
            // use the closest original point to the unique point's center.
            for (isize j = 0; j < unique_pt.count; ++j)
            {
                isize const pt = sorted_pts[unique_pt.start + j];
                point_to_unique[pt] = unique_pt.index;

                Real const dist = (cen - points[pt]).cwiseAbs().maxCoeff();
                if (dist < min_dist)
                {
                    nearest_pt = pt;
                    min_dist = dist;
                }
            }

            unique_points[unique_pt.index] = nearest_pt;
        };

        // Init unique point from first original point
        unique_pt.coords_sum = points[sorted_pts[0]];
        unique_pt.count = 1;

        for (isize i = 1; i < points.size(); ++i)
        {
            Vec<Real, dim> const& pt_coords = points[sorted_pts[i]];

            if (near_equal(pt_coords, unique_pt.coords(), tolerance))
            {
                // Update unique point
                unique_pt.coords_sum += pt_coords;
                ++unique_pt.count;
            }
            else
            {
                // Assign and reset unique point
                assign_unique();
                unique_pt.coords_sum = pt_coords;
                unique_pt.count = 1;
                unique_pt.start = i;
                ++unique_pt.index;
            }
        }

        // Assign last unique point and trim the array
        assign_unique();
        unique_points.erase(unique_points.begin() + unique_pt.index + 1, unique_points.end());
    }
}

template <typename Scalar, typename Index, int dim>
void merge_vertices(
    Span<Vec<Scalar, dim> const> const& vertex_positions,
    Span<Index const> const& unique_vertices,
    Span<Vec<Scalar, dim>> const& vertex_positions_out)
{
    static_assert(is_integer<Index> || is_natural<Index>);
    assert(vertex_positions_out.size() == unique_vertices.size());

    for (isize i = 0; i < unique_vertices.size(); ++i)
        vertex_positions_out[i] = vertex_positions[unique_vertices[i]];
}

template <typename Scalar, typename Index, int dim>
Span<Vec<Scalar, dim>> merge_vertices(
    Span<Vec<Scalar, dim>> const& vertex_positions,
    Span<Index const> const& unique_vertices)
{
    static_assert(is_integer<Index> || is_natural<Index>);

    for (isize i = 0; i < unique_vertices.size(); ++i)
        vertex_positions[i] = vertex_positions[unique_vertices[i]];

    return vertex_positions.front(unique_vertices.size());
}

template <typename Index>
Span<Vec3<Index>> reindex_faces(
    Span<Vec3<Index>> const& face_vertices,
    Span<Index const> const& vertex_new_indices,
    bool const remove_degenerate = true)
{
    static_assert(is_integer<Index> || is_natural<Index>);
    isize num_valid = 0;

    for (isize i = 0; i < face_vertices.size(); ++i)
    {
        Vec3<Index>& f_v = face_vertices[i];
        f_v[0] = vertex_new_indices[f_v[0]];
        f_v[1] = vertex_new_indices[f_v[1]];
        f_v[2] = vertex_new_indices[f_v[2]];

        if (remove_degenerate)
        {
            if (f_v[0] != f_v[1] && f_v[1] != f_v[2] && f_v[2] != f_v[0])
            {
                face_vertices[num_valid] = f_v;
                ++num_valid;
            }
        }
        else
        {
            ++num_valid;
        }
    }

    return face_vertices.front(num_valid);
}

} // namespace dr