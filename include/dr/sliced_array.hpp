#pragma once

#include <cassert>
#include <initializer_list>

#include <dr/basic_types.hpp>
#include <dr/container_utils.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/memory.hpp>
#include <dr/span.hpp>

namespace dr
{

template <typename T, typename Index = i32>
struct SlicedArray : AllocatorAware
{
    DynamicArray<T> items;
    DynamicArray<Index> slice_ends;

    SlicedArray(Allocator const alloc = {}) :
        items(alloc),
        slice_ends(alloc)
    {
    }

    SlicedArray(SlicedArray const& other, Allocator const alloc = {}) :
        items{other.items, alloc},
        slice_ends{other.slice_ends, alloc}
    {
    }

    SlicedArray(
        std::initializer_list<T> const items,
        std::initializer_list<Index> const slice_ends,
        Allocator const alloc = {}) :
        items(items, alloc),
        slice_ends(slice_ends, alloc)
    {
    }

    SlicedArray(SlicedArray&& other) noexcept = default;
    SlicedArray& operator=(SlicedArray const& other) = default;
    SlicedArray& operator=(SlicedArray&& other) noexcept = default;

    /// Returns the allocator used by this container
    Allocator allocator() const
    {
        return items.get_allocator();
    }

    /// Returns the total number of items
    Index num_items() const { return size<Index>(items); }

    /// Returns the number of slices
    Index num_slices() const { return size<Index>(slice_ends); }

    /// Returns the slice at the given index
    Span<T const> operator[](Index const index) const
    {
        const Range range = slice_range(index);
        assert(is_valid(range));
        return {items.data() + range.start, range.size()};
    }

    /// Returns the slice at the given index
    Span<T> operator[](Index const index)
    {
        Range const range = slice_range(index);
        assert(is_valid(range));
        return {items.data() + range.start, range.size()};
    }

    /// Appends a slice to the back of the array
    void push_back(Span<T const> const& slice)
    {
        items.insert(items.end(), begin(slice), end(slice));
        Index const end = slice_ends.empty() ? slice.size() : slice_ends.back() + slice.size();
        slice_ends.push_back(end);
    }

    /// Appends a slice to the back of the array
    void push_back(Index const size, T const& value = T{})
    {
        items.insert(items.end(), size, value);
        Index const end = slice_ends.empty() ? size : slice_ends.back() + size;
        slice_ends.push_back(end);
    }

    /// Removes a slice from the back of the array
    void pop_back()
    {
        Range const range = slice_range(num_slices() - 1);
        assert(is_valid(range));
        items.erase(items.begin() + range.start, items.begin() + range.end);
        slice_ends.pop_back();
    }

    /// Removes all slices from the array
    void clear()
    {
        items.clear();
        slice_ends.clear();
    }

    /// Reserves storage for the specified number of items and slices
    void reserve(Index const item_capacity, Index const slice_capacity)
    {
        items.reserve(item_capacity);
        slice_ends.reserve(slice_capacity);
    }

  private:
    struct Range
    {
        Index start;
        Index end;
        Index size() const { return end - start; }
    };

    Range slice_range(Index const index) const
    {
        assert(index >= 0 && index < num_slices());
        return {(index > 0) ? slice_ends[index - 1] : 0, slice_ends[index]};
    }

    bool is_valid(Range const& range) const
    {
        return {range.start >= 0 && range.end >= range.start && range.end <= num_items()};
    }
};

} // namespace dr