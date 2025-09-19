#pragma once

#include <deque>

namespace dr
{

template <typename T>
using Deque = std::pmr::deque<T>;

} // namespace dr