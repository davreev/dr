#pragma once

#include <ankerl/unordered_dense.h>

#include <dr/basic_types.hpp>
#include <dr/span.hpp>

namespace dr
{

template <typename T, typename Enable = void>
using Hash = ankerl::unordered_dense::hash<T, Enable>;

inline u64 hash(Span<u8 const> const& key)
{
    return ankerl::unordered_dense::detail::wyhash::hash(
        key.data(),
        static_cast<usize>(key.size()));
}

inline u64 hash(u64 const key) { return ankerl::unordered_dense::detail::wyhash::hash(key); }

inline u64 hash_mix(u64 const a, u64 const b)
{
    return ankerl::unordered_dense::detail::wyhash::mix(a, b);
}

} // namespace dr