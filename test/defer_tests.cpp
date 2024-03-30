#include <utest/utest.h>

#include <dr/basic_types.hpp>
#include <dr/defer.hpp>

UTEST(defer, sanity)
{
    using namespace dr;

    isize x = 0;

    {
        auto on_exit = defer([&]() {
            ++x;
        });

        ASSERT_EQ(x, 0);
    }

    ASSERT_EQ(x, 1);
}
