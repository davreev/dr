#pragma once

#include <dr/math_traits.hpp>

namespace dr
{
namespace impl
{

template <isize n, typename Value, typename Real>
constexpr Value spline_mix(Value const coeffs[n], Real const b[n])
{
    Value result{};

    for (isize i = 0; i < n; ++i)
        result += coeffs[i] * b[i];

    return result;
}

template <isize n_u, isize n_v, typename Value, typename Real>
constexpr Value spline_mix(Value const coeffs[n_u * n_v], Real const b_u[n_u], Real const b_v[n_v])
{
    Value const* c_it = coeffs;
    Value result{};

    for (isize i = 0; i < n_v; ++i)
    {
        for (isize j = 0; j < n_u; ++j)
        {
            result += *c_it * (b_v[i] * b_u[j]);
            ++c_it;
        }
    }

    return result;
}

template <isize n_u, isize n_v, isize n_w, typename Value, typename Real>
constexpr Value spline_mix(
    Value const coeffs[n_u * n_v * n_w],
    Real const b_u[n_u],
    Real const b_v[n_v],
    Real const b_w[n_w])
{
    Value const* c_it = coeffs;
    Value result{};

    for (isize i = 0; i < n_w; ++i)
    {
        for (isize j = 0; j < n_v; ++j)
        {
            for (isize k = 0; k < n_u; ++k)
            {
                result += *c_it * (b_w[i] * b_v[j] * b_u[k]);
                ++c_it;
            }
        }
    }

    return result;
}

template <typename Basis, typename Value, typename Real, isize n = Basis::size>
constexpr Value spline_eval(Value const coeffs[n], Real const t)
{
    static_assert(is_real<Real>);

    Real b[n]{};
    Basis::eval(t, b);

    return spline_mix<n>(coeffs, b);
}

template <
    typename BasisU,
    typename BasisV,
    typename Value,
    typename Real,
    isize n_u = BasisU::size,
    isize n_v = BasisV::size>
constexpr Value spline_eval(Value const coeffs[n_u * n_v], Real const u, Real const v)
{
    static_assert(is_real<Real>);

    Real b_u[n_u]{};
    BasisU::eval(u, b_u);

    Real b_v[n_v]{};
    BasisV::eval(v, b_v);

    return spline_mix<n_u, n_v>(coeffs, b_u, b_v);
}

template <
    typename BasisU,
    typename BasisV,
    typename BasisW,
    typename Value,
    typename Real,
    isize n_u = BasisU::size,
    isize n_v = BasisV::size,
    isize n_w = BasisW::size>
constexpr Value spline_eval(Value const coeffs[n_u * n_v * n_w], Real const u, Real const v, Real const w)
{
    static_assert(is_real<Real>);

    Real b_u[n_u]{};
    BasisU::eval(u, b_u);

    Real b_v[n_v]{};
    BasisV::eval(v, b_v);

    Real b_w[n_w]{};
    BasisW::eval(w, b_w);

    return spline_mix<n_u, n_v, n_w>(coeffs, b_u, b_v, b_w);
}

template <
    typename BasisU,
    typename BasisV,
    typename Value,
    typename Real,
    isize n_u = BasisU::size,
    isize n_v = BasisV::size>
constexpr void spline_eval(
    Value const coeffs[n_u * n_v],
    Real const u,
    Real const v,
    Value val[1],
    Value diff[2])
{
    static_assert(is_real<Real>);

    using DiffU = typename BasisU::template Diff<1>;
    using DiffV = typename BasisV::template Diff<1>;

    Real b_u[2][n_u]{};
    BasisU::eval(u, b_u[0]);

    Real b_v[2][n_v]{};
    BasisV::eval(v, b_v[0]);

    if (val)
        val[0] = spline_mix<n_u, n_v>(coeffs, b_u[0], b_v[0]);

    if (diff)
    {
        DiffU::eval(u, b_u[1]);
        diff[0] = spline_mix<n_u, n_v>(coeffs, b_u[1], b_v[0]);

        DiffV::eval(v, b_v[1]);
        diff[1] = spline_mix<n_u, n_v>(coeffs, b_u[0], b_v[1]);
    }
}

template <
    typename BasisU,
    typename BasisV,
    typename BasisW,
    typename Value,
    typename Real,
    isize n_u = BasisU::size,
    isize n_v = BasisV::size,
    isize n_w = BasisW::size>
constexpr void spline_eval(
    Value const coeffs[n_u * n_v * n_w],
    Real const u,
    Real const v,
    Real const w,
    Value val[1],
    Value diff[3])
{
    static_assert(is_real<Real>);

    using DiffU = typename BasisU::template Diff<1>;
    using DiffV = typename BasisV::template Diff<1>;
    using DiffW = typename BasisW::template Diff<1>;

    Real b_u[2][n_u]{};
    BasisU::eval(u, b_u[0]);

    Real b_v[2][n_v]{};
    BasisV::eval(v, b_v[0]);

    Real b_w[2][n_w]{};
    BasisW::eval(w, b_w[0]);

    if (val)
        val[0] = spline_mix<n_u, n_v, n_w>(coeffs, b_u[0], b_v[0], b_w[0]);

    if (diff)
    {
        DiffU::eval(u, b_u[1]);
        diff[0] = spline_mix<n_u, n_v, n_w>(coeffs, b_u[1], b_v[0], b_w[0]);

        DiffV::eval(v, b_v[1]);
        diff[1] = spline_mix<n_u, n_v, n_w>(coeffs, b_u[0], b_v[1], b_w[0]);

        DiffW::eval(w, b_w[1]);
        diff[2] = spline_mix<n_u, n_v, n_w>(coeffs, b_u[0], b_v[0], b_w[1]);
    }
}

} // namespace impl

template <isize order_>
struct SplineBasis
{
    static constexpr isize order{order_};
    static constexpr isize size{order_ + 1};
};

struct LinearBasis : SplineBasis<1>
{
    template <typename Real>
    static constexpr void eval(Real const t, Real result[size])
    {
        static_assert(is_real<Real>);

        result[0] = Real{1.0} - t;
        result[1] = t;
    }

    template <isize order_>
    struct Diff : SplineBasis<1>
    {
        template <typename Real>
        static constexpr void eval(Real const /*t*/, Real result[size])
        {
            static_assert(is_real<Real>);
            static_assert(order_ > 0);

            if constexpr (order_ == 1)
            {
                result[0] = Real{-1.0};
                result[1] = Real{1.0};
            }
            else
            {
                result[0] = Real{0.0};
                result[1] = Real{0.0};
            }
        }
    };
};

struct CatmullRomBasis : SplineBasis<3>
{
    template <typename Real>
    static constexpr void eval(Real const t, Real result[size])
    {
        static_assert(is_real<Real>);

        /*
            Evaluated as product with basis matrix

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

    template <isize order_>
    struct Diff : SplineBasis<3>
    {
        template <typename Real>
        static constexpr void eval(Real const t, Real result[size])
        {
            static_assert(is_real<Real>);
            static_assert(order_ > 0);

            if constexpr (order_ == 1)
            {
                Real const tt = t * t;
                result[0] = Real{2.0} * t - Real{1.5} * tt - Real{0.5};
                result[1] = Real{4.5} * tt - Real{5.0} * t;
                result[2] = Real{4.0} * t + Real{0.5} - Real{4.5} * tt;
                result[3] = Real{1.5} * tt - t;
            }
            else if (order_ == 2)
            {
                result[0] = Real{2.0} - Real{3.0} * t;
                result[1] = Real{9.0} * t - Real{5.0};
                result[2] = Real{4.0} - Real{9.0} * t;
                result[3] = Real{3.0} * t - Real{1.0};
            }
            else if (order_ == 3)
            {
                result[0] = Real{-3.0};
                result[1] = Real{9.0};
                result[2] = Real{-9.0};
                result[3] = Real{3.0};
            }
            else
            {
                result[0] = Real{0.0};
                result[1] = Real{0.0};
                result[2] = Real{0.0};
                result[3] = Real{0.0};
            }
        }
    };
};

template <isize order>
struct BernsteinBasis;

template <>
struct BernsteinBasis<2> : SplineBasis<2>
{
    template <typename Real>
    static constexpr void eval(Real const t, Real result[size])
    {
        static_assert(is_real<Real>);

        /*
            Evaluated as product with basis matrix

            | f0  f1  f2 | = | tt  t  1 | * | 1 -2  1 |
                                            |-2  2  0 |
                                            | 1  0  0 |
        */

        Real const tt = t * t;
        result[0] = tt - Real{2.0} * t + Real{1.0};
        result[1] = Real{2.0} * t - Real{2.0} * tt;
        result[2] = tt;
    }

    template <isize order_>
    struct Diff : SplineBasis<2>
    {
        template <typename Real>
        static constexpr void eval(Real const t, Real result[size])
        {
            static_assert(is_real<Real>);
            static_assert(order_ > 0);

            if constexpr (order_ == 1)
            {
                result[0] = Real{2.0} * t - Real{2.0};
                result[1] = Real{2.0} - Real{4.0} * t;
                result[2] = Real{2.0} * t;
            }
            else if (order_ == 2)
            {
                result[0] = Real{2.0};
                result[1] = Real{-4.0};
                result[2] = Real{2.0};
            }
            else
            {
                result[0] = Real{0.0};
                result[1] = Real{0.0};
                result[2] = Real{0.0};
            }
        }
    };
};

template <>
struct BernsteinBasis<3> : SplineBasis<3>
{
    template <typename Real>
    static constexpr void eval(Real const t, Real result[size])
    {
        static_assert(is_real<Real>);

        /*
            Evaluated as product with basis matrix

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

    template <isize order_>
    struct Diff : SplineBasis<3>
    {
        template <typename Real>
        static constexpr void eval(Real const t, Real result[size])
        {
            static_assert(is_real<Real>);
            static_assert(order_ > 0);

            if constexpr (order_ == 1)
            {
                Real const tt = t * t;
                result[0] = Real{6.0} * t - Real{3.0} * tt - Real{3.0};
                result[1] = Real{9.0} * tt - Real{12.0} * t + Real{3.0};
                result[2] = Real{6.0} * t - Real{9.0} * tt;
                result[3] = Real{3.0} * tt;
            }
            else if (order_ == 2)
            {
                result[0] = Real{6.0} - Real{2.0} * t;
                result[1] = Real{18.0} * t - Real{12.0};
                result[2] = Real{6.0} - Real{18.0} * t;
                result[3] = Real{6.0} * t;
            }
            else if (order_ == 3)
            {
                result[0] = Real{-2.0};
                result[1] = Real{18.0};
                result[2] = Real{-18.0};
                result[3] = Real{6.0};
            }
            else
            {
                result[0] = Real{0.0};
                result[1] = Real{0.0};
                result[2] = Real{0.0};
                result[3] = Real{0.0};
            }
        }
    };
};

template <typename Value, typename Real>
constexpr Value interp_linear(Value const x[2], Real const t)
{
    return impl::spline_eval<LinearBasis>(x, t);
}

template <typename Value, typename Real>
constexpr Value interp_linear_dt(Value const x[2], Real const t)
{
    return impl::spline_eval<LinearBasis::Diff<1>>(x, t);
}

template <typename Value, typename Real>
constexpr Value interp_bilinear(Value const x[4], Real const u, Real const v)
{
    using B = LinearBasis;
    return impl::spline_eval<B, B>(x, u, v);
}

template <typename Value, typename Real>
constexpr void interp_bilinear(
    Value const x[4],
    Real const u,
    Real const v,
    Value val[1],
    Value diff[2])
{
    using B = LinearBasis;
    impl::spline_eval<B, B>(x, u, v, val, diff);
}

template <typename Value, typename Real>
constexpr Value interp_bilinear_du(Value const x[4], Real const u, Real const v)
{
    using B = LinearBasis;
    return impl::spline_eval<B::Diff<1>, B>(x, u, v);
}

template <typename Value, typename Real>
constexpr Value interp_bilinear_dv(Value const x[4], Real const u, Real const v)
{
    using B = LinearBasis;
    return impl::spline_eval<B, B::Diff<1>>(x, u, v);
}

template <typename Value, typename Real>
constexpr Value interp_trilinear(Value const x[8], Real const u, Real const v, Real const w)
{
    using B = LinearBasis;
    return impl::spline_eval<B, B, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr void interp_trilinear(
    Value const x[8],
    Real const u,
    Real const v,
    Real const w,
    Value val[1],
    Value diff[3])
{
    using B = LinearBasis;
    impl::spline_eval<B, B, B>(x, u, v, w, val, diff);
}

template <typename Value, typename Real>
constexpr Value interp_trilinear_du(Value const x[8], Real const u, Real const v, Real const w)
{
    using B = LinearBasis;
    return impl::spline_eval<B::Diff<1>, B, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_trilinear_dv(Value const x[8], Real const u, Real const v, Real const w)
{
    using B = LinearBasis;
    return impl::spline_eval<B, B::Diff<1>, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_trilinear_dw(Value const x[8], Real const u, Real const v, Real const w)
{
    using B = LinearBasis;
    return impl::spline_eval<B, B, B::Diff<1>>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_cubic(Value const x[4], Real const t)
{
    return impl::spline_eval<CatmullRomBasis>(x, t);
}

template <typename Value, typename Real>
constexpr Value interp_cubic_dt(Value const x[4], Real const t)
{
    return impl::spline_eval<CatmullRomBasis::Diff<1>>(x, t);
}

template <typename Value, typename Real>
constexpr Value interp_bicubic(Value const x[16], Real const u, Real const v)
{
    using B = CatmullRomBasis;
    return impl::spline_eval<B, B>(x, u, v);
}

template <typename Value, typename Real>
constexpr void interp_bicubic(
    Value const x[16],
    Real const u,
    Real const v,
    Value val[1],
    Value diff[2])
{
    using B = CatmullRomBasis;
    impl::spline_eval<B, B>(x, u, v, val, diff);
}

template <typename Value, typename Real>
constexpr Value interp_bicubic_du(Value const x[16], Real const u, Real const v)
{
    using B = CatmullRomBasis;
    return impl::spline_eval<B::Diff<1>, B>(x, u, v);
}

template <typename Value, typename Real>
constexpr Value interp_bicubic_dv(Value const x[16], Real const u, Real const v)
{
    using B = CatmullRomBasis;
    return impl::spline_eval<B, B::Diff<1>>(x, u, v);
}

template <typename Value, typename Real>
constexpr Value interp_tricubic(Value const x[64], Real const u, Real const v, Real const w)
{
    using B = CatmullRomBasis;
    return impl::spline_eval<B, B, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr void interp_tricubic(
    Value const x[64],
    Real const u,
    Real const v,
    Real const w,
    Value val[1],
    Value diff[3])
{
    using B = CatmullRomBasis;
    impl::spline_eval<B, B, B>(x, u, v, w, val, diff);
}

template <typename Value, typename Real>
constexpr Value interp_tricubic_du(Value const x[64], Real const u, Real const v, Real const w)
{
    using B = CatmullRomBasis;
    return impl::spline_eval<B::Diff<1>, B, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_tricubic_dv(Value const x[64], Real const u, Real const v, Real const w)
{
    using B = CatmullRomBasis;
    return impl::spline_eval<B, B::Diff<1>, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_tricubic_dw(Value const x[64], Real const u, Real const v, Real const w)
{
    using B = CatmullRomBasis;
    return impl::spline_eval<B, B, B::Diff<1>>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_quadratic(Value const x[3], Real const t)
{
    return impl::spline_eval<BernsteinBasis<2>>(x, t);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_quadratic_dt(Value const x[3], Real const t)
{
    return impl::spline_eval<BernsteinBasis<2>::Diff<1>>(x, t);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_biquadratic(Value const x[9], Real const u, Real const v)
{
    using B = BernsteinBasis<2>;
    return impl::spline_eval<B, B>(x, u, v);
}

template <typename Value, typename Real>
constexpr void interp_bezier_biquadratic(
    Value const x[9],
    Real const u,
    Real const v,
    Value val[1],
    Value diff[2])
{
    using B = BernsteinBasis<2>;
    impl::spline_eval<B, B>(x, u, v, val, diff);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_biquadratic_du(Value const x[9], Real const u, Real const v)
{
    using B = BernsteinBasis<2>;
    return impl::spline_eval<B::Diff<1>, B>(x, u, v);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_biquadratic_dv(Value const x[9], Real const u, Real const v)
{
    using B = BernsteinBasis<2>;
    return impl::spline_eval<B, B::Diff<1>>(x, u, v);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_triquadratic(
    Value const x[27],
    Real const u,
    Real const v,
    Real const w)
{
    using B = BernsteinBasis<2>;
    return impl::spline_eval<B, B, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr void interp_bezier_triquadratic(
    Value const x[27],
    Real const u,
    Real const v,
    Real const w,
    Value val[1],
    Value diff[3])
{
    using B = BernsteinBasis<2>;
    impl::spline_eval<B, B>(x, u, v, w, val, diff);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_triquadratic_du(
    Value const x[27],
    Real const u,
    Real const v,
    Real const w)
{
    using B = BernsteinBasis<2>;
    return impl::spline_eval<B::Diff<1>, B, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_triquadratic_dv(
    Value const x[27],
    Real const u,
    Real const v,
    Real const w)
{
    using B = BernsteinBasis<2>;
    return impl::spline_eval<B, B::Diff<1>, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_triquadratic_dw(
    Value const x[27],
    Real const u,
    Real const v,
    Real const w)
{
    using B = BernsteinBasis<2>;
    return impl::spline_eval<B, B, B::Diff<1>>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_cubic(Value const x[4], Real const t)
{
    return impl::spline_eval<BernsteinBasis<3>>(x, t);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_cubic_dt(Value const x[4], Real const t)
{
    return impl::spline_eval<BernsteinBasis<3>::Diff<1>>(x, t);
}


template <typename Value, typename Real>
constexpr Value interp_bezier_bicubic(Value const x[16], Real const u, Real const v)
{
    using B = BernsteinBasis<3>;
    return impl::spline_eval<B, B>(x, u, v);
}

template <typename Value, typename Real>
constexpr void interp_bezier_bicubic(
    Value const x[16],
    Real const u,
    Real const v,
    Value val[1],
    Value diff[2])
{
    using B = BernsteinBasis<3>;
    impl::spline_eval<B, B>(x, u, v, val, diff);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_bicubic_du(Value const x[16], Real const u, Real const v)
{
    using B = BernsteinBasis<3>;
    return impl::spline_eval<B::Diff<1>, B>(x, u, v);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_bicubic_dv(Value const x[16], Real const u, Real const v)
{
    using B = BernsteinBasis<3>;
    return impl::spline_eval<B, B::Diff<1>>(x, u, v);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_tricubic(
    Value const x[64],
    Real const u,
    Real const v,
    Real const w)
{
    using B = BernsteinBasis<3>;
    return impl::spline_eval<B, B, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr void interp_bezier_tricubic(
    Value const x[64],
    Real const u,
    Real const v,
    Real const w,
    Value val[1],
    Value diff[3])
{
    using B = BernsteinBasis<3>;
    impl::spline_eval<B, B>(x, u, v, w, val, diff);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_tricubic_du(
    Value const x[64],
    Real const u,
    Real const v,
    Real const w)
{
    using B = BernsteinBasis<3>;
    return impl::spline_eval<B::Diff<1>, B, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_tricubic_dv(
    Value const x[64],
    Real const u,
    Real const v,
    Real const w)
{
    using B = BernsteinBasis<3>;
    return impl::spline_eval<B, B::Diff<1>, B>(x, u, v, w);
}

template <typename Value, typename Real>
constexpr Value interp_bezier_tricubic_dw(
    Value const x[64],
    Real const u,
    Real const v,
    Real const w)
{
    using B = BernsteinBasis<3>;
    return impl::spline_eval<B, B, B::Diff<1>>(x, u, v, w);
}

} // namespace dr