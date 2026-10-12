#pragma once

#include <random>

#include <dr/meta.hpp>
#include <dr/num_traits.hpp>

namespace dr
{
namespace impl
{

template <typename T, typename Enable = void>
struct UniformDistribution;

template <typename Num>
struct UniformDistribution<
    Num,
    std::enable_if_t<NumTraits<Num>::is_real && NumTraits<Num>::is_builtin>>
{
    using Type = std::uniform_real_distribution<Num>;
};

template <typename Num>
struct UniformDistribution<
    Num,
    std::enable_if_t<
        (NumTraits<Num>::is_integer || NumTraits<Num>::is_natural) && NumTraits<Num>::is_builtin>>
{
    using Type = std::uniform_int_distribution<Num>;
};

} // namespace impl

template <typename Num>
using UniformDistribution = typename impl::UniformDistribution<DropCvRef<Num>>::Type;

template <typename Engine = std::default_random_engine>
struct Random
{
    template <typename Num>
    struct Generator
    {
        Generator(Engine* engine, Num const min, Num const max) :
            engine_{engine}, distrib_{min, max}
        {
        }

        Num operator()() { return distrib_(*engine_); }

      private:
        using Distribution = UniformDistribution<Num>;

        using UnsupportedNums = TypePack<u8, i8>;
        static_assert(!UnsupportedNums::includes<Num>);

        Engine* engine_;
        Distribution distrib_;
    };

    Random() = default;

    Random(u32 const seed) : engine_{seed} {}

    template <typename Num>
    Generator<Num> generator(Num const min, Num const max)
    {
        return {&engine_, min, max};
    }

  private:
    Engine engine_{};
};

} // namespace dr
