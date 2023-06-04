#include <nanobench.h>

#include <dr/linalg_reshape.hpp>
#include <dr/math.hpp>

namespace nb = ankerl::nanobench;

namespace dr
{
namespace
{

void bench_mat4_solve()
{
    Mat4<f64> const A = mat(
        vec(1.0, 0.0, 0.0, 0.0),
        vec(1.0, 2.0, 0.0, 0.0),
        vec(1.0, 0.0, 3.0, 0.0),
        vec(1.0, 0.0, 0.0, 4.0));

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

} // namespace

} // namespace dr

int main()
{
    dr::bench_mat4_solve();
    return 0;
}