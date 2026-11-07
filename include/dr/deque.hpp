#pragma once

#include <deque>
#include <memory_resource>

namespace dr
{

template <typename T>
using Deque = std::pmr::deque<T>;

} // namespace dr
