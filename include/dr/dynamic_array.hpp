#pragma once

#include <vector>

namespace dr
{

template <typename T>
using DynamicArray = std::pmr::vector<T>;

} // namespace dr