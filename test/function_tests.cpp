#include <utest.h>

#include <dr/basic_types.hpp>
#include <dr/function.hpp>
#include <dr/memory.hpp>

UTEST(function, lambda)
{
    using namespace dr;

    DebugMemoryResource mem{};

    isize count = 0;
    auto const increment = [&](isize const n) {
        return count += n;
    };

    using Fn = Function<isize(isize)>;

    Fn const fn{increment, &mem};
    ASSERT_TRUE(fn.is_valid());
    ASSERT_EQ(1u, mem.num_allocs);
    ASSERT_EQ(1, fn(1));
    ASSERT_EQ(3, fn(2));
    ASSERT_EQ(6, fn(3));

    Fn fn_copy{fn, &mem};
    ASSERT_TRUE(fn.is_valid());
    ASSERT_TRUE(fn_copy.is_valid());
    ASSERT_EQ(2u, mem.num_allocs);
    ASSERT_EQ(10, fn_copy(4));

    Fn const fn_move{std::move(fn_copy)};
    ASSERT_FALSE(fn_copy.is_valid()); // Shouldn't be valid after moving
    ASSERT_TRUE(fn_move.is_valid());
    ASSERT_EQ(2u, mem.num_allocs); // Move shouldn't alloc
    ASSERT_EQ(15, fn_move(5));
}

UTEST(function, function_object)
{
    using namespace dr;

    DebugMemoryResource mem{};

    struct Increment
    {
        isize count = 0;
        isize operator()(isize const n) { return count += n; }
    };

    using Fn = Function<isize(isize)>;

    Fn const fn{Increment{}, &mem};
    ASSERT_TRUE(fn.is_valid());
    ASSERT_EQ(1u, mem.num_allocs);
    ASSERT_EQ(1, fn(1));
    ASSERT_EQ(3, fn(2));
    ASSERT_EQ(6, fn(3));

    Fn fn_copy{fn, &mem};
    ASSERT_TRUE(fn.is_valid());
    ASSERT_TRUE(fn_copy.is_valid());
    ASSERT_EQ(2u, mem.num_allocs);
    ASSERT_EQ(10, fn_copy(4));

    Fn const fn_move{std::move(fn_copy)};
    ASSERT_FALSE(fn_copy.is_valid()); // Shouldn't be valid after moving
    ASSERT_TRUE(fn_move.is_valid());
    ASSERT_EQ(2u, mem.num_allocs); // Move shouldn't alloc
    ASSERT_EQ(15, fn_move(5));
}

UTEST(function, function_ptr)
{
    using namespace dr;

    DebugMemoryResource mem{};

    struct Math
    {
        static isize square(isize const x) { return x * x; }
    };

    using Fn = Function<isize(isize)>;

    Fn const fn{&Math::square, &mem};
    ASSERT_TRUE(fn.is_valid());
    ASSERT_EQ(0u, mem.num_allocs); // Shouldn't allocate when wrapping a function ptr
    ASSERT_EQ(1, fn(1));
    ASSERT_EQ(4, fn(2));
    ASSERT_EQ(9, fn(3));

    Fn fn_copy{fn, &mem};
    ASSERT_EQ(0u, mem.num_allocs);
    ASSERT_EQ(16, fn_copy(4));

    Fn const fn_move{std::move(fn_copy)};
    ASSERT_FALSE(fn_copy.is_valid()); // Shouldn't be valid after moving
    ASSERT_EQ(0u, mem.num_allocs);
    ASSERT_EQ(25, fn_move(5));
}

UTEST(function, assignment)
{
    using namespace dr;

    DebugMemoryResource mem{};
    using Fn = Function<isize(isize)>;

    isize count = 0;
    auto const add = [&](isize const n) {
        return count += n;
    };

    // Move assignment onto a non-empty target
    {
        Fn a{add, &mem};
        Fn b{add, &mem};
        ASSERT_EQ(2u, mem.num_allocs);
        ASSERT_EQ(0u, mem.num_deallocs);

        a = std::move(b);
        ASSERT_TRUE(a.is_valid());
        ASSERT_FALSE(b.is_valid()); // moved-from target is empty
        ASSERT_EQ(2u, mem.num_allocs); // move steals; no new allocation
        ASSERT_EQ(1u, mem.num_deallocs); // a's original object freed, not leaked
    }
    ASSERT_EQ(mem.num_allocs, mem.num_deallocs); // balanced -> no leak
    ASSERT_EQ(0u, mem.bytes_allocated);

    // Copy assignment onto a non-empty target
    {
        Fn a{add, &mem};
        Fn b{add, &mem};
        usize const allocs = mem.num_allocs;
        usize const deallocs = mem.num_deallocs;

        a = b;
        ASSERT_TRUE(a.is_valid());
        ASSERT_TRUE(b.is_valid()); // copy leaves the source intact
        ASSERT_EQ(allocs + 1u, mem.num_allocs); // b cloned into a
        ASSERT_EQ(deallocs + 1u, mem.num_deallocs); // a's original object freed
    }
    ASSERT_EQ(mem.num_allocs, mem.num_deallocs);
    ASSERT_EQ(0u, mem.bytes_allocated);

    // Self assignment (both forms) must be a safe no-op
    {
        Fn a{add, &mem};
        Fn* const self = &a;

        a = *self; // self copy-assignment
        ASSERT_TRUE(a.is_valid());

        a = std::move(*self); // self move-assignment
        ASSERT_TRUE(a.is_valid());

        count = 0;
        ASSERT_EQ(5, a(5)); // still invocable after self-assignment
    }
    ASSERT_EQ(mem.num_allocs, mem.num_deallocs);
    ASSERT_EQ(0u, mem.bytes_allocated);
}

/*
    Compile-time checks
*/

namespace dr
{
namespace
{

[[maybe_unused]]
void check_arg_value_categories()
{
    Function<void(int, int&, int const&, int const&, int&&, int*, int const*)> fn;
    int x = 2;
    int const y = 3;
    fn(1, x, y, 4, 5, &x, &y);
}

} // namespace

/*
    Explicit instantiation of templates to catch compile errors
*/

template struct Function<i32(i32)>;

} // namespace dr