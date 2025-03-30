#pragma once

#include <type_traits>

#include <Eigen/Dense>

namespace dr
{
namespace impl
{

template <typename T>
struct IsMatrix
{
    static constexpr bool value{std::is_base_of_v<Eigen::MatrixBase<T>, T>};
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

} // namespace impl

/// True if T is a matrix type
template <typename T>
inline constexpr bool is_matrix = impl::IsMatrix<std::decay_t<T>>::value;

/// True if T is a matrix type with a single column
template <typename T>
inline constexpr bool is_vector = impl::IsVector<std::decay_t<T>>::value;

/// True if T is a matrix type with a single row
template <typename T>
inline constexpr bool is_covector = impl::IsCovector<std::decay_t<T>>::value;

} // namespace dr
