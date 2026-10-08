#pragma once

#include <limits>

#include <dr/num_traits.hpp>

namespace dr
{

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num pi{
    3.141592653589793238462643383279502884197169399375105820974944592307816406L};

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num inv_pi{Num{1.0} / pi<Num>};

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num e{2.7182818284590452353602874713526624977572470936999595749669676277L};

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num sqrt2{1.4142135623730950488016887242096980785696718753769480731766797379L};

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num sqrt3{1.7320508075688772935274463415058723669428052538103806280558069794L};

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num ln2{0.6931471805599453094172321214581765680755001343602552541206800094L};

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num ln10{2.3025850929940456840179914546843642076011014886287729760333279009L};

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num log2e{1.4426950408889634073599246810018921374266459541529859341354494069L};

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num log10e{0.4342944819032518276511289189166050822943970058036665661144537831L};

template <typename Num, std::enable_if_t<is_real<Num>>* = nullptr>
inline constexpr Num default_epsilon{std::numeric_limits<Num>::epsilon()};

} // namespace dr