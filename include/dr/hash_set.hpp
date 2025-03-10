#pragma once

#include <dr/hash.hpp>

namespace dr
{

template <
    typename Key,
    typename KeyHash = Hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
using HashSet = ankerl::unordered_dense::pmr::set<Key, KeyHash, KeyEqual>;

template <
    typename Key,
    typename KeyHash = Hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
using StableHashSet = ankerl::unordered_dense::pmr::segmented_set<Key, KeyHash, KeyEqual>;

} // namespace dr