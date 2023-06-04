#pragma once

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

    /// Loop body is expected to take loop and thread indices as arguments e.g.
    /// void body(isize loop_index, isize thread_index)
    template <typename Body>
    void operator()(isize const count, Body&& body) const
    {
        set_schedule(schedule, chunk_size);

#pragma omp parallel for num_threads(num_threads) schedule(runtime)
        for (isize i = 0; i < count; ++i)
            body(i, thread_index());
    }

    /// Loop body is expected to take loop and thread indices as arguments e.g.
    /// void body(isize loop_index_i, isize loop_index j, isize thread_index)
    template <typename Body>
    void operator()(isize const count_i, isize const count_j, Body&& body) const
    {
        set_schedule(schedule, chunk_size);

#pragma omp parallel for num_threads(num_threads) schedule(runtime) collapse(2)
        for (isize i = 0; i < count_i; ++i)
        {
            for (isize j = 0; j < count_j; ++j)
                body(i, j, thread_index());
        }
    }

    /// Loop body is expected to take loop and thread indices as arguments e.g.
    /// void body(isize loop_index_i, isize loop_index j, isize loop_index k, isize thread_index)
    template <typename Body>
    void operator()(
        isize const count_i,
        isize const count_j,
        isize const count_k,
        Body&& body) const
    {
        set_schedule(schedule, chunk_size);

#pragma omp parallel for num_threads(num_threads) schedule(runtime) collapse(3)
        for (isize i = 0; i < count_i; ++i)
        {
            for (isize j = 0; j < count_j; ++j)
            {
                for (isize k = 0; k < count_k; ++k)
                    body(i, j, k, thread_index());
            }
        }
    }

  private:
    static void set_schedule(Schedule schedule, isize chunk_size);
    static isize thread_index();
};

} // namespace dr
