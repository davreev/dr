#pragma once

#include <dr/math_types.hpp>

namespace dr
{

/// Virtually triangulates a polygon as a fan
template <typename Index>
struct FanTriangulator
{
    FanTriangulator(Index const size) : size_{(assert(size > 2), size)} {}

    /// Returns the current triangle
    Vec3<Index> current() const { return {0, curr_ - 1, curr_}; }

    /// Returns true if the current triangle is valid
    bool is_valid() const { return curr_ < size_; }

    /// Advances to the next triangle in the polygon
    void advance() { ++curr_; }

    /// Advances to the next triangle in the polygon
    void operator++() { advance(); }

  private:
    Index size_;
    Index curr_{2};
};

/// Virtually triangulates a polygon as a strip
template <typename Index>
struct StripTriangulator
{
    StripTriangulator(Index const size) : c_{(assert(size > 2), size - 1)} {}

    /// Returns the current triangle
    Vec3<Index> current() const
    {
        if (a_ == b_)
            return {a_, c_, a_ - 1};
        else
            return {a_, b_, c_};
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
    Index a_{0};
    Index b_{1};
    Index c_;
};

} // namespace dr