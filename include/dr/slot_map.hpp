#pragma once

#include <type_traits>
#include <utility>

#include <dr/basic_types.hpp>
#include <dr/container_utils.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/memory.hpp>

namespace dr
{

template <typename T, typename Index = u64, int version_bits = (sizeof(Index) << 2)>
struct SlotMap : AllocatorAware
{
    static constexpr int index_bits = (sizeof(Index) << 3) - version_bits;
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
            items_.emplace_back(make_t(std::forward<Args>(args)...));
            return {static_cast<Index>(items_.size() - 1), 1};
        }
        else
        {
            Index const index = free_indices_.back();
            free_indices_.pop_back();

            // Use existing slot
            Item& item = items_[index];
            item.data = make_t(std::forward<Args>(args)...);

            return {index, item.version};
        }
    }

    /// Removes the item at the given handle if it's valid. Returns true on success.
    bool remove(Handle const handle)
    {
        constexpr Index max_version = (Index{1} << version_bits) - Index{1};

        if (Item& item = items_[handle.index]; item.version == handle.version)
        {
            item.data = make_t();

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
    isize size() { return static_cast<isize>(items_.size()); }

    /// Returns true if the map contains no items
    bool empty() { return items_.empty(); }

  private:
    struct Item
    {
        T data;
        Index version{1};
        Item(T&& data) : data{std::move(data)} {}
    };

    DynamicArray<Item> items_;
    DynamicArray<Index> free_indices_;

    template <typename... Args>
    T make_t(Args&&... args)
    {
        // If T is allocator-aware, use the map's allocator to construct
        if constexpr (is_allocator_aware<T>)
            return T(std::forward<Args>(args)..., allocator());
        else
            return T(std::forward<Args>(args)...);
    }
};

}; // namespace dr