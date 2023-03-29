#pragma once

#include <Eigen/SparseCholesky>

#include <dr/linalg_reshape.hpp>
#include <dr/sparse_linalg_types.hpp>

namespace dr
{

/// Minimizes a convex quadratic objective with fixed value constraints
template <typename Scalar, typename Index = i32>
struct SparseMinQuadFixed
{
    // NOTE: Can assume semidefinite matrix if objective is convex quadratic
    using Solver = Eigen::SimplicialLDLT<SparseMat<Scalar, Index>>;

    /// Isolates unknown variables and performs decomposition
    template <typename Predicate>
    bool init(SparseMat<Scalar, Index> const& A, Predicate&& is_fixed)
    {
        // A should be square
        assert(A.rows() == A.cols());

        // Isolate unknown variables
        {
            perm_.resize(A.rows());
            n_[0] = make_permutation(is_fixed, as_span(perm_));
            n_[1] = A.cols() - n_[0];

            // A_ = Pt A P
            A_ = A.twistedBy(perm_.asPermutation().transpose());
        }

        // Prefactor block of A_ corresponding with unknown variables
        solver_.compute(A_.topLeftCorner(n_[0], n_[0]));
        return is_init_ = (solver_.info() == Eigen::Success);
    }

    /// Solves Ax = b
    template <typename DerivedB, typename DerivedX>
    void solve(MatBase<DerivedB> const& b, MatBase<DerivedX>& x)
    {
        assert(is_init_);
        auto const P = perm_.asPermutation();

        x_.noalias() = P.transpose() * x;

        b_.noalias() = (P.transpose() * b).topRows(n_[0]) - A_.topRightCorner(n_[0], n_[1]) * x_.bottomRows(n_[1]);
        x_.topRows(n_[0]) = solver_.solve(b_);

        x.noalias() = P * x_;
    }

    /// Solves Ax = 0
    template <typename Derived>
    void solve_zero(MatBase<Derived>& x)
    {
        assert(is_init_);
        auto const P = perm_.asPermutation();

        x_.noalias() = P.transpose() * x;
        b_.noalias() = A_.topRightCorner(n_[0], n_[1]) * x_.bottomRows(n_[1]);

        x_.topRows(n_[0]) = solver_.solve(-b_);

        x.noalias() = P * x_;
    }

    /// True if the solver has been successfully initialized
    bool is_init() const { return is_init_; }

    /// Returns the underlying solver
    Solver const& solver() const { return solver_; }

    /// Returns the permutation used to isolate unknown variables
    VecN<Index> const& perm() const { return perm_; }

  private:
    Solver solver_{};
    SparseMat<Scalar, Index> A_{};
    MatN<Scalar> x_{};
    MatN<Scalar> b_{};
    VecN<Index> perm_{};
    Index n_[2];
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
};

} // namespace dr