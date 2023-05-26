#pragma once

#include <dr/shim/pmr/unordered_map.hpp>

namespace dr
{

template <
    typename Key,
    typename Value,
    typename KeyHash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
using HashMap = std::pmr::unordered_map<Key, Value, KeyHash, KeyEqual>;

} // namespace dr