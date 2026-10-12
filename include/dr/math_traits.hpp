#pragma once

#include <dr/math_types.hpp>
#include <dr/num_traits.hpp>

namespace dr
{

template <typename T>
struct CustomNumTraits<Complex<T>> : DefaultNumTraits<Complex<T>>
{
    static constexpr bool is_complex{true};
};

} // namespace dr