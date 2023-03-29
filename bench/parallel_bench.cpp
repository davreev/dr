#include "bench.hpp"

#include <nanobench.h>

#include <dr/linalg_reshape.hpp>
#include <dr/math.hpp>
#include <dr/parallel.hpp>

namespace nb = ankerl::nanobench;

namespace dr
{
namespace
{

template <typename Body>
void bench_parallel_for(
    char const* const name,
    ParallelFor const& parallel_for,
    isize const num_iters,
    Body&& body)
{
    nb::Bench().run(name, [&] {
        parallel_for(num_iters, body);
    });
}

template <typename Body>
void bench_serial_for(
    char const* const name,
    isize const num_iters,
    Body&& body)
{
    nb::Bench().run(name, [&] {
        for (isize i = 0; i < num_iters; ++i)
            body(i, 0);
    });
}

void parallel_vec3_reject_norm()
{
    constexpr i32 n = 1000000;

    VecArray<f64, 3> vecs_a{3, n};
    vecs_a.setRandom();

    VecArray<f64, 3> vecs_b{3, n};
    vecs_b.setRandom();

    VecN<f64> result{n};

    Span<Vec3<f64> const> const src_a = as_span(vecs_a);
    Span<Vec3<f64> const> const src_b = as_span(vecs_b);
    Span<f64> const dst = as_span(result);

    auto const body = [&](isize const i, isize /*thread_id*/) {
        dst[i] = reject(src_a[i], src_b[i]).norm();
    };

    bench_serial_for("Vec3<f64> reject norm serial", n, body);
    bench_parallel_for("Vec3<f64> reject norm parallel", ParallelFor{}, n, body);
}

} // namespace

void parallel_bench()
{
    parallel_vec3_reject_norm();
    // ...
}

} // namespace dr