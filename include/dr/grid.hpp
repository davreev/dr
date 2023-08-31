#pragma once

#include <dr/geometry_types.hpp>
#include <dr/math_types.hpp>

namespace dr
{

template <typename Scalar, int dim>
struct Grid
{
    using Index = isize;

    Vec<Index, dim> count{};
    Vec<Scalar, dim> scale{};
    Vec<Scalar, dim> translate{};

    Vec<Index, dim> stride() const
    {
        Vec<Index, dim> result{};
        result[0] = 1;

        for (int i = 1; i < dim; ++i)
            result[i] = count[i - 1] * result[i - 1];

        return result;
    }

    Index to_index(Vec<Index, dim> const& grid_pt) const
    {
        return grid_pt.dot(stride());
    }

    Vec<Index, dim> to_grid(Index index) const
    {
        auto const stride = this->stride();
        Vec<Index, dim> result{};

        for (int i = dim - 1; i > 0; --i)
        {
            result[i] = index / stride[i];
            index -= result[i] * stride[i];
        }
        result[0] = index;

        return result;
    }

    Vec<Scalar, dim> to_grid(Vec<Scalar, dim> const& world_pt) const
    {
        return (world_pt - translate).array() / scale.array();
    }

    Vec<Scalar, dim> to_world(Vec<Scalar, dim> const& grid_pt) const
    {
        return grid_pt.array() * scale.array() + translate.array();
    }

    Vec<Scalar, dim> to_world(Vec<Index, dim> const& grid_pt) const
    {
        return to_world(grid_pt.template cast<Scalar>().eval());
    }

    Interval<Scalar, dim> bounds() const
    {
        return {
            translate,
            translate.array() + scale.array() * (count.array() - 1).template cast<Scalar>(),
        };
    }
};

template <typename Scalar>
using Grid2 = Grid<Scalar, 2>;

template <typename Scalar>
using Grid3 = Grid<Scalar, 3>;

template <typename Scalar>
using Grid4 = Grid<Scalar, 4>;

} // namespace dr