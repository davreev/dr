#include <utest/utest.h>

#include <dr/random.hpp>

namespace dr
{
namespace
{

template <typename Real>
bool check_random_real(Real const min, Real const max)
{
    Random<Real> rnd{min, max, 1};

    for (isize i = 0; i < 1000; ++i)
    {
        Real const x = rnd();

        // Reals should be in [min, max)
        if (x < min || x >= max)
            return false;
    }

    return true;
}

template <typename Int>
bool check_random_int(Int const min, Int const max)
{
    Random<Int> rnd{min, max, 1};

    for (isize i = 0; i < 1000; ++i)
    {
        Int const x = rnd();

        // Integers should be in [min, max]
        if (x < min || x > max)
            return false;
    }

    return true;
}

} // namespace
} // namespace dr

UTEST(random, generate)
{
    using namespace dr;

    ASSERT_TRUE(check_random_real<f32>(0.0f, 1.0f));
    ASSERT_TRUE(check_random_real<f64>(-1.0, 1.0));

    ASSERT_TRUE(check_random_int<u16>(0u, 1000u));
    ASSERT_TRUE(check_random_int<i32>(-1000, 1000));
}