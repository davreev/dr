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
    isize const max_iters = 5)
{
    static_assert(is_real<Real>);

    // Returns true if converged
    auto const step = [&](Real const radius) -> bool {
        constexpr Real rad_scale{8.0};
        grid.set_cell_size(radius * rad_scale);

        // Insert points
        for (isize i = 0; i < points.size(); ++i)
            grid.insert(points[i], i);

        Real max_sqr_dist{0.0};

        // Set each point to the weighted average of others within range
        for (isize i = 0; i < points.size(); ++i)
        {
            Vec<Real, dim> const p = points[i];

            Vec<Real, dim> p_sum{p};
            Real w_sum = Real{1.0};

            grid.find({p.array() - radius, p.array() + radius}, [&](i32 const j) {
                if (j == i)
                    return;

                Vec<Real, dim> const p_adj = points[j];
                Real const dist = (p - p_adj).norm();
                Real const w = smooth_step(radius, Real{0.5} * radius, dist);
                p_sum += p_adj * w;
                w_sum += w;
            });

            Vec<Real, dim> const p_new = p_sum / w_sum;
            max_sqr_dist = max(max_sqr_dist, (p_new - p).squaredNorm());
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

/// Finds unique points based on a given (Euclidean) distance tolerance. Returns the index of each
/// unique point along with the mapping from original points to unique points.
template <typename Real, typename Index, int dim>
void find_unique_points(
    Span<Vec<Real, dim> const> const& points,
    HashGrid<Real, dim>& grid,
    Real const tolerance,
    DynamicArray<Index>& unique_points,
    Span<Index> const& point_to_unique)
{
    static_assert(is_real<Real>);
    static_assert(is_integer<Index> || is_natural<Index>);

    constexpr Real tol_scale{8.0};
    grid.set_cell_size(tolerance * tol_scale);

    unique_points.clear();

    Real const sqr_tol = tolerance * tolerance;
    isize num_unique = 0;

    for (isize i = 0; i < points.size(); ++i)
    {
        Vec<Real, dim> const p = points[i];

        constexpr Index invalid_idx{~0};
        Index unique_idx = invalid_idx;

        // Search for existing points within range
        grid.find({p.array() - tolerance, p.array() + tolerance}, [&](i32 const j) -> bool {
            if ((points[j] - p).squaredNorm() <= sqr_tol)
            {
                unique_idx = point_to_unique[j];
                return false;
            }
            return true;
        });

        // If the point is unique, store its index and add it to the grid
        if (unique_idx == invalid_idx)
        {
            unique_idx = num_unique++;
            unique_points.push_back(i);
            grid.insert(p, i);
        }

        point_to_unique[i] = unique_idx;
    }
}

/// Removes values associated with non-unique vertices from the given array. Returns the truncated
/// array of values associated with unique vertices. To allow for in-place removal, the given array
/// of unique vertices is assumed to be monotonic increasing.
template <typename Scalar, typename Index, int dim>
Span<Vec<Scalar, dim>> merge_vertices(
    Span<Vec<Scalar, dim>> const& vertex_values,
    Span<Index const> const& unique_vertices)
{
    static_assert(is_integer<Index> || is_natural<Index>);

    for (isize i = 0; i < unique_vertices.size(); ++i)
        vertex_values[i] = vertex_values[unique_vertices[i]];

    return vertex_values.front(unique_vertices.size());
}

/// Re-indexes faces of a mesh given a map from old vertex indices to new vertex indices. By
/// default, any degenerate faces created by this process are removed.
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