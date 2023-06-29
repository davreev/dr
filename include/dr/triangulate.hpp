#pragma once

#include <dr/container_utils.hpp>
#include <dr/math_types.hpp>
#include <dr/span.hpp>

namespace dr
{

/// Virtually triangulates a given polygon as a fan
template <typename Index>
struct FanTriangulator
{
    FanTriangulator(Span<Index const> const& polygon) :
        polygon_{(assert(polygon.size() > 2), polygon)}
    {
    }

    /// Returns the current triangle
    Vec3<Index> current() const { return {polygon_[0], polygon_[curr_ - 1], polygon_[curr_]}; }

    /// Returns true if the current triangle is valid
    bool is_valid() const { return curr_ < polygon_.size(); }

    /// Advances to the next triangle in the polygon
    void advance() { ++curr_; }

    /// Advances to the next triangle in the polygon
    void operator++() { advance(); }

  private:
    Span<Index const> polygon_;
    Index curr_{2};
};

/// Virtually triangulates a given polygon as a strip
template <typename Index>
struct StripTriangulator
{
    StripTriangulator(Span<Index const> const& polygon) :
        polygon_{(assert(polygon.size() > 2), polygon)},
        c_{size_as<Index>(polygon) - 1}
    {
    }

    /// Returns the current triangle
    Vec3<Index> current() const
    {
        if (a_ == b_)
            return {polygon_[a_], polygon_[c_], polygon_[a_ - 1]};
        else
            return {polygon_[a_], polygon_[b_], polygon_[c_]};
    }

    /// Returns true if the current triangle is valid
    bool is_valid() const { return b_ < c_; }

    /// Advances to the next triangle in the polygon
    void advance()
    {
        if (a_ == b_)
            a_ = c_--;
        else
            a_ = ++b_;
    }

    /// Advances to the next triangle in the polygon
    void operator++() { advance(); }

  private:
    Span<Index const> polygon_;
    Index a_{0};
    Index b_{1};
    Index c_;
};

} // namespace dr