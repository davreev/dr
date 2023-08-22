#pragma once

#include <cmath>

#include <dr/math.hpp>
#include <dr/math_traits.hpp>

namespace dr
{
namespace impl
{

template <typename Real>
constexpr void eval_catmull_rom_basis(Real const t, Real result[4])
{
    static_assert(is_real<Real>);

    /*
        Evaluated as product with Catmull-Rom basis matrix

        | f0  f1  f2  f3 | = 0.5 * | ttt  tt  t  1 | * |-1  3 -3  1 |
                                                       | 2 -5  4 -1 |
                                                       |-1  0  1  0 |
                                                       | 0  2  0  0 |
    */

    Real const tt = t * t;
    Real const ttt = tt * t;

    result[0] = Real{0.5} * (Real{2.0} * tt - ttt - t);
    result[1] = Real{0.5} * (Real{3.0} * ttt + Real{2.0} - Real{5.0} * tt);
    result[2] = Real{0.5} * (Real{4.0} * tt + t - Real{3.0} * ttt);
    result[3] = Real{0.5} * (ttt - tt);
}

template <typename Real>
constexpr void eval_hat_basis(Real const t, Real result[2])
{
    result[0] = Real{1.0} - t;
    result[1] = t;
}

template <typename Real>
constexpr void eval_bernstein_basis_deg2(Real const t, Real result[3])
{
    static_assert(is_real<Real>);

    /*
        Evaluated as product with degree 2 Bernstein basis matrix

        | f0  f1  f2 | = | tt  t  1 | * | 1 -2  1 |
                                        |-2  2  0 |
                                        | 1  0  0 |
    */

    Real const tt = t * t;

    result[0] = tt - Real{2.0} * t + Real{1.0};
    result[1] = Real{2.0} * t - Real{2.0} * tt;
    result[2] = tt;
}

template <typename Real>
constexpr void eval_bernstein_basis_deg3(Real const t, Real result[4])
{
    static_assert(is_real<Real>);

    /*
        Evaluated as product with degree 3 Bernstein basis matrix

        | f0  f1  f2  f3 | = 0.5 * | ttt  tt  t  1 | * |-1  3 -3  1 |
                                                       | 3 -6  3  0 |
                                                       |-3  3  0  0 |
                                                       | 1  0  0  0 |
    */

    Real const tt = t * t;
    Real const ttt = tt * t;

    result[0] = Real{3.0} * tt - ttt - Real{3.0} * t + Real{1.0};
    result[1] = Real{3.0} * ttt - Real{6.0} * tt + Real{3.0} * t;
    result[2] = Real{3.0} * tt - Real{3.0} * ttt;
    result[3] = ttt;
}

template <typename Value, typename Real>
constexpr Value mix4(Value const x[4], Real const f[4])
{
    static_assert(is_real<Real>);
    return f[0] * x[0] + f[1] * x[1] + f[2] * x[2] + f[3] * x[3];
}

template <typename Value, typename Real>
constexpr Value mix3(Value const x[3], Real const f[3])

{
    static_assert(is_real<Real>);
    return f[0] * x[0] + f[1] * x[1] + f[2] * x[2];
}

template <typename Value, typename Real>
constexpr Value mix2(Value const x[2], Real const f[2])
{
    static_assert(is_real<Real>);
    return f[0] * x[0] + f[1] * x[1];
}

} // namespace impl

template <typename Value, typename Real>
constexpr Value interp_linear(Value const x[2], Real const t)
{
    static_assert(is_real<Real>);
    Real const f[2]{t, Real{1.0} - t};
    return impl::mix2(x, f);
}

template <typename Value, typename Real>
constexpr Value interp_bilinear(Value const x[4], Real const u, Real const v)
{
    static_assert(is_real<Real>);

    Real f_u[2]{};
    impl::eval_hat_basis(saturate(u), f_u);

    Real f_v[2]{};
    impl::eval_hat_basis(saturate(v), f_v);

    Value const x_v[2]{
        impl::mix2(x, f_u),
        impl::mix2(x + 2, f_u),
    };

    return impl::mix2(x_v, f_v);
}

template <typename Value, typename Real>
constexpr Value interp_trilinear(Value const x[8], Real const u, Real const v, Real const w)
{
    static_assert(is_real<Real>);

    Real f_u[2]{};
    impl::eval_hat_basis(saturate(u), f_u);

    Real f_v[2]{};
    impl::eval_hat_basis(saturate(v), f_v);

    Real f_w[2]{};
    impl::eval_hat_basis(saturate(w), f_w);

    Value const* x_u = x;
    Value x_v[2]{};
    Value x_w[2]{};

    for (i8 i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            x_v[j] = impl::mix2(x_u, f_u);
            x_u += 2;
        }

        x_w[i] = impl::mix2(x_v, f_v);
    }

    return impl::mix2(x_w, f_w);
}

template <typename Value, typename Real>
constexpr Value interp_cubic(Value const x[4], Real const t)
{
    static_assert(is_real<Real>);

    Real f[4]{};
    impl::eval_catmull_rom_basis(saturate(t), f);

    return impl::mix4(x, f);
}

template <typename Value, typename Real>
constexpr Value interp_bicubic(Value const x[16], Real const u, Real const v)
{
    static_assert(is_real<Real>);

    Real f_u[4]{};
    impl::eval_catmull_rom_basis(saturate(u), f_u);

    Real f_v[4]{};
    impl::eval_catmull_rom_basis(saturate(v), f_v);

    Value const x_v[4]{
        impl::mix4(x, f_u),
        impl::mix4(x + 4, f_u),
        impl::mix4(x + 8, f_u),
        impl::mix4(x + 12, f_u),
    };

    return impl::mix4(x_v, f_v);
}

template <typename Value, typename Real>
constexpr Value interp_tricubic(Value const x[64], Real const u, Real const v, Real const w)
{
    static_assert(is_real<Real>);

    Real f_u[4]{};
    impl::eval_catmull_rom_basis(saturate(u), f_u);

    Real f_v[4]{};
    impl::eval_catmull_rom_basis(saturate(v), f_v);

    Real f_w[4]{};
    impl::eval_catmull_rom_basis(saturate(w), f_w);

    Value const* x_u = x;
    Value x_v[4]{};
    Value x_w[4]{};

    for (i8 i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            x_v[j] = impl::mix4(x_u, f_u);
            x_u += 4;
        }

        x_w[i] = impl::mix4(x_v, f_v);
    }

    return impl::mix4(x_w, f_w);
}

template <typename Value, typename Real>
constexpr Value bezier_quadratic(Value const x[3], Real const t)
{
    static_assert(is_real<Real>);
    Real f[3]{};
    impl::eval_bernstein_basis_deg2(t, f);
    return impl::mix3(x, f);
}

template <typename Value, typename Real>
constexpr Value bezier_cubic(Value const x[4], Real const t)
{
    static_assert(is_real<Real>);
    Real f[4]{};
    impl::eval_bernstein_basis_deg3(t, f);
    return impl::mix4(x, f);
}

} // namespace dr