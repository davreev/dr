#include <nanobench.h>

#include <dr/math.hpp>

namespace nb = ankerl::nanobench;

namespace dr
{
namespace
{

void bench_mod()
{
    {
        f32 x = -1.0f;

        nb::Bench().run("mod f32", [&] {
            mod(x, 1.0f);
            x += 0.1;
            nb::doNotOptimizeAway(x);
        });
    }

    {
        i32 x = -100;

        nb::Bench().run("mod i32", [&] {
            mod(x, 10);
            ++x;
            nb::doNotOptimizeAway(x);
        });
    }

    {
        u32 x = 0;

        nb::Bench().run("mod u32", [&] {
            mod(x, 10u);
            ++x;
            nb::doNotOptimizeAway(x);
        });
    }
}

} // namespace

} // namespace dr

int main()
{
    dr::bench_mod();
    return 0;
}