#pragma once

#include <dr/shim/pmr/vector.hpp>

namespace dr
{

template <typename T>
using DynamicArray = std::pmr::vector<T>;

} // namespace dr