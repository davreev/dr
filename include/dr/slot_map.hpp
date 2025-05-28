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

    SlotMap(Allocator const alloc = {}) : slots_(alloc), free_indices_(alloc) {}

    SlotMap(SlotMap const& other) = delete;
    SlotMap& operator=(SlotMap const& other) = delete;

    /// Returns the allocator used by this container
    Allocator allocator() const { return slots_.get_allocator(); }

    /// Inserts a new item into the map by constructing it in-place. Returns a valid handle to the
    /// new item.
    template <typename... Args>
    Handle insert(Args&&... args)
    {
        if (free_indices_.empty())
        {
            Index const index{slots_.size()};
            constexpr Index version{1};

            // If T is allocator-aware, share this container's allocator
            if constexpr (is_allocator_aware<T>)
                slots_.push_back({T(std::forward<Args>(args)..., allocator()), version, false});
            else
                slots_.push_back({T(std::forward<Args>(args)...), version, false});

            return {index, version};
        }
        else
        {
            Index const index = free_indices_.back();
            free_indices_.pop_back();

            // Use existing slot
            Slot& slot = slots_[index];
            allocator().construct(&slot.item, std::forward<Args>(args)...);

            return {index, slot.version};
        }
    }

    /// Removes the item at the given handle if it's valid. Returns true on success.
    bool remove(Handle const handle)
    {
        constexpr Index max_version = (Index{1} << version_bits) - Index{1};

        if (Slot& slot = slots_[handle.index]; slot.version == handle.version)
        {
            allocator().destroy(&slot.item);

            // Mark slot as free
            slot.is_free = true;

            // Reuse the slot if its version isn't maxed out
            if (++slot.version < max_version)
                free_indices_.push_back(handle.index);

            return true;
        }

        return false;
    }

    /// Returns true if the handle refers to a valid item
    bool is_valid(Handle const handle) { return slots_[handle.index].version == handle.version; }

    /// Returns the item associated with the given handle or null if the handle isn't valid
    T const* operator[](Handle const handle) const
    {
        if (Slot const& slot = slots_[handle.index]; slot.version == handle.version)
            return &slot.item;

        return nullptr;
    }

    /// Returns the item associated with the given handle or null if the handle isn't valid
    T* operator[](Handle const handle) { return const_cast<T*>(std::as_const(*this)[handle]); }

    /// Returns the handle to the item at the given index if one exists. Otherwise, returns an
    /// invalid handle.
    Handle handle_at(Index const index)
    {
        auto const& slot = slots_[index];
        return slot.is_free ? Handle{index, 0} : Handle{index, slot.version};
    }

    /// Returns the number of items in the map
    isize num_items() { return isize(slots_.size() - free_indices_.size()); }

    /// Returns the number of slots in the map
    isize num_slots() { return isize(slots_.size()); }

  private:
    struct Slot
    {
        T item;
        Index version;
        bool is_free;
    };

    DynamicArray<Slot> slots_;
    DynamicArray<Index> free_indices_;
};

}; // namespace dr