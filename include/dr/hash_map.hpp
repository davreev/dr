#pragma once

#include <dr/hash.hpp>

namespace dr
{

template <
    typename Key,
    typename Value,
    typename KeyHash = Hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
using HashMap = ankerl::unordered_dense::pmr::map<Key, Value, KeyHash, KeyEqual>;

} // namespace dr