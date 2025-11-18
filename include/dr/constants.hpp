#pragma once

#include <dr/math_traits.hpp>

namespace dr
{

template <typename Index, std::enable_if_t<is_integer<Index> || is_natural<Index>>* = nullptr>
inline constexpr Index invalid_index{~0};

template <typename T>
inline constexpr bool always_false{false};

} // namespace dr