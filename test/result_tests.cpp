#include <utest.h>

#include <dr/basic_types.hpp>
#include <dr/result.hpp>

/*
    Compile-time checks
*/

namespace dr
{
namespace
{

[[maybe_unused]]
void check_result_behavior()
{
    enum Error : u8
    {
        Error_None = 0,
        Error_Reason,
        Error_OtherReason,
        _Error_Count,
    };

    {
        constexpr auto expr = []() -> Result<int, Error> {
            return ErrorResult{Error_Reason};
        };
        static_assert(expr().error() == Error_Reason);

        constexpr Result res = expr();
        static_assert(!res.has_value());
        static_assert(res.error() == Error_Reason);
    }

    {
        constexpr auto expr = []() -> Result<int, Error> {
            return {1};
        };
        static_assert(expr().value() == 1);

        constexpr Result res = expr();
        static_assert(res.has_value());
        static_assert(res.value() == 1);
    }
}

[[maybe_unused]]
void check_maybe_behavior()
{
    {
        constexpr auto expr = []() -> Maybe<int> {
            return {};
        };
        static_assert(!expr().has_value());

        constexpr Maybe mb = expr();
        static_assert(!mb.has_value());
        static_assert(!mb.has_value());
    }

    {
        constexpr auto expr = []() -> Maybe<int> {
            return 1;
        };
        static_assert(expr().value() == 1);

        constexpr Maybe mb = expr();
        static_assert(mb.has_value());
        static_assert(mb.value() == 1);
    }
}

} // namespace
} // namespace dr