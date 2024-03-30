#include <utest.h>

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

    ASSERT_TRUE(all_equal(as_bytes(vec), as<u8>(as_span(arr))));
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
