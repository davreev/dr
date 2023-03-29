#include <utest.h>

#include <dr/diagnostics.hpp>

#include "test_utils.hpp"

UTEST(diagnostics, scoped_timer)
{
    using namespace dr;
    using namespace std::chrono_literals;

    std::FILE* out = safe_fopen("scoped_timer_test.txt", "w+");
    ASSERT_TRUE(out != nullptr);

    {
        NanoScopedTimer timer{"Test", out};
        // ...
    }

    {
        MicroScopedTimer timer{"Test", out};
        // ...
    }

    {
        MilliScopedTimer timer{"Test", out};
        // ...
    }

    std::fclose(out);
}
