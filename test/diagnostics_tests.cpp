#include <utest.h>

#include <filesystem>

#include <dr/diagnostics.hpp>

namespace fs = std::filesystem;

UTEST(diagnostics, scoped_timer)
{
    using namespace dr;
    using namespace std::chrono_literals;

    fs::path const tmp_dir = std::filesystem::temp_directory_path();
    fs::path const path = tmp_dir / "scoped_timer_test.txt";

    std::FILE* out = std::fopen(path.string().c_str(), "w+");
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
