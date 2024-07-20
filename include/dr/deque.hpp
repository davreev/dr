#pragma once

#include <dr/shim/pmr/deque.hpp>

namespace dr
{

template <typename T>
using Deque = std::pmr::deque<T>;

} // namespace dr