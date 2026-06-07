#include <utest.h>

#include <dr/random.hpp>

namespace dr
{
namespace
{

template <typename Real>
bool check_random_real(Real const min, Real const max)
{
    Random rnd{1};
    auto gen = rnd.generator(min, max);

    for (isize i = 0; i < 1000; ++i)
    {
        Real const x = gen();

        // Reals should be in [min, max)
        if (x < min || x >= max)
            return false;
    }

    return true;
}

template <typename Int>
bool check_random_int(Int const min, Int const max)
{
    Random rnd{1};
    auto gen = rnd.generator(min, max);

    for (isize i = 0; i < 1000; ++i)
    {
        Int const x = gen();

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

/*
    Compile-time checks
*/

namespace dr
{

/*
    Explicit instantiation of templates to catch compile errors
*/

template struct Random<std::default_random_engine>;
template struct Random<std::default_random_engine>::Generator<f64>;
template struct Random<std::default_random_engine>::Generator<i32>;

template Random<std::default_random_engine>::Generator<f64>
Random<std::default_random_engine>::generator<f64>(f64 const, f64 const);

template Random<std::default_random_engine>::Generator<i32>
Random<std::default_random_engine>::generator<i32>(i32 const, i32 const);

} // namespace dr