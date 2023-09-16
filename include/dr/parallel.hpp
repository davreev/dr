#pragma once

#include <type_traits>

#include <dr/basic_types.hpp>

namespace dr
{

struct ParallelFor
{
    enum Schedule : u8
    {
        Schedule_Static = 0,
        Schedule_Dynamic,
        _Schedule_Count,
    };

    isize num_threads{max_num_threads()};
    Schedule schedule{};
    isize chunk_size{};

    /// The maximum number of threads that can be used within a loop
    static isize max_num_threads();

    /// Executes the given loop body in parallel. The loop body is expected to be an invocable with
    /// the signature `void body(isize i, isize j, isize thread_idx)`.
    template <typename Body>
    void operator()(isize const count, Body&& body) const
    {
        static_assert(std::is_invocable_v<Body, isize, isize>);
        set_schedule(schedule, chunk_size);

#pragma omp parallel num_threads(num_threads)
        {
            isize const t_idx = thread_index();

#pragma omp for schedule(runtime)
            for (isize i = 0; i < count; ++i)
                body(i, t_idx);
        }
    }

    /// Executes the given loop body in parallel. The loop body is expected to be an invocable with
    /// the signature `void body(isize i, isize j, isize thread_idx)`.
    template <typename Body>
    void operator()(isize const count_i, isize const count_j, Body&& body) const
    {
        static_assert(std::is_invocable_v<Body, isize, isize, isize>);
        set_schedule(schedule, chunk_size);

#pragma omp parallel num_threads(num_threads)
        {
            isize const t_idx = thread_index();

#pragma omp for schedule(runtime) collapse(2)
            for (isize i = 0; i < count_i; ++i)
            {
                for (isize j = 0; j < count_j; ++j)
                    body(i, j, t_idx);
            }
        }
    }

    /// Executes the given loop body in parallel. The loop body is expected to be an invocable with
    /// the signature `void body(isize i, isize j, isize thread_idx)`.
    /// thread_idx).
    template <typename Body>
    void operator()(
        isize const count_i,
        isize const count_j,
        isize const count_k,
        Body&& body) const
    {
        static_assert(std::is_invocable_v<Body, isize, isize, isize, isize>);
        set_schedule(schedule, chunk_size);

#pragma omp parallel num_threads(num_threads)
        {
            isize const t_idx = thread_index();

#pragma omp for schedule(runtime) collapse(3)
            for (isize i = 0; i < count_i; ++i)
            {
                for (isize j = 0; j < count_j; ++j)
                {
                    for (isize k = 0; k < count_k; ++k)
                        body(i, j, k, t_idx);
                }
            }
        }
    }

  private:
    static void set_schedule(Schedule schedule, isize chunk_size);
    static isize thread_index();
};

} // namespace dr
