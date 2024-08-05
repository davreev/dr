#include <utest.h>

#include <dr/basic_types.hpp>
#include <dr/dynamic_array.hpp>
#include <dr/linalg_reshape.hpp>
#include <dr/sparse_linalg_types.hpp>
#include <dr/sparse_min_quad.hpp>

#include "test_utils.hpp"

namespace dr
{
namespace
{

template <bool use_iterative_solver>
bool solve_common()
{
    constexpr f64 eps = 1.0e-8;

    struct TestCase
    {
        DynamicArray<Triplet<f64>> coeffs{};
        DynamicArray<f64> b{};
        DynamicArray<i8> is_fixed{};
        struct
        {
            DynamicArray<f64> x{};
        } expect;
    };

    static TestCase const solve_test_cases[]{
        {
            {
                {0, 0, 1.0},
                {1, 1, 1.0},
                {2, 2, 1.0},
                {3, 3, 1.0},
                {4, 4, 1.0},
            },
            {1.0, 2.0, 3.0, 4.0, 5.0},
            {0, 0, 0, 0, 0},
            {
                {1.0, 2.0, 3.0, 4.0, 5.0},
            },
        },
        {
            {
                {0, 0, 1.0},
                {1, 1, 1.0},
                {2, 2, 1.0},
                {3, 3, 1.0},
                {4, 4, 1.0},
            },
            {0.0, 2.0, 3.0, 4.0, 5.0},
            {1, 0, 0, 0, 0},
            {
                {1.0, 2.0, 3.0, 4.0, 5.0},
            },
        },
        {
            {
                {0, 0, 1.0},
                {1, 1, 1.0},
                {2, 2, 1.0},
                {3, 3, 1.0},
                {4, 4, 1.0},
            },
            {0.0, 2.0, 0.0, 4.0, 5.0},
            {1, 0, 1, 0, 0},
            {
                {1.0, 2.0, 3.0, 4.0, 5.0},
            },
        },
        {
            {
                {0, 0, 2.0},
                {1, 1, 2.0},
                {2, 2, 2.0},
                {3, 3, 2.0},
                {4, 4, 2.0},
                {0, 3, 1.0},
                {3, 0, 1.0},
            },
            {6.0, 4.0, 6.0, 9.0, 10.0},
            {0, 0, 0, 0, 0},
            {
                {1.0, 2.0, 3.0, 4.0, 5.0},
            },
        },
        {
            {
                {0, 0, 2.0},
                {1, 1, 2.0},
                {2, 2, 2.0},
                {3, 3, 2.0},
                {4, 4, 2.0},
                {0, 3, 1.0},
                {3, 0, 1.0},
            },
            {6.0, 4.0, 6.0, 9.0, 10.0},
            {1, 0, 0, 0, 0},
            {
                {1.0, 2.0, 3.0, 4.0, 5.0},
            },
        },
        {
            {
                {0, 0, 2.0},
                {1, 1, 2.0},
                {2, 2, 2.0},
                {3, 3, 2.0},
                {4, 4, 2.0},
                {0, 3, 1.0},
                {3, 0, 1.0},
            },
            {6.0, 4.0, 6.0, 9.0, 10.0},
            {1, 0, 1, 0, 0},
            {
                {1.0, 2.0, 3.0, 4.0, 5.0},
            },
        },
    };

    using Solver = SparseMinQuadFixed<f64, i32, use_iterative_solver>;
    Solver solver{};

    for (auto const& tc : solve_test_cases)
    {
        isize const n = tc.b.size();
        SparseMat<f64> A(n, n);
        A.setFromTriplets(tc.coeffs.begin(), tc.coeffs.end());

        solver.init(A, [&](i32 const i) -> bool { return tc.is_fixed[i]; });

        DynamicArray<f64> x = tc.expect.x;
        solver.solve(as_vec(as_span(tc.b)), as_vec(as_span(x)));

        if(!all_near_equal(as_span(tc.expect.x), as_span(x).as_const(), eps))
            return false;
    }

    return true;
}

} // namespace
} // namespace dr

UTEST(sparse_min_quad, solve_direct)
{
    ASSERT_TRUE(dr::solve_common<false>());
}

UTEST(sparse_min_quad, solve_iterative)
{
    ASSERT_TRUE(dr::solve_common<true>());
}
