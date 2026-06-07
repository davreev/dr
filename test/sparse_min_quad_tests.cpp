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
void solve_common(int* const utest_result)
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

    using Solver = SparseMinQuadFixed<f64, i32, SolverType_Iterative>;
    Solver solver{};

    for (auto const& tc : solve_test_cases)
    {
        isize const n = tc.b.size();
        SparseMat<f64> A(n, n);
        A.setFromTriplets(tc.coeffs.begin(), tc.coeffs.end());

        solver.init(A, [&](i32 const i) -> bool {
            return tc.is_fixed[i];
        });

        DynamicArray<f64> x = tc.expect.x;
        solver.solve(as_vec(as_span(tc.b)), as_vec(as_span(x)));

        ASSERT_TRUE(all_near_equal(as_span(tc.expect.x), as_span(x).as_const(), eps));
    }
}

} // namespace
} // namespace dr

UTEST(sparse_min_quad, solve_direct) { dr::solve_common<false>(utest_result); }

UTEST(sparse_min_quad, solve_iterative) { dr::solve_common<true>(utest_result); }

/*
    Compile-time checks
*/

namespace dr
{

/*
    Explicit instantiation of templates to catch compile errors
*/

template struct SparseMinQuadFixed<f64, i32, SolverType_Direct>;

template struct SparseMinQuadFixed<f64, i32, SolverType_Iterative>;

template bool SparseMinQuadFixed<f64, i32, SolverType_Direct>::init<bool (*)(i32)>(
    SparseMat<f64, i32> const&,
    bool (*&&)(i32));

template bool SparseMinQuadFixed<f64, i32, SolverType_Iterative>::init<bool (*)(i32)>(
    SparseMat<f64, i32> const&,
    bool (*&&)(i32));

template void SparseMinQuadFixed<f64, i32, SolverType_Direct>::solve<Mat<f64>, Mat<f64>>(
    MatExpr<Mat<f64>> const&,
    MatExpr<Mat<f64>>&);

template void SparseMinQuadFixed<f64, i32, SolverType_Iterative>::solve<Mat<f64>, Mat<f64>>(
    MatExpr<Mat<f64>> const&,
    MatExpr<Mat<f64>>&);

template void SparseMinQuadFixed<f64, i32, SolverType_Direct>::solve<Mat<f64>, Mat<f64>>(
    MatExpr<Mat<f64>> const&,
    MatView<Mat<f64>>);

template void SparseMinQuadFixed<f64, i32, SolverType_Iterative>::solve<Mat<f64>, Mat<f64>>(
    MatExpr<Mat<f64>> const&,
    MatView<Mat<f64>>);

template void SparseMinQuadFixed<f64, i32, SolverType_Direct>::solve<Mat<f64>>(MatExpr<Mat<f64>>&);

template void SparseMinQuadFixed<f64, i32, SolverType_Iterative>::solve<Mat<f64>>(
    MatExpr<Mat<f64>>&);

template void SparseMinQuadFixed<f64, i32, SolverType_Direct>::solve<Mat<f64>>(MatView<Mat<f64>>);

template void SparseMinQuadFixed<f64, i32, SolverType_Iterative>::solve<Mat<f64>>(
    MatView<Mat<f64>>);

} // namespace dr
