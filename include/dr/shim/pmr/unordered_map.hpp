#pragma once

#include <unordered_map>

#include <dr/shim/pmr/memory_resource.hpp>

#ifdef PMR_EXPERIMENTAL

namespace std::experimental::pmr
{

template <typename Key, typename Value, typename Hash = hash<Key>, typename Equal = equal_to<Key>>
using unordered_map = std::unordered_map<Key, Value, Hash, Equal, polymorphic_allocator<std::pair<const Key, Value>>>;

} // namespace std::experimental::pmr

#endif