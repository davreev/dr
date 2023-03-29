#pragma once

#include <algorithm>

#include <dr/hash_grid.hpp>
#include <dr/math.hpp>
#include <dr/span.hpp>

#include <dr/shim/pmr/vector.hpp>

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
    std::pmr::vector<i32> found{alloc};

    // Returns true if converged
    auto const step = [&](Real const radius) -> bool {
        constexpr Real rad_scale{8.0};
        grid.set_grid_scale(radius * rad_scale);

        // Insert points
        for (i32 i = 0; i < points.size(); ++i)
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

                    // TODO(dr): Test different weighting functions
                    Real const dist = (p - p_adj).norm();
                    // Real const w = ramp(radius, radius * Real{0.5}, dist);
                    Real const w = smooth_step(radius, radius* Real{0.5}, dist);

                    p_sum += p_adj * w;
                    w_sum += w;
                }
            }

            Vec<Real, dim> const p_new = p_sum / w_sum;
            max_sqr_dist = std::max(max_sqr_dist, (p_new - p).squaredNorm());
            points[i] = p_new;
        }

        // Converged if largest move was within tolerance
        constexpr Real rel_tol{1.0e-3}; // TODO(dr): Add this as a separate arg?
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

template <typename Real, int dim>
void find_unique_points(
    HashGrid<Real, dim>& grid,
    Span<Vec<Real, dim> const> const& points,
    Real const tolerance,
    Span<i32> const& to_unique,
    std::pmr::vector<i32>& from_unique,
    Allocator const alloc = {})
{
    constexpr Real tol_scale{8.0};
    grid.set_grid_scale(tolerance * tol_scale);

    from_unique.clear();

    std::pmr::vector<i32> found{alloc};
    Real const sqr_tol = tolerance * tolerance;
    isize num_unique = 0;

    for (isize i = 0; i < points.size(); ++i)
    {
        Vec<Real, dim> const p = points[i];

        // Search for existing points within range
        grid.find({p.array() - tolerance, p.array() + tolerance}, found);
        i32 unique_index = -1;

        for (i32 const j : found)
        {
            if ((points[j] - p).squaredNorm() <= sqr_tol)
            {
                unique_index = to_unique[j];
                break;
            }
        }

        // If the point is unique, store its index and add it to the grid
        if (unique_index == -1)
        {
            unique_index = num_unique++;
            from_unique.push_back(i);
            grid.insert(p, i);
        }

        to_unique[i] = unique_index;
        found.clear();
    }
}

template <typename Real, int dim>
void find_unique_points(
    Span<Vec<Real, dim> const> const& points,
    Real const tolerance,
    Span<i32> const& to_unique,
    std::pmr::vector<i32>& from_unique)
{
    assert(points.size() == to_unique.size());

    from_unique.resize(points.size());
    Span<i32> const sorted = as_span(from_unique);

    for (isize i = 0; i < to_unique.size(); ++i)
        sorted[i] = i;

    auto const compare = [&](i32 const a, i32 const b) {
        auto const& pa = points[a];
        auto const& pb = points[b];

        for (int i = 0; i < dim; ++i)
        {
            if (pa[i] < pb[i]) return true;
            if (pa[i] > pb[i]) return false;
        }

        return false;
    };

    std::sort(begin(sorted), end(sorted), compare);

    auto const are_equal = [&](i32 const a, i32 const b) {
        auto const& pa = points[a];
        auto const& pb = points[b];

        for (int i = 0; i < dim; ++i)
        {
            if (std::abs(pa[i] - pb[i]) > tolerance)
                return false;
        }

        return true;
    };

    // Use sorted indices to create maps between original and unique points
    {
        to_unique[sorted[0]] = 0;
        from_unique[0] = 0;
        isize unique_index = 0;

        for (isize i = 1; i < to_unique.size(); ++i)
        {
            isize const index = sorted[i];

            if (!are_equal(from_unique[unique_index], index))
                from_unique[++unique_index] = index;

            to_unique[index] = unique_index;
        }

        isize const num_unique = unique_index + 1;
        from_unique.erase(from_unique.begin() + num_unique, from_unique.end());
    }
}

template <typename Real, int dim>
void merge_vertices(
    Span<Vec<Real, dim> const> const& vertex_positions,
    Span<i32 const> const& to_unique,
    Span<i32 const> const& from_unique,
    Span<Vec3<i32>> const& face_vertices,
    Span<Vec<Real, dim>> const& vertex_positions_out)
{
    // Collect unique vertex positions
    {
        assert(vertex_positions_out.size() == from_unique.size());

        for (isize i = 0; i < vertex_positions_out.size(); ++i)
            vertex_positions_out[i] = vertex_positions[from_unique[i]];
    }

    // Reindex faces in-place
    for (isize i = 0; i < face_vertices.size(); ++i)
    {
        Vec3<i32>& f_v = face_vertices[i];
        f_v[0] = to_unique[f_v[0]];
        f_v[1] = to_unique[f_v[1]];
        f_v[2] = to_unique[f_v[2]];
    }
}

void remove_degenerate_faces(Span<Vec3<i32>>& face_vertices);

} // namespace dr