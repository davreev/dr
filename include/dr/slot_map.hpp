#pragma once

#include <utility>

#include <dr/basic_types.hpp>
#include <dr/container_utils.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/memory.hpp>

namespace dr
{

template <typename T, typename Index = u64, int version_bits = (8 * sizeof(Index) / 2)>
struct SlotMap : AllocatorAware
{
    static constexpr int index_bits = (8 * sizeof(Index)) - version_bits;
    static_assert(index_bits > 0 && version_bits > 0);

    struct Handle
    {
        Index index : index_bits;
        Index version : version_bits;
    };

    SlotMap(Allocator const alloc = {}) : items_(alloc), free_indices_(alloc) {}

    SlotMap(SlotMap const& other) = delete;
    SlotMap& operator=(SlotMap const& other) = delete;

    /// Returns the allocator used by this container
    Allocator allocator() const { return items_.get_allocator(); }

    /// Inserts a new item into the map by constructing it in-place. Returns a valid handle to the
    /// new item.
    template <typename... Args>
    Handle insert(Args&&... args)
    {
        if (free_indices_.empty())
        {
            Index const index{items_.size()};
            constexpr Index version{1};

            // If T is allocator-aware, share this container's allocator
            if constexpr (is_allocator_aware<T>)
                items_.push_back({T(std::forward<Args>(args)..., allocator()), version});
            else
                items_.push_back({T(std::forward<Args>(args)...), version});

            return {index, version};
        }
        else
        {
            Index const index = free_indices_.back();
            free_indices_.pop_back();

            // Use existing slot
            Item& item = items_[index];
            allocator().construct(&item.data, std::forward<Args>(args)...);

            return {index, item.version};
        }
    }

    /// Removes the item at the given handle if it's valid. Returns true on success.
    bool remove(Handle const handle)
    {
        constexpr Index max_version = (Index{1} << version_bits) - Index{1};

        if (Item& item = items_[handle.index]; item.version == handle.version)
        {
            allocator().destroy(&item.data);

            // Reuse the slot if the version isn't maxed out
            if (++item.version < max_version)
                free_indices_.push_back(handle.index);

            return true;
        }

        return false;
    }

    /// Returns true if the handle refers to a valid item
    bool is_valid(Handle const handle) { return (items_[handle.index].version == handle.version); }

    /// Returns the item associated with the given handle or null if the handle isn't valid
    T const* operator[](Handle const handle) const
    {
        if (Item const& item = items_[handle.index]; item.version == handle.version)
            return &item.data;

        return nullptr;
    }

    /// Returns the item associated with the given handle or null if the handle isn't valid
    T* operator[](Handle const handle) { return const_cast<T*>(std::as_const(*this)[handle]); }

    /// Returns the number of items in the map
    isize size() { return isize(items_.size() - free_indices_.size()); }

    /// Returns true if the map contains no items
    bool empty() { return items_.size() == free_indices_.size(); }

  private:
    struct Item
    {
        T data;
        Index version;
    };

    DynamicArray<Item> items_;
    DynamicArray<Index> free_indices_;
};

}; // namespace dr