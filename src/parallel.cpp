#include <dr/parallel.hpp>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <dr/container_utils.hpp>

namespace dr
{

#ifdef _OPENMP

isize ParallelFor::max_num_threads()
{
    return omp_get_max_threads();
}

void ParallelFor::set_schedule(Schedule const schedule, isize const chunk_size)
{
    static omp_sched_t const to_omp[]{
        omp_sched_static,
        omp_sched_dynamic,
    };
    static_assert(size(to_omp) == _Schedule_Count);

    omp_set_schedule(to_omp[schedule], chunk_size);
}

isize ParallelFor::thread_index() { return omp_get_thread_num(); }

#else

// clang-format off

isize ParallelFor::max_num_threads() { return 1; }

void ParallelFor::set_schedule(Schedule /*schedule*/, isize /*chunk_size*/) {}

isize ParallelFor::thread_index() { return 0; }

// clang-format on

#endif

} // namespace dr