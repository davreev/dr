#pragma once

#include <random>

#include <dr/num_traits.hpp>

namespace dr
{

template <typename Engine = std::default_random_engine>
struct Random
{
    template <typename Num>
    struct Generator
    {
        static_assert(is_number<Num>);

        Generator(Engine* engine, Num const min, Num const max) :
            engine_{engine}, distrib_{min, max}
        {
        }

        Num operator()() { return distrib_(*engine_); }

      private:
        using Distribution = std::conditional_t<
            is_real<Num>,
            std::uniform_real_distribution<Num>,
            std::uniform_int_distribution<Num>>;

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
