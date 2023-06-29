#pragma once

#include <array>
#include <type_traits>

#include <dr/basic_types.hpp>

namespace dr
{

template <typename... T>
struct TypePack
{
    template <typename U>
    static constexpr bool includes()
    {
        return ((std::is_same_v<U, T>) || ...);
    }

    template <typename... U>
    static constexpr TypePack<T..., U...> join(TypePack<U...> /*other*/)
    {
        return {};
    }

    static constexpr usize size() { return sizeof...(T); }
};

template <typename T, T... vals>
struct ValuePack
{
    template <T val>
    static constexpr bool includes()
    {
        return ((vals == val) || ...);
    }

    template <T... other_vals>
    static constexpr ValuePack<T, vals..., other_vals...> join(ValuePack<T, other_vals...> /*other*/)
    {
        return {};
    }

    static constexpr std::array<T, sizeof...(vals)> array()
    {
        return {vals...};
    }

    static constexpr usize size() { return sizeof...(vals); }
};

} // namespace dr