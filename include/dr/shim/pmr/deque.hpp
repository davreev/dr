#pragma once

#include <deque>

#include <dr/shim/pmr/memory_resource.hpp>

#ifdef DR_PMR_EXPERIMENTAL

namespace std::experimental::pmr
{

template <typename T>
using deque = std::deque<T, polymorphic_allocator<T>>;

} // namespace std::experimental::pmr

#endif