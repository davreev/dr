#pragma once

#include <type_traits>

#include <dr/basic_types.hpp>
#include <dr/meta.hpp>

namespace dr
{
namespace impl
{

using NaturalTypes = TypePack<u8, u16, u32, u64, usize>;

using IntegerTypes = TypePack<i8, i16, i32, i64, isize>;

using RealTypes = TypePack<f32, f64, f128>;

} // namespace impl

/// True if T models the set of natural numbers (N) (including zero)
template <typename T>
inline constexpr bool is_natural = impl::NaturalTypes::includes<std::decay_t<T>>;

/// True if T models the set of integers (Z)
template <typename T>
inline constexpr bool is_integer = impl::IntegerTypes::includes<std::decay_t<T>>;

/// True if T models the set of real numbers (R)
template <typename T>
inline constexpr bool is_real = impl::RealTypes::includes<std::decay_t<T>>;

} // namespace dr