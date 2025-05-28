#include <memory_resource>
#include <utest.h>

#include <dr/defer.hpp>
#include <dr/memory.hpp>
#include <dr/slot_map.hpp>

#include <dr/shim/pmr/string.hpp>

UTEST(slot_map, insert_remove)
{
    using namespace dr;

    using SlotMap = SlotMap<std::string>;
    using Handle = SlotMap::Handle;

    SlotMap map{};
    assert(map.num_items() == 0);
    assert(map.num_slots() == 0);

    Handle h0{};
    ASSERT_EQ(0u, h0.index);
    ASSERT_EQ(0u, h0.version);

    h0 = map.insert("One");
    ASSERT_EQ(0u, h0.index);
    ASSERT_EQ(1u, h0.version);
    ASSERT_TRUE(map.num_items() == 1);
    ASSERT_TRUE(map.num_slots() == 1);
    ASSERT_TRUE(map.is_valid(h0));
    ASSERT_TRUE(map[h0]->compare("One") == 0);

    Handle const h1 = map.insert("Two");
    ASSERT_EQ(1u, h1.index);
    ASSERT_EQ(1u, h1.version);
    ASSERT_TRUE(map.num_items() == 2);
    ASSERT_TRUE(map.num_slots() == 2);
    ASSERT_TRUE(map.is_valid(h1));
    ASSERT_TRUE(map[h1]->compare("Two") == 0);

    Handle const h2 = map.insert("Three");
    ASSERT_EQ(2u, h2.index);
    ASSERT_EQ(1u, h2.version);
    ASSERT_TRUE(map.num_items() == 3);
    ASSERT_TRUE(map.num_slots() == 3);
    ASSERT_TRUE(map.is_valid(h2));
    ASSERT_TRUE(map[h2]->compare("Three") == 0);

    ASSERT_TRUE(map.remove(h1));
    ASSERT_FALSE(map.remove(h1));
    ASSERT_TRUE(map.num_items() == 2);
    ASSERT_TRUE(map.num_slots() == 3);
    ASSERT_FALSE(map.is_valid(h1));
    ASSERT_TRUE(map[h1] == nullptr);

    Handle const h3 = map.insert("Four");
    ASSERT_EQ(1u, h3.index);
    ASSERT_EQ(2u, h3.version);
    ASSERT_TRUE(map.num_items() == 3);
    ASSERT_TRUE(map.num_slots() == 3);
    ASSERT_TRUE(map.is_valid(h3));
    ASSERT_TRUE(map[h3]->compare("Four") == 0);
}

UTEST(slot_map, allocator_propagation)
{
    using namespace dr;

    // Restore default memory resource after test is complete
    auto def_mem = std::pmr::get_default_resource();
    auto _ = defer([=]() {
        std::pmr::set_default_resource(def_mem);
    });

    DebugMemoryResource mem[3]{};
    std::pmr::set_default_resource(&mem[0]);

    // Check propagation on copy/move
    {
        SlotMap<DynamicArray<i32>> src{&mem[1]};
        ASSERT_TRUE(src.allocator().resource()->is_equal(mem[1]));

        {
            // dst should use the given memory resource
            SlotMap<DynamicArray<i32>> dst{src, &mem[2]};
            ASSERT_TRUE(dst.allocator().resource()->is_equal(mem[2]));
        }

        {
            // dst should use the current default memory resource
            SlotMap<DynamicArray<i32>> dst{src};
            ASSERT_TRUE(dst.allocator().resource()->is_equal(mem[0]));
        }

        {
            // dst should use the same memory resource as src
            SlotMap<DynamicArray<i32>> dst{std::move(src)};
            ASSERT_TRUE(dst.allocator().resource()->is_equal(mem[1]));
        }
    }

    // Check propagation to elements
    {
        SlotMap<DynamicArray<i32>> map{&mem[1]};
        using Handle = SlotMap<DynamicArray<i32>>::Handle;

        Handle const h0 = map.insert(1000, 1);
        ASSERT_TRUE(map[h0]->get_allocator().resource()->is_equal(mem[1]));
        map.remove(h0);

        Handle const h1 = map.insert(1000, 2);
        ASSERT_TRUE(map[h1]->get_allocator().resource()->is_equal(mem[1]));

        map.remove(h1);
        ASSERT_TRUE(map[h1] == nullptr);

        Handle const h2 = map.insert(1000, 3);
        ASSERT_TRUE(map[h2]->get_allocator().resource()->is_equal(mem[1]));
    }
}