#pragma once

#include <type_traits>

#include <dr/math_types.hpp>

namespace dr
{
namespace impl
{

template <typename T>
struct IsComplex
{
    static constexpr bool value{false};
};

template <typename T>
struct IsComplex<Complex<T>>
{
    static constexpr bool value{true};
};

template <typename T>
struct IsQuaternion
{
    static constexpr bool value{false};
};

template <typename T>
struct IsQuaternion<Quat<T>>
{
    static constexpr bool value{true};
};

} // namespace impl

/// True if T models the set of complex numbers (C)
template <typename T>
inline constexpr bool is_complex = impl::IsComplex<std::decay_t<T>>::value;

/// True if T models the set of quaternions (H)
template <typename T>
inline constexpr bool is_quaternion = impl::IsQuaternion<std::decay_t<T>>::value;

} // namespace dr
