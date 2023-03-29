#pragma once

#include <dr/math_types.hpp>

namespace dr
{
namespace impl
{

template <typename T>
struct IsNatural
{
    static constexpr bool value{false};
};

template <>
struct IsNatural<unsigned short int>
{
    static constexpr bool value{true};
};

template <>
struct IsNatural<unsigned int>
{
    static constexpr bool value{true};
};

template <>
struct IsNatural<unsigned long int>
{
    static constexpr bool value{true};
};

template <>
struct IsNatural<unsigned long long int>
{
    static constexpr bool value{true};
};

template <typename T>
struct IsInteger
{
    static constexpr bool value{false};
};

template <>
struct IsInteger<short int>
{
    static constexpr bool value{true};
};

template <>
struct IsInteger<int>
{
    static constexpr bool value{true};
};

template <>
struct IsInteger<long int>
{
    static constexpr bool value{true};
};

template <>
struct IsInteger<long long int>
{
    static constexpr bool value{true};
};

template <typename T>
struct IsReal
{
    static constexpr bool value{false};
};

template <>
struct IsReal<float>
{
    static constexpr bool value{true};
};

template <>
struct IsReal<double>
{
    static constexpr bool value{true};
};

template <>
struct IsReal<long double>
{
    static constexpr bool value{true};
};

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

template <typename T>
struct IsMatrix
{
    static constexpr bool value{std::is_base_of_v<MatBase<T>, T>};
};

template <typename T>
struct IsVector
{
    static constexpr bool value{IsMatrix<T>::value && T::ColsAtCompileTime == 1};
};

template <typename T>
struct IsCovector
{
    static constexpr bool value{IsMatrix<T>::value && T::RowsAtCompileTime == 1};
};

template <typename T, typename U>
struct IsMatrixExpr
{
    static constexpr bool value{
        IsMatrix<T>::value &&
        std::is_same_v<std::decay_t<typename T::EvalReturnType>, U>};
};

} // namespace impl

/// True if T models the set of natural numbers (N) (including zero)
template <typename T>
inline constexpr bool is_natural = impl::IsNatural<std::decay_t<T>>::value;

/// True if T models the set of integers (Z)
template <typename T>
inline constexpr bool is_integer = impl::IsInteger<std::decay_t<T>>::value;

/// True if T models the set of real numbers (R)
template <typename T>
inline constexpr bool is_real = impl::IsReal<std::decay_t<T>>::value;

/// True if T models the set of complex numbers (C)
template <typename T>
inline constexpr bool is_complex = impl::IsComplex<std::decay_t<T>>::value;

/// True if T models the set of quaternions (H)
template <typename T>
inline constexpr bool is_quaternion = impl::IsQuaternion<std::decay_t<T>>::value;

/// True if T derives from MatBase
template <typename T>
inline constexpr bool is_matrix = impl::IsMatrix<std::decay_t<T>>::value;

/// True if T derives from MatBase and has a single column
template <typename T>
inline constexpr bool is_vector = impl::IsVector<std::decay_t<T>>::value;

/// True if T derives from MatBase and has a single row
template <typename T>
inline constexpr bool is_covector = impl::IsCovector<std::decay_t<T>>::value;

/// True if T is a matrix expression which evaluates to U (ignoring cv qualifiers)
template <typename T, typename U>
inline constexpr bool is_matrix_expr = impl::IsMatrixExpr<std::decay_t<T>, std::decay_t<U>>::value;

} // namespace dr