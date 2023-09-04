#include <utest.h>

#include <dr/basic_types.hpp>
#include <dr/function_ref.hpp>

UTEST(function_ref, function_ptr)
{
    using namespace dr;

    struct Math
    {
        static isize square(isize const x) { return x * x; }
    };

    FunctionRef<isize(isize)> const square_ref{&Math::square};
    ASSERT_EQ(Math::square(2), square_ref(2));
}

UTEST(function_ref, lambda)
{
    using namespace dr;

    // As function ptr
    {
        FunctionRef<isize(isize)> const square_ref{[](isize const x) { return x * x; }};
        ASSERT_EQ(4, square_ref(2));
    }

    // As function object
    {
        auto const square = [](isize const x) { return x * x; };
        FunctionRef<isize(isize)> const square_ref{&square};
        ASSERT_EQ(square(2), square_ref(2));
    }
}

UTEST(function_ref, lambda_closure)
{
    using namespace dr;

    isize const a = 2;
    isize const b = 1;
    auto const affine = [&](isize const x) { return a * x + b; };

    FunctionRef<isize(isize)> const affine_ref{&affine};
    ASSERT_EQ(affine(2), affine_ref(2));
}

UTEST(function_ref, function_object)
{
    using namespace dr;

    struct Affine
    {
        isize a;
        isize b;
        isize operator()(isize const x) const { return a * x + b; }
    };

    Affine const affine{2, 1};
    FunctionRef<isize(isize)> const affine_ref{&affine};
    ASSERT_EQ(affine(2), affine_ref(2));
}

UTEST(function_ref, is_valid)
{
    using namespace dr;

    {
        FunctionRef<f32(f32)> fn{};
        ASSERT_FALSE(fn.is_valid());
    }

    {
        FunctionRef<f32(f32)> fn{nullptr};
        ASSERT_FALSE(fn.is_valid());
    }

    {
        FunctionRef<f32(f32)> fn{[](f32 const x) { return x * x; }};
        ASSERT_TRUE(fn.is_valid());
    }
}
