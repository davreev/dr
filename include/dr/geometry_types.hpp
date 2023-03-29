#pragma once

/*
    Fixed-size POD geometry types
*/

#include <dr/math_traits.hpp>
#include <dr/math_types.hpp>


namespace dr
{

template <typename Scalar, int dim>
struct Interval
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    Vec<Scalar, dim> from{};
    Vec<Scalar, dim> to{};
    Vec<Scalar, dim> min() const { return from.array().min(to.array()); }
    Vec<Scalar, dim> max() const { return from.array().max(to.array()); }
    Vec<Scalar, dim> delta() const { return to - from; }
};

template <typename Scalar>
using Interval2 = Interval<Scalar, 2>;

template <typename Scalar>
using Interval3 = Interval<Scalar, 3>;

template <typename Scalar>
using Interval4 = Interval<Scalar, 4>;

template <typename Scalar, int dim>
struct Box
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    Vec<Scalar, dim> center{};
    Vec<Scalar, dim> extent{};
};

template <typename Scalar>
using Box2 = Box<Scalar, 2>;

template <typename Scalar>
using Box3 = Box<Scalar, 3>;

template <typename Scalar>
using Box4 = Box<Scalar, 4>;

template <typename Scalar, int dim>
struct Ball
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    Vec<Scalar, dim> center{};
    Scalar radius{1};
};

template <typename Scalar>
using Ball2 = Ball<Scalar, 2>;

template <typename Scalar>
using Ball3 = Ball<Scalar, 3>;

template <typename Scalar>
using Ball4 = Ball<Scalar, 4>;

template <typename Scalar, int dim>
struct Line
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    Vec<Scalar, dim> start{};
    Vec<Scalar, dim> delta{};
};

template <typename Scalar>
using Line2 = Line<Scalar, 2>;

template <typename Scalar>
using Line3 = Line<Scalar, 3>;

template <typename Scalar>
using Line4 = Line<Scalar, 4>;

template <typename Scalar, int n, int m>
struct Simplex
{
    static_assert(is_real<Scalar> || is_integer<Scalar>);
    Vec<Scalar, n> vertices[m + 1];
    Vec<Scalar, n>& operator[](int const index) { return vertices[index]; }
    Vec<Scalar, n> const& operator[](int const index) const { return vertices[index]; }
};

template <typename Scalar, int dim>
using Simplex1 = Simplex<Scalar, dim, 1>;

template <typename Scalar, int dim>
using Simplex2 = Simplex<Scalar, dim, 2>;

template <typename Scalar, int dim>
using Simplex3 = Simplex<Scalar, dim, 3>;

template <typename Scalar>
using Tri2 = Simplex2<Scalar, 2>;

template <typename Scalar>
using Tri3 = Simplex2<Scalar, 3>;

template <typename Scalar>
using Tri4 = Simplex2<Scalar, 4>;

template <typename Scalar>
using Tet3 = Simplex3<Scalar, 3>;

template <typename Scalar>
using Tet4 = Simplex3<Scalar, 4>;

} // namespace dr