#pragma once

#include <dr/meta.hpp>
#include <dr/num_types.hpp>

namespace dr
{

template <typename T>
struct DefaultNumTraits
{
    using BuiltinRealTypes = TypePack<f32, f64>;
    using BuiltinIntegerTypes = TypePack<i8, i16, i32, i64, isize>;
    using BuiltinNaturalTypes = TypePack<u8, u16, u32, u64, usize>;
    using BuiltinTypes = BuiltinRealTypes::Join<BuiltinIntegerTypes>::Join<BuiltinNaturalTypes>;

    /*
        NOTE: Traits identifying numeric domains are *mutually exclusive* - for a given numeric
        type, they specify the domain it's intended to approximate. They are not meant to imply
        exact representation or closure under every operation, and they do not reflect relationships
        implied by the number system hierarchy.
    */

    static constexpr bool is_complex{};
    static constexpr bool is_real{BuiltinRealTypes::includes<T>};
    static constexpr bool is_rational{};
    static constexpr bool is_integer{BuiltinIntegerTypes::includes<T>};
    static constexpr bool is_natural{BuiltinNaturalTypes::includes<T>};
    static constexpr bool is_builtin{BuiltinTypes::includes<T>};
};

template <typename T, typename Enable = void>
struct CustomNumTraits : DefaultNumTraits<T>
{
};

namespace impl
{

template <typename T>
struct NumTraits : CustomNumTraits<T>
{
    using Base = CustomNumTraits<T>;

    static constexpr bool is_signed{Base::is_real || Base::is_rational || Base::is_integer};
    static constexpr bool is_ordered{is_signed || Base::is_natural};
    static constexpr bool is_number{Base::is_complex || Base::is_rational || is_ordered};
    static constexpr bool is_index{(Base::is_integer || Base::is_natural) && Base::is_builtin};
};

} // namespace impl

template <typename T>
using NumTraits = impl::NumTraits<DropCvRef<T>>;

} // namespace dr