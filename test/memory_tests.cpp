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

/*
    Compile-time checks
*/

namespace dr
{

/*
    Explicit instantiation of templates to catch compile errors
*/

template bool is_aligned<f64>(void const*);

template f64* as<f64>(void*);
template f64 const* as<f64>(void const*);

template i32* as<i32, f64>(f64*);
template i32 const* as<i32, f64>(f64 const*);

template f64* as<f64, i32>(i32*);
template f64 const* as<f64, i32>(i32 const*);

template Span<i32> as<i32, f64>(Span<f64> const&);
template Span<i32 const> as<i32, f64>(Span<f64 const> const&);

template Span<u8> as_bytes<f64>(f64&);
template Span<u8 const> as_bytes<f64>(f64 const&);

template UniquePtr<f64> make_unique<f64>(Allocator);

} // namespace dr
