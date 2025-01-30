#pragma once

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

template <typename T, typename U>
struct IsSame
{
    static constexpr bool value = false;
};

template <typename T>
struct IsSame<T, T>
{
    static constexpr bool value = true;
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
    static constexpr bool includes = (impl::IsSame<U, T>::value || ...);

    template <typename... U>
    static constexpr bool includes_all = (includes<U> && ...);

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

    static constexpr isize size = sizeof...(vals);

    static constexpr T array[size]{vals...};

    template <isize index>
    static constexpr T at = array[index];

    template <T val>
    static constexpr bool includes = ((vals == val) || ...);

    template <T... other_vals>
    static constexpr bool includes_all = (includes<other_vals> && ...);
};

} // namespace dr
