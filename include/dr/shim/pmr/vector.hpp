#pragma once

#include <vector>

#include <dr/shim/pmr/memory_resource.hpp>

#ifdef PMR_EXPERIMENTAL

namespace std::experimental::pmr
{

template <typename T>
using vector = std::vector<T, polymorphic_allocator<T>>;

} // namespace std::experimental::pmr

#endif