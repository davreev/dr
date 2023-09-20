#include <utest.h>

#include <dr/basic_types.hpp>
#include <dr/container_utils.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/memory.hpp>

#include "test_utils.hpp"

UTEST(memory, is_aligned)
{
    using namespace dr;

    struct alignas(8) Foo
    {
    };

    u8 const arr[] = {0, 1, 2, 3, 4, 5, 6, 7};

    isize num_aligned = 0;
    for (isize i = 0; i < size(arr); ++i)
    {
        if (is_aligned<Foo>(arr + i))
            ++num_aligned;
    }

    ASSERT_EQ(1, num_aligned);
}

UTEST(memory, as_bytes)
{
    using namespace dr;

    struct Vec3i
    {
        i32 x;
        i32 y;
        i32 z;
    };

    Vec3i const vec{0, 1, 2};
    i32 const arr[]{0, 1, 2};

    ASSERT_TRUE(equal(as_bytes(vec), as<u8>(as_span(arr))));
}

UTEST(memory, scoped_alloc)
{
    using namespace dr;

    DebugMemoryResource mem{};

    {
        using T = i32;
        constexpr usize n = 10;
        ScopedAlloc<alignof(T)> block{sizeof(T) * n, &mem};
        ASSERT_EQ(sizeof(T) * n, mem.bytes_allocated);
    }

    ASSERT_EQ(0u, mem.bytes_allocated);
}

UTEST(memory, allocator_new_propagation)
{
    using namespace dr;

    DebugMemoryResource mem{};
    Allocator alloc{&mem};

    {
        auto ptr = alloc.new_object<DynamicArray<int>>(10, 1);
        ASSERT_TRUE(ptr->get_allocator().resource()->is_equal(mem));
        alloc.delete_object(ptr);
    }

    {
        auto ptr = make_unique<DynamicArray<int>>(alloc, 10, 1);
        ASSERT_TRUE(ptr->get_allocator().resource()->is_equal(mem));
    }

    ASSERT_EQ(0u, mem.bytes_allocated);
}

UTEST(memory, allocator_wink_out)
{
    using namespace dr;

    using Arena = std::pmr::monotonic_buffer_resource;
    u8 buf[1184]{};

    {
        Arena mem{buf, size(buf), std::pmr::null_memory_resource()};
        Allocator alloc{&mem};

        using Pool = DynamicArray<i32>;
        Pool* pools[]{
            alloc.new_object<Pool>(128, 1), // 40 + 16 + 512 = 568
            alloc.new_object<Pool>(64, 1), // 40 + 16 + 256 = 312
            alloc.new_object<Pool>(32, 1), // 40 + 16 + 128 = 184
            alloc.new_object<Pool>(16, 1), // 40 + 16 + 64 = 120
        };

        // This is just to prevent unused variable warnings
        auto const no_op = [](Pool**) {};
        no_op(pools);

        // No need to destroy individual pools here since memory is cleaned up by the Arena
        // destructor. Individual allocations are "magically winked out".
    }

    ASSERT_TRUE(true);
}