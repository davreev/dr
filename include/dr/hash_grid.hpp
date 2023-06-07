#pragma once

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

    HashGrid(Allocator const alloc = {}) :
        buckets_(alloc)
    {
    }

    HashGrid(Real const grid_scale, Allocator const alloc = {}) :
        buckets_(alloc)
    {
        set_grid_scale(grid_scale);
    }

    HashGrid(HashGrid const& other, Allocator const alloc = {}) :
        buckets_(other.buckets_, alloc),
        grid_scale_{other.grid_scale_},
        inv_grid_scale_{other.inv_grid_scale_},
        size_{other.size_},
        version_{other.version_}
    {
    }

    HashGrid(HashGrid&& other) noexcept = default;
    HashGrid& operator=(HashGrid const& other) = default;
    HashGrid& operator=(HashGrid&& other) noexcept = default;

    /// Returns the allocator used by this container
    Allocator allocator() const noexcept
    {
        return buckets_.get_allocator();
    }

    /// Number of values in the grid
    isize size() const { return size_; }

    /// Scale of the grid
    Real grid_scale() const { return grid_scale_; }

    /// Sets the scale of the grid. Calling this also clears the grid.
    void set_grid_scale(Real const value)
    {
        assert(value > Real{0.0});
        grid_scale_ = value;
        inv_grid_scale_ = Real{1.0} / value;
        clear();
    }

    /// Clears all buckets in the grid
    void clear()
    {
        constexpr usize max_version = 0xFFFFFFFFFFFFFFFF;

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
    void find(Interval<Real, dim> const& interval, DynamicArray<Index>& result) const
    {
        static_assert(dim == 2 || dim == 3);

        Vec<Index, dim> const key_from = to_key(interval.from);
        Vec<Index, dim> const key_to = to_key(interval.to);

        if constexpr (dim == 2)
        {
            for (Index i = key_from[0]; i <= key_to[0]; ++i)
            {
                for (Index j = key_from[1]; j <= key_to[1]; ++j)
                    find_impl({i, j}, result);
            }
        }
        else if constexpr (dim == 3)
        {
            for (Index i = key_from[0]; i <= key_to[0]; ++i)
            {
                for (Index j = key_from[1]; j <= key_to[1]; ++j)
                {
                    for (Index k = key_from[2]; k <= key_to[2]; ++k)
                        find_impl({i, j, k}, result);
                }
            }
        }
    }

  private:
    struct Bucket : AllocatorAware
    {
        DynamicArray<Index> values;
        usize version;

        Bucket(Allocator const alloc = {}) :
            values(alloc)
        {
        }

        Bucket(Bucket const& other, Allocator const alloc = {}) :
            values(other.values, alloc),
            version{other.version}
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

        usize operator()(Vec<Index, 2> const& key) const
        {
            // Multiply each coord with a large prime and xor together
            return usize{
                static_cast<usize>(key[0]) * 73856093 ^ static_cast<usize>(key[1]) * 19349663};
        }

        usize operator()(Vec<Index, 3> const& key) const
        {
            // Multiply each coord with a large prime and xor together
            return usize{
                static_cast<usize>(key[0]) * 73856093 ^ static_cast<usize>(key[1]) * 19349663 ^ static_cast<usize>(key[2]) * 83492791};
        }
    };

    Vec<Index, dim> to_key(Vec<Real, dim> const& point) const
    {
        return (point.array() * inv_grid_scale_).floor().template cast<Index>();
    }

    Bucket& get_bucket(Vec<Index, dim> const& key)
    {
        return buckets_[key];
    }

    Bucket const* get_bucket(Vec<Index, dim> const& key) const
    {
        auto const it = buckets_.find(key);
        return (it != buckets_.end()) ? &it->second : nullptr;
    }

    void find_impl(Vec<Index, dim> const& key, DynamicArray<Index>& result) const
    {
        Bucket const* bucket = get_bucket(key);

        // Skip if bucket doesn't exist or is out of date
        if (bucket == nullptr || bucket->version != version_)
            return;

        result.insert(result.end(), bucket->values.begin(), bucket->values.end());
    }

    HashMap<Vec<Index, dim>, Bucket, KeyHash> buckets_;
    Real grid_scale_{1.0};
    Real inv_grid_scale_{1.0};
    isize size_{};
    usize version_{};
};

template <typename Real>
using HashGrid2 = HashGrid<Real, 2>;

template <typename Real>
using HashGrid3 = HashGrid<Real, 3>;

} // namespace dr