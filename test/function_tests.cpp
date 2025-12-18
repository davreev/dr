#include <utest.h>

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
    ASSERT_EQ(2u, mem.num_allocs);  // Move shouldn't alloc
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
} // namespace dr