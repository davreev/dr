#pragma once

/*
    Assorted helper functions for container types
*/

#include <dr/container_traits.hpp>
#include <dr/span.hpp>

namespace dr
{
namespace impl
{

struct Size
{
    // NOTE: This is implemented as a function object to avoid ADL insanity
    // (https://quuxplusone.github.io/blog/2018/06/17/std-size/)

    /// Returns the number of elements in the given array
    template <typename T, isize n>
    constexpr isize operator()(T (& /*array*/)[n]) const
    {
        return n;
    }

    /// Returns the number of elements in the given container
    template <typename Container>
    constexpr isize operator()(Container&& container) const
    {
        return static_cast<isize>(container.size());
    }
};

} // namespace impl

inline constexpr impl::Size size{};

/// Returns the number of elements in the given array using a specified size type
template <typename Size, typename T, Size n>
constexpr Size size_as(T const (& /*array*/)[n])
{
    return n;
}

/// Returns the number of elements in the given container using a specified size type
template <typename Size, typename Container>
constexpr Size size_as(Container&& container)
{
    return static_cast<Size>(container.size());
}

/// Creates a span from the given pointer and size
template <typename T>
constexpr Span<T> as_span(T* const begin, isize const size)
{
    return {begin, size};
}

/// Creates a span over the given c array
template <typename T, isize n>
constexpr Span<T> as_span(T (&array)[n])
{
    return {array, n};
}

/// Creates a span from another contiguous container type
template <typename Container, std::enable_if_t<is_contiguous<Container>>* = nullptr>
constexpr Span<typename Container::value_type> as_span(Container& container)
{
    return {container.data(), static_cast<isize>(container.size())};
}

/// Creates a span from another contiguous container type
template <typename Container, std::enable_if_t<is_contiguous<Container>>* = nullptr>
constexpr Span<const typename Container::value_type> as_span(Container const& container)
{
    return {container.data(), static_cast<isize>(container.size())};
}

/// Deleted to avoid creating a span from a temporary
template <typename Container, std::enable_if_t<is_contiguous<Container>>* = nullptr>
constexpr Span<const typename Container::value_type> as_span(Container const&& container) = delete;

} // namespace dr
