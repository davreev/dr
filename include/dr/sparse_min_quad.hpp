#pragma once

#include <type_traits>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCholesky>

#include <dr/linalg_reshape.hpp>
#include <dr/sparse_linalg_types.hpp>

namespace dr
{

template <typename T, typename Enable = void>
struct SelectSolver;

/// Minimizes a convex quadratic objective with fixed value constraints
template <typename Scalar, typename Index = i32, SolverType solver_type = SolverType_Direct>
struct SparseMinQuadFixed
{
    using Solver = typename SelectSolver<SparseMinQuadFixed>::Type;

    /// Isolates unknown variables and factorizes/preconditions the linear system
    template <typename Predicate>
    bool init(SparseMat<Scalar, Index> const& A, Predicate&& is_fixed)
    {
        static_assert(std::is_invocable_r_v<bool, Predicate, Index>);
        assert(A.rows() == A.cols());

        // Isolate unknown variables
        {
            perm_.resize(A.rows());
            n_[0] = make_permutation(is_fixed, as_span(perm_));
            n_[1] = A.cols() - n_[0];

            // A_ = PᵀAP
            A_ = A.twistedBy(perm_.asPermutation().transpose());
        }

        // Factorize/precondition the block of A_ corresponding with unknown variables
        solver_.compute(A_.topLeftCorner(n_[0], n_[0]));
        return is_init_ = (solver_.info() == Eigen::Success);
    }

    /// Minimizes xᵀAx + bᵀx (i.e. solves Ax = b)
    template <typename DerivedB, typename DerivedX>
    void solve(MatExpr<DerivedB> const& b, MatExpr<DerivedX>& x)
    {
        solve_impl(b, x);
    }

    /// Minimizes xᵀAx + bᵀx (i.e. solves Ax = b)
    template <typename DerivedB, typename DerivedX>
    void solve(MatExpr<DerivedB> const& b, MatView<DerivedX> x)
    {
        solve_impl(b, x);
    }

    /// Minimizes xᵀAx (i.e. solves Ax = 0)
    template <typename Derived>
    void solve(MatExpr<Derived>& x)
    {
        solve_impl(x);
    }

    /// Minimizes xᵀAx (i.e. solves Ax = 0)
    template <typename Derived>
    void solve(MatView<Derived> x)
    {
        solve_impl(x);
    }

    /// True if the solver has been successfully initialized
    bool is_init() const { return is_init_; }

    /// Returns the underlying solver
    Solver const& solver() const { return solver_; }

    /// Returns the permutation used to isolate unknown variables
    Vec<Index> const& perm() const { return perm_; }

  private:
    Solver solver_{};
    SparseMat<Scalar, Index> A_{};
    Mat<Scalar> x_{};
    Mat<Scalar> b_{};
    Vec<Index> perm_{};
    Index n_[2]{};
    bool is_init_{};

    template <typename Predicate>
    static Index make_permutation(Predicate&& is_fixed, Span<Index> const& result)
    {
        Index lo = 0;
        Index hi = result.size();

        for (Index i = 0; i < result.size(); ++i)
        {
            if (is_fixed(i))
                result[--hi] = i;
            else
                result[lo++] = i;
        }

        return lo;
    }

    template <typename DerivedB, typename DerivedX>
    void solve_impl(MatExpr<DerivedB> const& b, MatExpr<DerivedX>& x)
    {
        assert(is_init_);
        auto const P = perm_.asPermutation();

        // Create rhs
        x_.noalias() = P.transpose() * x;
        b_.noalias() = (P.transpose() * b).topRows(n_[0])
            - A_.topRightCorner(n_[0], n_[1]) * x_.bottomRows(n_[1]);

        // Solve
        x_.topRows(n_[0]) = solver_.solve(b_);
        x.noalias() = P * x_;
    }

    template <typename Derived>
    void solve_impl(MatExpr<Derived>& x)
    {
        assert(is_init_);
        auto const P = perm_.asPermutation();

        // Create rhs
        x_.noalias() = P.transpose() * x;
        b_.noalias() = A_.topRightCorner(n_[0], n_[1]) * x_.bottomRows(n_[1]);

        // Solve
        x_.topRows(n_[0]) = solver_.solve(-b_);
        x.noalias() = P * x_;
    }
};

template <typename Scalar, typename Index>
struct SelectSolver<SparseMinQuadFixed<Scalar, Index, SolverType_Direct>>
{
    using Type = Eigen::SimplicialLDLT<SparseMat<Scalar, Index>>;
};

template <typename Scalar, typename Index>
struct SelectSolver<SparseMinQuadFixed<Scalar, Index, SolverType_Iterative>>
{
    using Type = Eigen::ConjugateGradient<
        SparseMat<Scalar, Index>,
        Eigen::Lower | Eigen::Upper,
        Eigen::IncompleteCholesky<Scalar>>;
};

} // namespace dr
