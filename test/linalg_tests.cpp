#include <utest.h>

#include <dr/linalg_reshape.hpp>
#include <dr/linalg_types.hpp>
#include <dr/math_types.hpp>

UTEST(linalg, default_mat_options)
{
    using namespace dr;

    {
        constexpr int options = default_mat_options<3, 3>;
        ASSERT_EQ(Eigen::ColMajor, options);
    }

    {
        constexpr int options = default_mat_options<3, 1>;
        ASSERT_EQ(Eigen::ColMajor, options);
    }

    {
        constexpr int options = default_mat_options<1, 3>;
        ASSERT_EQ(Eigen::RowMajor, options);
    }

    {
        constexpr int options = default_mat_options<1, 1>;
        ASSERT_EQ(Eigen::ColMajor, options);
    }
}

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

    f64 const scals[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

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