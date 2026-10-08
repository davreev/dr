#pragma once

#include <dr/num_traits.hpp>

namespace dr
{

template <typename Index, std::enable_if_t<is_index<Index>>* = nullptr>
inline constexpr Index invalid_index{~0};

template <typename T>
inline constexpr bool always_false{false};

inline constexpr isize dynamic_size = -1;

} // namespace dr