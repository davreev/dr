#pragma once

#include <cassert>

#include <dr/basic_types.hpp>
#include <dr/container_utils.hpp>
#include <dr/memory.hpp>
#include <dr/span.hpp>

#include <dr/shim/pmr/vector.hpp>

namespace dr
{

template <typename T, typename Index = i32>
struct JaggedArray : AllocatorAware
{
    std::pmr::vector<T> items{};
    std::pmr::vector<Index> block_ends{};

    JaggedArray(Allocator const alloc = {}) :
        items{alloc},
        block_ends{alloc}
    {
    }

    JaggedArray(JaggedArray const& other, Allocator const alloc = {}) :
        items{other.items, alloc},
        block_ends{other.block_ends, alloc}
    {
    }

    JaggedArray(JaggedArray&& other) noexcept = default;
    JaggedArray& operator=(JaggedArray&& other) noexcept = default;

    /// Returns the number of items in the array
    Index num_items() const { return size<Index>(items); }

    /// Returns the number of blocks in the array
    Index num_blocks() const { return size<Index>(block_ends); }

    /// Returns the block at the given index
    Span<T const> operator[](Index const index) const
    {
        const Range range = block_range(index);
        assert(is_valid(range));
        return {items.data() + range.start, range.size()};
    }

    /// Returns the block at the given index
    Span<T> operator[](Index const index)
    {
        Range const range = block_range(index);
        assert(is_valid(range));
        return {items.data() + range.start, range.size()};
    }

    /// Appends a block to the back of the array
    void push_back(Span<T const> const& block)
    {
        items.insert(items.end(), begin(block), end(block));
        Index const end = block_ends.empty() ? block.size() : block_ends.back() + block.size();
        block_ends.push_back(end);
    }

    /// Appends a block to the back of the array
    void push_back(Index const size, T const& value = T{})
    {
        items.insert(items.end(), size, value);
        Index const end = block_ends.empty() ? size : block_ends.back() + size;
        block_ends.push_back(end);
    }

    /// Removes a block from the back of the array
    void pop_back()
    {
        Range const range = block_range(num_blocks() - 1);
        assert(is_valid(range));
        items.erase(items.begin() + range.start, items.begin() + range.end);
        block_ends.pop_back();
    }

    /// Removes all blocks from the array
    void clear()
    {
        items.clear();
        block_ends.clear();
    }

    /// Reserves storage for the specified number of items and blocks
    void reserve(Index const items_capacity, Index const block_capacity)
    {
        items.reserve(items_capacity);
        block_ends.reserve(block_capacity);
    }

    /// Returns the allocator used by this container
    Allocator allocator() const noexcept
    {
        return items.get_allocator();
    }

  private:
    struct Range
    {
        Index start;
        Index end;
        Index size() const { return end - start; }
    };

    Range block_range(Index const index) const
    {
        assert(index >= 0 && index < num_blocks());
        return {(index > 0) ? block_ends[index - 1] : 0, block_ends[index]};
    }

    bool is_valid(Range const& range) const
    {
        return {range.start >= 0 && range.end >= range.start && range.end <= num_items()};
    }
};

} // namespace dr