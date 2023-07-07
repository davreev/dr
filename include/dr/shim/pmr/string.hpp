#pragma once

#include <string>

#include <dr/shim/pmr/memory_resource.hpp>

#ifdef DR_PMR_EXPERIMENTAL

namespace std::experimental::pmr
{

template <typename Char, typename Traits = char_traits<Char>>
using basic_string = std::basic_string<Char, Traits, polymorphic_allocator<Char>>;

using string = basic_string<char>;

} // namespace std::experimental::pmr

#endif