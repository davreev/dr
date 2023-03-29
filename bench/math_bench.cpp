#include "bench.hpp"

#include <nanobench.h>

#include <dr/linalg_reshape.hpp>
#include <dr/math.hpp>

namespace nb = ankerl::nanobench;

namespace dr
{
namespace
{

void mapped_mat3_mult_bench()
{
    Vec3<f64> const a_cols[]{
        {1.0, 1.0, 1.0},
        {2.0, 2.0, 2.0},
    };

    Covec3<f64> const b_rows[]{
        {1.0, 1.0, 1.0},
        {2.0, 2.0, 2.0},
    };

    Mat3<f64> AB{};
    nb::doNotOptimizeAway(AB);

    nb::Bench().run("Map<Mat3<f64>> mult", [&] {
        AB = as_mat<2>(a_cols) * as_mat<2>(b_rows);
    });

    nb::Bench().run("Mat3<f64> mult", [&] {
        AB = mat(a_cols[0], a_cols[1]) * mat(b_rows[0], b_rows[1]);
    });
}

void mat4_solve_bench()
{
    Mat4<f64> const A = mat(
        vec(1.0, 0.0, 0.0, 0.0),
        vec(1.0, 2.0, 0.0, 0.0),
        vec(0.0, 0.0, 3.0, 0.0),
        vec(0.0, 0.0, 0.0, 4.0));

    Vec4<f64> const b{1.0, 0.0, 0.0, 0.0};

    nb::Bench().run("Mat4<f64>::inverse", [&] {
        Mat4<f64> const A_inv = A.inverse();
        nb::doNotOptimizeAway(A_inv);

        Vec4<f64> const x = A_inv * b;
        nb::doNotOptimizeAway(x);
    });

    nb::Bench().run("LDLT<Mat4<f64>>", [&] {
        using Decomp = Eigen::LDLT<Mat4<f64>>;
        Decomp decomp{};
        decomp.compute(A);
        nb::doNotOptimizeAway(decomp);

        Vec4<f64> const x = decomp.solve(b);
        nb::doNotOptimizeAway(x);
    });

    nb::Bench().run("PartialPivLU<Mat4<f64>>", [&] {
        using Decomp = Eigen::PartialPivLU<Mat4<f64>>;
        Decomp decomp{};
        decomp.compute(A);
        nb::doNotOptimizeAway(decomp);

        Vec4<f64> const x = decomp.solve(b);
        nb::doNotOptimizeAway(x);
    });

    nb::Bench().run("HouseholderQR<Mat4<f64>>", [&] {
        using Decomp = Eigen::HouseholderQR<Mat4<f64>>;
        Decomp decomp{};
        decomp.compute(A);
        nb::doNotOptimizeAway(decomp);

        Vec4<f64> const x = decomp.solve(b);
        nb::doNotOptimizeAway(x);
    });

    nb::Bench().run("JacobiSVD<Mat4<f64>>", [&] {
        using Decomp = Eigen::JacobiSVD<Mat4<f64>>;
        Decomp decomp{};
        decomp.compute(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
        nb::doNotOptimizeAway(decomp);

        Vec4<f64> const x = decomp.solve(b);
        nb::doNotOptimizeAway(x);
    });
}

void mod_bench()
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

void math_bench()
{
    mat4_solve_bench();
    mapped_mat3_mult_bench();
    mod_bench();
    // ...
}

} // namespace dr