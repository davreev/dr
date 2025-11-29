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

    SlotMap(SlotMap const& other, Allocator const alloc = {}) :
        slots_(other.slots_, alloc), free_indices_(other.free_indices_, alloc)
    {
    }

    SlotMap(SlotMap&& other) noexcept = default;
    SlotMap& operator=(SlotMap const& other) = default;
    SlotMap& operator=(SlotMap&& other) noexcept = default;

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
            slots_.push_back({make_item(std::forward<Args>(args)...), {version, 0}});
            return {index, version};
        }
        else
        {
            Index const index = free_indices_.back();
            free_indices_.pop_back();

            // Use existing slot
            Slot& slot = slots_[index];
            slot.item = make_item(std::forward<Args>(args)...);

            return {index, slot.status.version};
        }
    }

    /// Removes the item at the given handle if it's valid. Returns true on success.
    bool remove(Handle const handle)
    {
        constexpr Index max_version = (Index{1} << version_bits) - Index{1};

        if (Slot& slot = slots_[handle.index]; slot.status.version == handle.version)
        {
            // Reset the slot's item
            slot.item = make_item();

            // Mark slot as free
            slot.status.flags |= Flag_Free;

            // Reuse the slot if its version isn't maxed out
            if (++slot.status.version < max_version)
                free_indices_.push_back(handle.index);

            return true;
        }

        return false;
    }

    /// Returns true if the handle refers to a valid item
    bool is_valid(Handle const handle)
    {
        return slots_[handle.index].status.version == handle.version;
    }

    /// Returns the item associated with the given handle or null if the handle isn't valid
    T const* operator[](Handle const handle) const
    {
        if (Slot const& slot = slots_[handle.index]; slot.status.version == handle.version)
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
        return (slot.status.flags & Flag_Free) //
            ? Handle{index, 0}
            : Handle{index, slot.status.version};
    }

    /// Returns the number of items in the map
    isize num_items() { return isize(slots_.size() - free_indices_.size()); }

    /// Returns the number of slots in the map
    isize num_slots() { return isize(slots_.size()); }

  private:
    static constexpr auto flag_bits = index_bits;

    struct Slot
    {
        T item;
        struct
        {
            Index version : version_bits;
            Index flags : flag_bits;
        } status;
    };

    enum Flag : u8
    {
        Flag_Free = 1,
        // ...
    };

    template <typename... Args>
    T make_item(Args&&... args)
    {
        // If T is allocator-aware, share this container's allocator
        if constexpr (is_allocator_aware<T>)
            return T(std::forward<Args>(args)..., allocator());
        else
            return T(std::forward<Args>(args)...);
    }

    DynamicArray<Slot> slots_;
    DynamicArray<Index> free_indices_;
};

}; // namespace dr