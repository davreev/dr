#pragma once

/*
    Assorted helper functions for container types
*/

#include <dr/container_traits.hpp>
#include <dr/span.hpp>

namespace dr
{

/// Returns the size of the given c array as the specified type
template <typename Size, typename T, Size n>
constexpr Size size(T const (& /*array*/)[n])
{
    return n;
}

/// Returns the size of the given container as the specified type
template <typename Size, typename Container>
Size size(Container const& container)
{
    return static_cast<Size>(container.size());
}

/// Creates a span from the given pointer and size
template <typename T>
Span<T> as_span(T* const begin, isize const size)
{
    return {begin, size};
}

/// Creates a span over the given c array
template <typename T, isize n>
Span<T> as_span(T (&array)[n])
{
    return {array, n};
}

/// Creates a span from another contiguous container type
template <typename Container, std::enable_if_t<is_contiguous<Container>>* = nullptr>
Span<typename Container::value_type> as_span(Container& container)
{
    return {container.data(), static_cast<isize>(container.size())};
}

/// Creates a span from another contiguous container type
template <typename Container, std::enable_if_t<is_contiguous<Container>>* = nullptr>
Span<const typename Container::value_type> as_span(Container const& container)
{
    return {container.data(), static_cast<isize>(container.size())};
}

/// Deleted to avoid creating a span from a temporary
template <typename Container, std::enable_if_t<is_contiguous<Container>>* = nullptr>
Span<const typename Container::value_type> as_span(Container const&& container) = delete;

} // namespace dr
