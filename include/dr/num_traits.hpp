#pragma once

#include <dr/meta.hpp>
#include <dr/num_types.hpp>

namespace dr
{

namespace impl
{

using NaturalTypes = TypePack<u8, u16, u32, u64, usize>;
using IntegerTypes = TypePack<i8, i16, i32, i64, isize>;
using RealTypes = TypePack<f32, f64>;
using IndexTypes = IntegerTypes::Join<NaturalTypes>;
using SignedTypes = RealTypes::Join<IntegerTypes>;
using NumberTypes = RealTypes::Join<IntegerTypes>::Join<NaturalTypes>;

} // namespace impl

/// True if T is a built-in numeric type
template <typename T>
inline constexpr bool is_number = impl::NumberTypes::includes<DropCvRef<T>>;

/// True if T is a built-in numeric type that can represent negative values
template <typename T>
inline constexpr bool is_signed = impl::SignedTypes::includes<DropCvRef<T>>;

/// True if T is a built-in numeric type suitable for indexing data structures
template <typename T>
inline constexpr bool is_index = impl::IndexTypes::includes<DropCvRef<T>>;

/// True if T is a built-in numeric type that models the set of natural numbers (N)
template <typename T>
inline constexpr bool is_natural = impl::NaturalTypes::includes<DropCvRef<T>>;

/// True if T is a built-in numeric type that models the set of integers (Z)
template <typename T>
inline constexpr bool is_integer = impl::IntegerTypes::includes<DropCvRef<T>>;

/// True if T is a built-in numeric type that models the set of real numbers (R)
template <typename T>
inline constexpr bool is_real = impl::RealTypes::includes<DropCvRef<T>>;

} // namespace dr