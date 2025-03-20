#pragma once

#include <random>

#include <dr/basic_types.hpp>
#include <dr/math_traits.hpp>

namespace dr
{
namespace impl
{

template <typename Scalar, typename Enable = void>
struct UniformDistribution;

template <typename Real>
struct UniformDistribution<Real, std::enable_if_t<is_real<Real>>>
{
    using Type = std::uniform_real_distribution<Real>;
};

template <typename Int>
struct UniformDistribution<Int, std::enable_if_t<(is_integer<Int> || is_natural<Int>)>>
{
    using Type = std::uniform_int_distribution<Int>;
};

} // namespace impl

template <typename Scalar>
using UniformDistribution = typename impl::UniformDistribution<Scalar>::Type;

template <typename Engine = std::default_random_engine>
struct Random
{
    template <typename Scalar>
    struct Generator
    {
        using Distribution = UniformDistribution<Scalar>;

        Generator(Engine* engine, Scalar const min, Scalar const max) :
            engine_{engine}, distribution_{min, max}
        {
        }

        Scalar operator()() { return distribution_(*engine_); }

      private:
        Engine* engine_;
        Distribution distribution_;
    };

    Random() = default;

    Random(u32 const seed) : engine_{seed} {}

    template <typename Scalar>
    Generator<Scalar> generator(Scalar const min, Scalar const max)
    {
        return {&engine_, min, max};
    }

  private:
    Engine engine_{};
};

} // namespace dr
