#pragma once

#include <type_traits>

#include <Eigen/Dense>

namespace dr
{
namespace impl
{

template <typename T>
struct IsMat
{
    static constexpr bool value{std::is_base_of_v<Eigen::MatrixBase<T>, T>};
};

template <typename T>
struct IsVec
{
    static constexpr bool value{IsMat<T>::value && T::ColsAtCompileTime == 1};
};

template <typename T>
struct IsCovec
{
    static constexpr bool value{IsMat<T>::value && T::RowsAtCompileTime == 1};
};

} // namespace impl

/// True if T is a matrix type
template <typename T>
inline constexpr bool is_mat = impl::IsMat<std::decay_t<T>>::value;

/// True if T is a matrix type with a single column
template <typename T>
inline constexpr bool is_vec = impl::IsVec<std::decay_t<T>>::value;

/// True if T is a matrix type with a single row
template <typename T>
inline constexpr bool is_covec = impl::IsCovec<std::decay_t<T>>::value;

} // namespace dr
