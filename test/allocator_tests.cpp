#include <utest.h>

#include <dr/allocator.hpp>
#include <dr/container_utils.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/memory.hpp>

UTEST(allocator, new_propagation)
{
    using namespace dr;

    DebugMemoryResource mem{};
    Allocator alloc{&mem};

    {
        auto ptr = alloc.new_object<DynamicArray<int>>(10, 1);
        ASSERT_TRUE(ptr->get_allocator() == alloc);
        alloc.delete_object(ptr);
    }

    {
        auto ptr = make_unique<DynamicArray<int>>(alloc, 10, 1);
        ASSERT_TRUE(ptr->get_allocator() == alloc);
    }

    ASSERT_EQ(0u, mem.bytes_allocated);
}

UTEST(allocator, wink_out)
{
    using namespace dr;

    using Arena = std::pmr::monotonic_buffer_resource;
    u8 buf[1184]{};

    {
        Arena mem{buf, size(buf), std::pmr::null_memory_resource()};
        Allocator alloc{&mem};

        using Pool = DynamicArray<i32>;
        [[maybe_unused]] Pool* pools[]{
            alloc.new_object<Pool>(128, 1), // 40 + 16 + 512 = 568
            alloc.new_object<Pool>(64, 1), // 40 + 16 + 256 = 312
            alloc.new_object<Pool>(32, 1), // 40 + 16 + 128 = 184
            alloc.new_object<Pool>(16, 1), // 40 + 16 + 64 = 120
        };

        // NOTE: No need to destroy individual pools here since memory is cleaned up by the Arena
        // destructor. Individual allocations are "magically winked out".
    }

    ASSERT_TRUE(true);
}