#pragma once

#include <type_traits>

#include <dr/math_types.hpp>
#include <dr/meta.hpp>

namespace dr
{
namespace impl
{

inline constexpr TypePack<
    unsigned char,
    unsigned short int,
    unsigned int,
    unsigned long int,
    unsigned long long int>
    natural_types{};

inline constexpr TypePack<signed char, short int, int, long int, long long int> integer_types{};

inline constexpr TypePack<float, double, long double> real_types{};

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

/// True if T models the set of natural numbers (N) (including zero)
template <typename T>
inline constexpr bool is_natural = impl::natural_types.includes<std::decay_t<T>>();

/// True if T models the set of integers (Z)
template <typename T>
inline constexpr bool is_integer = impl::integer_types.includes<std::decay_t<T>>();

/// True if T models the set of real numbers (R)
template <typename T>
inline constexpr bool is_real = impl::real_types.includes<std::decay_t<T>>();

/// True if T models the set of complex numbers (C)
template <typename T>
inline constexpr bool is_complex = impl::IsComplex<std::decay_t<T>>::value;

/// True if T models the set of quaternions (H)
template <typename T>
inline constexpr bool is_quaternion = impl::IsQuaternion<std::decay_t<T>>::value;

} // namespace dr
