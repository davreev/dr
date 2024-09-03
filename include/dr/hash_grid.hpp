#pragma once

#include <type_traits>

#include <dr/container_utils.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/geometry_types.hpp>
#include <dr/hash_map.hpp>
#include <dr/math_types.hpp>
#include <dr/memory.hpp>

namespace dr
{

template <typename Real, int dim>
struct HashGrid : AllocatorAware
{
    using Index = i32;

    HashGrid(Allocator const alloc = {}) : buckets_(alloc) {}

    HashGrid(Real const cell_size, Allocator const alloc = {}) : buckets_(alloc)
    {
        set_cell_size(cell_size);
    }

    HashGrid(HashGrid const& other, Allocator const alloc = {}) :
        buckets_(other.buckets_, alloc),
        cell_size_{other.cell_size_},
        inv_cell_size_{other.inv_cell_size_},
        size_{other.size_},
        version_{other.version_}
    {
    }

    HashGrid(HashGrid&& other) noexcept = default;
    HashGrid& operator=(HashGrid const& other) = default;
    HashGrid& operator=(HashGrid&& other) noexcept = default;

    /// Returns the allocator used by this container
    Allocator allocator() const { return buckets_.get_allocator(); }

    /// Number of values in the grid
    isize size() const { return size_; }

    /// Grid cell size
    Real cell_size() const { return cell_size_; }

    /// Sets the grid cell size. Calling this also clears the grid.
    void set_cell_size(Real const value)
    {
        assert(value > Real{0.0});
        cell_size_ = value;
        inv_cell_size_ = Real{1.0} / value;
        clear();
    }

    /// Removes all values from the grid
    void clear()
    {
        constexpr usize max_version = ~0;

        // NOTE: Buckets are cleared lazily on insertion
        if (version_ < max_version)
        {
            ++version_;
        }
        else
        {
            version_ = 0;

            for (auto& [key, bucket] : buckets_)
            {
                bucket.values.clear();
                bucket.version = 0;
            }
        }

        size_ = 0;
    }

    /// Inserts a value at the given point
    void insert(Vec<Real, dim> const& point, Index const value)
    {
        Bucket& bucket = get_bucket(to_key(point));

        // Clear bucket if outdated
        if (bucket.version != version_)
        {
            bucket.values.clear();
            bucket.version = version_;
        }

        bucket.values.push_back(value);
        ++size_;
    }

    /// Finds all values over the given interval
    template <typename Callback>
    void find(Interval<Real, dim> const& interval, Callback&& callback) const
    {
        static_assert(dim == 2 || dim == 3);

        if constexpr (std::is_invocable_r_v<void, Callback, Index>)
        {
            find_impl(interval, [&](Index const i) -> bool { return (callback(i), true); });
        }
        else
        {
            static_assert(std::is_invocable_r_v<bool, Callback, Index>);
            find_impl(interval, callback);
        }
    }

  private:
    struct Bucket : AllocatorAware
    {
        DynamicArray<Index> values;
        usize version;

        Bucket(Allocator const alloc = {}) : values(alloc) {}

        Bucket(Bucket const& other, Allocator const alloc = {}) :
            values(other.values, alloc), version{other.version}
        {
        }

        Bucket(Bucket&& other) noexcept = default;
        Bucket& operator=(Bucket const& other) = default;
        Bucket& operator=(Bucket&& other) noexcept = default;
    };

    struct KeyHash
    {
        // Impl ref
        // http://matthias-mueller-fischer.ch/publications/tetraederCollision.pdf

        static constexpr usize primes[]{73856093, 19349663, 83492791};

        usize operator()(Vec<Index, 2> const& key) const
        {
            // Multiply each coord with a large prime and xor together
            return static_cast<usize>(key[0]) * primes[0] ^ static_cast<usize>(key[1]) * primes[1];
        }

        usize operator()(Vec<Index, 3> const& key) const
        {
            // Multiply each coord with a large prime and xor together
            return static_cast<usize>(key[0]) * primes[0] ^ static_cast<usize>(key[1]) * primes[1]
                ^ static_cast<usize>(key[2]) * primes[2];
        }
    };

    HashMap<Vec<Index, dim>, Bucket, KeyHash> buckets_;
    Real cell_size_{1.0};
    Real inv_cell_size_{1.0};
    isize size_{};
    usize version_{};

    Vec<Index, dim> to_key(Vec<Real, dim> const& point) const
    {
        return (point.array() * inv_cell_size_).floor().template cast<Index>();
    }

    Bucket& get_bucket(Vec<Index, dim> const& key) { return buckets_[key]; }

    Bucket const* get_bucket(Vec<Index, dim> const& key) const
    {
        auto const it = buckets_.find(key);
        return (it != buckets_.end()) ? &it->second : nullptr;
    }

    template <typename Callback>
    void find_impl(Interval<Real, 2> const& interval, Callback&& callback) const
    {
        Vec<Index, 2> const key_from = to_key(interval.from);
        Vec<Index, 2> const key_to = to_key(interval.to);

        for (Index i = key_from[0]; i <= key_to[0]; ++i)
        {
            for (Index j = key_from[1]; j <= key_to[1]; ++j)
            {
                Bucket const* bucket = get_bucket({i, j});

                // Skip if bucket doesn't exist or is out of date
                if (bucket == nullptr || bucket->version != version_)
                    continue;

                for (auto const& val : bucket->values)
                {
                    // Early out if callback returns false
                    if (!callback(val))
                        return;
                }
            }
        }
    }

    template <typename Callback>
    void find_impl(Interval<Real, 3> const& interval, Callback&& callback) const
    {
        Vec<Index, 3> const key_from = to_key(interval.from);
        Vec<Index, 3> const key_to = to_key(interval.to);

        for (Index i = key_from[0]; i <= key_to[0]; ++i)
        {
            for (Index j = key_from[1]; j <= key_to[1]; ++j)
            {
                for (Index k = key_from[2]; k <= key_to[2]; ++k)
                {
                    Bucket const* bucket = get_bucket({i, j, k});

                    // Skip if bucket doesn't exist or is out of date
                    if (bucket == nullptr || bucket->version != version_)
                        continue;

                    for (auto const& val : bucket->values)
                    {
                        // Early out if callback returns false
                        if (!callback(val))
                            return;
                    }
                }
            }
        }
    }
};

template <typename Real>
using HashGrid2 = HashGrid<Real, 2>;

template <typename Real>
using HashGrid3 = HashGrid<Real, 3>;

} // namespace dr