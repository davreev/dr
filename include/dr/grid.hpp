#pragma once

#include <dr/math_types.hpp>

namespace dr
{

template <typename Index, int dim>
Vec<Index, dim> grid_stride(Vec<Index, dim> const& count)
{
    Vec<Index, dim> result{};
    result[0] = 1;

    for (int i = 1; i < dim; ++i)
        result[i] = count[i - 1] * result[i - 1];

    return result;
}

template <typename Index, int dim>
Index grid_to_index(Vec<Index, dim> const& point, Vec<Index, dim> const& stride)
{
    return point.dot(stride);
}

template <typename Index, int dim>
Vec<Index, dim> index_to_grid(Index index, Vec<Index, dim> const& stride)
{
    Vec<Index, dim> result{};

    for (int i = dim - 1; i > 0; --i)
    {
        result[i] = index / stride[i];
        index -= result[i] * stride[i];
    }
    result[0] = index;

    return result;
}

template <typename Scalar, int dim>
struct Grid
{
    Vec<isize, dim> count{};
    Vec<Scalar, dim> spacing{Vec<Scalar, dim>::Ones()};
    Vec<Scalar, dim> origin{};

    Vec<isize, dim> stride() const { return grid_stride(count); }

    isize to_index(Vec<isize, dim> const& grid_pt) const
    {
        return grid_to_index(grid_pt, stride());
    }

    Vec<isize, dim> to_grid(isize const index) const { return index_to_grid(index, stride()); }

    Vec<Scalar, dim> to_grid(Vec<Scalar, dim> const& world_pt) const
    {
        return (world_pt - origin).array() / spacing.array();
    }

    Vec<Scalar, dim> to_world(Vec<Scalar, dim> const& grid_pt) const
    {
        return grid_pt.array() * spacing.array() + origin.array();
    }

    Vec<Scalar, dim> to_world(Vec<isize, dim> const& grid_pt) const
    {
        return to_world(grid_pt.template cast<Scalar>().eval());
    }

    [[deprecated]]
    Vec<Scalar, dim> const& extents() const
    {
        return spacing.array() * (count.array() - 1).template cast<Scalar>();
    }
};

template <typename Scalar>
using Grid2 = Grid<Scalar, 2>;

template <typename Scalar>
using Grid3 = Grid<Scalar, 3>;

template <typename Scalar>
using Grid4 = Grid<Scalar, 4>;

} // namespace dr