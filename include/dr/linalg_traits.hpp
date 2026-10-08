#pragma once

#include <dr/linalg_types.hpp>
#include <dr/meta.hpp>

namespace dr
{
namespace impl
{

template <typename T>
struct MatTraits
{
    static_assert(std::is_base_of_v<MatExpr<T>, T>);

    using Scalar = typename T::Scalar;
    using Value = typename T::PlainObject;

    struct Shape
    {
        static constexpr int rows{T::RowsAtCompileTime};
        static constexpr int cols{T::ColsAtCompileTime};
        static constexpr bool is_dynamic{rows == dynamic_size || cols == dynamic_size};
        static constexpr bool is_static{!is_dynamic};
        static constexpr int size{is_dynamic ? dynamic_size : rows * cols};
        static constexpr int rank{((cols == 1) ? 0 : 1) + ((rows == 1) ? 0 : 1)};
        static constexpr bool is_vec{cols == 1 && (is_dynamic || rows > 1)};
        static constexpr bool is_covec{rows == 1 && (is_dynamic || cols > 1)};
    };
};

} // namespace impl

template <typename T>
using MatTraits = impl::MatTraits<DropCvRef<T>>;

template <typename T>
using MatScalar = typename MatTraits<T>::Scalar;

template <typename T>
using MatValue = typename MatTraits<T>::Value;

template <typename T>
using MatShape = typename MatTraits<T>::Shape;

} // namespace dr
