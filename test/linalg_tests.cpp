#include <utest.h>

#include <dr/linalg_reshape.hpp>
#include <dr/linalg_types.hpp>
#include <dr/math_ctors.hpp>
#include <dr/math_types.hpp>

UTEST(linalg_reshape, alignment)
{
    using namespace dr;

    {
        f64 const arr[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
        ASSERT_TRUE(is_aligned<Vec3<f64>>(arr));
        ASSERT_TRUE(is_aligned<Vec3<f64>>(arr + 1));
        ASSERT_TRUE(is_aligned<Vec3<f64>>(arr + 3));
    }

    {
        alignas(Vec4<f64>) f64 const arr[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
        ASSERT_TRUE(is_aligned<Vec4<f64>>(arr));
        ASSERT_FALSE(is_aligned<Vec4<f64>>(arr + 1));
        ASSERT_TRUE(is_aligned<Vec4<f64>>(arr + 4));
    }
}

UTEST(linalg_reshape, scals_to_vecs)
{
    using namespace dr;

    alignas(Vec2<f64>) f64 const scals[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    static_assert(alignof(Covec3<f64>) < alignof(Vec2<f64>));

    {
        Span<Vec2<f64> const> const vecs = as<Vec2<f64>>(as_span(scals));
        ASSERT_EQ(3, vecs.size());

        ASSERT_EQ(scals[4], vecs[2].x());
        ASSERT_EQ(scals[5], vecs[2].y());
    }

    {
        Span<Covec3<f64> const> const covecs = as<Covec3<f64>>(as_span(scals));
        ASSERT_EQ(2, covecs.size());

        ASSERT_EQ(scals[3], covecs[1].x());
        ASSERT_EQ(scals[4], covecs[1].y());
        ASSERT_EQ(scals[5], covecs[1].z());
    }
}

UTEST(linalg_reshape, vecs_to_scals)
{
    using namespace dr;

    {
        Vec2<f64> const vecs[] = {
            {0.0, 0.1},
            {1.0, 1.1},
            {2.0, 2.1},
        };

        Span<f64 const> const scals = as<f64>(as_span(vecs));
        ASSERT_EQ(6, scals.size());

        ASSERT_EQ(vecs[2].x(), scals[4]);
        ASSERT_EQ(vecs[2].y(), scals[5]);
    }

    {
        Covec3<f64> const covecs[] = {
            {0.0, 0.1, 0.2},
            {1.0, 1.1, 1.2},
        };

        Span<f64 const> const scals = as<f64>(as_span(covecs));
        ASSERT_EQ(6, scals.size());

        ASSERT_EQ(covecs[1].x(), scals[3]);
        ASSERT_EQ(covecs[1].y(), scals[4]);
        ASSERT_EQ(covecs[1].z(), scals[5]);
    }
}

UTEST(linalg_reshape, scals_to_mat)
{
    using namespace dr;

    // clang-format off
    f64 const scals[] = {
        0.0, 0.1, 0.2, 
        1.1, 1.1, 1.2, 
        2.0, 2.1, 2.2,
        3.0, 3.1, 3.2,
    };
    // clang-format on

    {
        auto mat = as_mat(as_span(scals), 3);
        ASSERT_EQ(3, mat.rows());
        ASSERT_EQ(4, mat.cols());
        ASSERT_EQ(scals[11], mat(2, 3));
    }

    {
        auto mat = as_mat(as_span(scals), 4);
        ASSERT_EQ(4, mat.rows());
        ASSERT_EQ(3, mat.cols());
        ASSERT_EQ(scals[11], mat(3, 2));
    }
}

UTEST(linalg_reshape, vecs_to_mat)
{
    using namespace dr;

    {
        Vec2<f64> const vecs[] = {
            {0.0, 0.1},
            {1.0, 1.1},
            {2.0, 2.1},
        };

        auto mat = as_mat(as_span(vecs));
        ASSERT_EQ(2, mat.rows());
        ASSERT_EQ(3, mat.cols());
        ASSERT_EQ(vecs[2].y(), mat(1, 2));
    }

    {
        Covec3<f64> const covecs[] = {
            {0.0, 0.1, 0.2},
            {1.0, 1.1, 1.2},
        };

        auto mat = as_mat(as_span(covecs));
        ASSERT_EQ(2, mat.rows());
        ASSERT_EQ(3, mat.cols());
        ASSERT_EQ(covecs[1].z(), mat(1, 2));
    }
}

UTEST(linalg_reshape, unpack)
{
    using namespace dr;

    {
        Vec2<i32> const a{1, 2};
        Vec2<i32> const b{3, 4};
        auto const [x, y] = unpack(a + b);
        ASSERT_EQ(4, x);
        ASSERT_EQ(6, y);
    }

    {
        Covec2<i32> const a{1, 2};
        Covec2<i32> const b{3, 4};
        auto const [x, y] = unpack(a + b);
        ASSERT_EQ(4, x);
        ASSERT_EQ(6, y);
    }

    {
        Mat<i32, 2, 2> const A = mat(vec(1, 2), vec(3, 4));
        auto const [a00, a10, a01, a11] = unpack(A + A);
        ASSERT_EQ(2, a00);
        ASSERT_EQ(4, a10);
        ASSERT_EQ(6, a01);
        ASSERT_EQ(8, a11);
    }
}

/*
    Compile-time checks
*/

namespace dr
{
namespace
{

[[maybe_unused]]
void check_default_mat_options()
{
    static_assert(default_mat_options<3, 3>() == Eigen::ColMajor);
    static_assert(default_mat_options<3, 1>() == Eigen::ColMajor);
    static_assert(default_mat_options<1, 3>() == Eigen::RowMajor);
    static_assert(default_mat_options<1, 1>() == Eigen::ColMajor);
}

} // namespace
} // namespace dr