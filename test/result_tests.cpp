#include <utest.h>

#include <dr/result.hpp>

UTEST(result, sanity)
{
    using namespace dr;

    enum Error
    {
        Error_None = 0,
        Error_Reason,
        Error_OtherReason,
        _Error_Count,
    };

    {
        constexpr auto expr = []() -> Result<int, Error> { return ErrorResult{Error_Reason}; };
        static_assert(expr().error == Error_Reason);

        auto const [val, err] = expr();
        ASSERT_EQ(0, val);
        ASSERT_EQ(Error_Reason, err);
    }

    {
        constexpr auto expr = []() -> Result<int, Error> { return {1}; };
        static_assert(expr().value == 1);

        auto const [val, err] = expr();
        ASSERT_EQ(1, val);
        ASSERT_EQ(Error_None, err);
    }
}

UTEST(maybe, sanity)
{
    using namespace dr;

    {
        constexpr auto expr = []() -> Maybe<int> { return {}; };
        static_assert(!expr().has_value);

        auto const [val, ok] = expr();
        ASSERT_EQ(0, val);
        ASSERT_EQ(false, ok);
    }

    {
        constexpr auto expr = []() -> Maybe<int> { return 1; };
        static_assert(expr().value == 1);

        auto const [val, ok] = expr();
        ASSERT_EQ(1, val);
        ASSERT_EQ(true, ok);
    }
}
