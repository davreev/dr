#pragma once

#include <array>
#include <string>
#include <vector>

namespace dr
{
namespace impl
{

template <typename T>
struct IsContiguous
{
    static constexpr bool value{false};
};

template <typename T, typename Allocator>
struct IsContiguous<std::vector<T, Allocator>>
{
    static constexpr bool value{true};
};

template <typename Allocator>
struct IsContiguous<std::vector<bool, Allocator>>
{
    static constexpr bool value{false};
};

template <typename T, typename Traits, typename Allocator>
struct IsContiguous<std::basic_string<T, Traits, Allocator>>
{
    static constexpr bool value{true};
};

template <typename T, std::size_t size>
struct IsContiguous<std::array<T, size>>
{
    static constexpr bool value{true};
};

} // namespace impl

/// True if T is a container with contiguous memory layout
template <typename T>
inline constexpr bool is_contiguous = impl::IsContiguous<std::decay_t<T>>::value;

} // namespace dr