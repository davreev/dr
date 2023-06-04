#include <nanobench.h>

#include <dr/linalg_reshape.hpp>
#include <dr/math.hpp>

namespace nb = ankerl::nanobench;

namespace dr
{
namespace
{

void bench_mat3_mul()
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

} // namespace

} // namespace dr

int main()
{
    dr::bench_mat3_mul();
    return 0;
}