#pragma once

#include <array>

#include <dr/basic_types.hpp>

namespace dr
{

template <typename... T>
struct TypePack;

template <typename T, T... vals>
struct ValuePack;

namespace impl
{

template <isize index, typename T>
struct TypeAt;

template <isize index, typename Head, typename... Tail>
struct TypeAt<index, TypePack<Head, Tail...>> : TypeAt<index - 1, TypePack<Tail...>>
{
};

template <typename Head, typename... Tail>
struct TypeAt<0, TypePack<Head, Tail...>>
{
    using Type = Head;
};

template <isize index, typename T>
struct ValueAt;

template <isize index, typename T, T head, T... tail>
struct ValueAt<index, ValuePack<T, head, tail...>> : ValueAt<index - 1, ValuePack<T, tail...>>
{
};

template <typename T, T head, T... tail>
struct ValueAt<0, ValuePack<T, head, tail...>>
{
    static constexpr T value = head;
};

template <typename T, typename U>
struct Join;

template <typename... T, typename... U>
struct Join<TypePack<T...>, TypePack<U...>>
{
    using Type = TypePack<T..., U...>;
};

template <typename T, T... a, T... b>
struct Join<ValuePack<T, a...>, ValuePack<T, b...>>
{
    using Type = ValuePack<T, a..., b...>;
};

} // namespace impl

template <typename... T>
struct TypePack
{
    template <typename... U>
    using Append = TypePack<T..., U...>;

    template <typename... U>
    using Prepend = TypePack<U..., T...>;

    template <typename Other>
    using Join = typename impl::Join<TypePack<T...>, Other>::Type;

    template <isize index>
    using At = typename impl::TypeAt<index, TypePack<T...>>::Type;

    template <typename U>
    static constexpr bool includes = ((std::is_same_v<U, T>) || ...);

    static constexpr isize size = sizeof...(T);
};

template <typename T, T... vals>
struct ValuePack
{
    template <T... more_vals>
    using Append = ValuePack<T, vals..., more_vals...>;

    template <T... more_vals>
    using Prepend = ValuePack<T, more_vals..., vals...>;

    template <typename Other>
    using Join = typename impl::Join<ValuePack<T, vals...>, Other>::Type;

    template <isize index>
    static constexpr T at = impl::ValueAt<index, ValuePack<T, vals...>>::value;

    template <T val>
    static constexpr bool includes = ((vals == val) || ...);

    static constexpr isize size = sizeof...(vals);

    static constexpr std::array<T, sizeof...(vals)> array = {vals...};
};

} // namespace dr
