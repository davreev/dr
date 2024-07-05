#include <utest.h>

#include <dr/linalg_reshape.hpp>
#include <dr/math.hpp>
#include <dr/math_ctors.hpp>
#include <dr/transform.hpp>

#include "test_utils.hpp"

UTEST(rigid3, apply_forward_inverse)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    for (isize i = 0; i < 32; ++i)
    {
        auto const a = Vec3<f64>::Random().eval();
        Rigid3<f64> const xform{{Quat<f64>::UnitRandom()}, Vec3<f64>::Random()};

        Vec3<f64> const b = xform.apply_inverse(xform.apply(a));
        ASSERT_TRUE(near_equal(a, b, eps));

        Vec3<f64> const c = (xform.inverse_to_matrix() * (xform.to_matrix() * a.homogeneous())).head<3>();
        ASSERT_TRUE(near_equal(a, c, eps));
    }
}

UTEST(rigid2, apply_forward_inverse)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    for (isize i = 0; i < 32; ++i)
    {
        auto const a = Vec2<f64>::Random().eval();

        Rigid2<f64> const xform{
            {Vec2<f64>::Random().normalized()},
            Vec2<f64>::Random(),
        };

        Vec2<f64> const b = xform.apply_inverse(xform.apply(a));
        ASSERT_TRUE(near_equal(a, b, eps));

        Vec2<f64> const c = (xform.inverse_to_matrix() * (xform.to_matrix() * a.homogeneous())).head<2>();
        ASSERT_TRUE(near_equal(a, c, eps));
    }
}

UTEST(conformal3, apply_forward_inverse)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    for (isize i = 0; i < 32; ++i)
    {
        auto const a = Vec3<f64>::Random().eval();

        auto const rand = 2.0 * Vec4<f64>::Random();
        Conformal3<f64> const xform{
            {Quat<f64>::UnitRandom()},
            rand.head<3>(),
            rand[3],
        };

        Vec3<f64> const b = xform.apply_inverse(xform.apply(a));
        ASSERT_TRUE(near_equal(a, b, eps));

        Vec3<f64> const c = (xform.inverse_to_matrix() * (xform.to_matrix() * a.homogeneous())).head<3>();
        ASSERT_TRUE(near_equal(a, c, eps));
    }
}

UTEST(conformal2, apply_forward_inverse)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    for (isize i = 0; i < 32; ++i)
    {
        auto const a = Vec2<f64>::Random().eval();

        auto const rand = 2.0 * Vec3<f64>::Random();
        Conformal2<f64> const xform{
            {Vec2<f64>::Random().normalized()},
            rand.head<2>(),
            rand[2],
        };

        Vec2<f64> const b = xform.apply_inverse(xform.apply(a));
        ASSERT_TRUE(near_equal(a, b, eps));

        Vec2<f64> const c = (xform.inverse_to_matrix() * (xform.to_matrix() * a.homogeneous())).head<2>();
        ASSERT_TRUE(near_equal(a, c, eps));
    }
}

UTEST(affine3, apply_forward_inverse)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    for (isize i = 0; i < 32; ++i)
    {
        auto const a = Vec3<f64>::Random().eval();
        Affine3<f64> const xform{Mat3<f64>::Random(), Vec3<f64>::Random()};

        Vec3<f64> const b = xform.inverse().apply(xform.apply(a));
        ASSERT_TRUE(near_equal(a, b, eps));

        Vec3<f64> const c = (xform.inverse_to_matrix() * (xform.to_matrix() * a.homogeneous())).head<3>();
        ASSERT_TRUE(near_equal(a, c, eps));
    }
}

UTEST(affine2, apply_forward_inverse)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    for (isize i = 0; i < 32; ++i)
    {
        auto const a = Vec2<f64>::Random().eval();
        Affine2<f64> const xform{Mat2<f64>::Random(), Vec2<f64>::Random()};

        Vec2<f64> const b = xform.inverse().apply(xform.apply(a));
        ASSERT_TRUE(near_equal(a, b, eps));

        Vec2<f64> const c = (xform.inverse_to_matrix() * (xform.to_matrix() * a.homogeneous())).head<2>();
        ASSERT_TRUE(near_equal(a, c, eps));
    }
}

UTEST(rigid3, apply_inverse_self)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    Rigid3<f64> identity{};

    for (i8 i = 0; i < 16; ++i)
    {
        Rigid3<f64> const a{{Quat<f64>::UnitRandom()}, Vec3<f64>::Random()};
        Rigid3<f64> const b = a.apply_inverse(a);

        ASSERT_TRUE(near_equal(b.rotation.q.coeffs(), identity.rotation.q.coeffs(), eps));
        ASSERT_TRUE(near_equal(b.translation, identity.translation, eps));
    }
}

UTEST(rigid2, apply_inverse_self)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    Rigid2<f64> identity{};

    for (i8 i = 0; i < 16; ++i)
    {
        Rigid2<f64> const a{{Vec2<f64>::Random().normalized()}, Vec2<f64>::Random()};
        Rigid2<f64> const b = a.apply_inverse(a);

        ASSERT_TRUE(near_equal(b.rotation.x, identity.rotation.x, eps));
        ASSERT_TRUE(near_equal(b.translation, identity.translation, eps));
    }
}

UTEST(conformal3, apply_inverse_self)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    Conformal3<f64> identity{};

    for (i8 i = 0; i < 16; ++i)
    {
        auto const rand = 2.0 * Vec4<f64>::Random();

        Conformal3<f64> const a{
            {Quat<f64>::UnitRandom()},
            rand.head<3>(),
            rand[3],
        };

        Conformal3<f64> const b = a.apply_inverse(a);

        ASSERT_TRUE(near_equal(b.rotation.q.coeffs(), identity.rotation.q.coeffs(), eps));
        ASSERT_TRUE(near_equal(b.translation, identity.translation, eps));
        ASSERT_TRUE(near_equal(b.scale, identity.scale, eps));
    }
}

UTEST(conformal2, apply_inverse_self)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    Conformal2<f64> identity{};

    for (i8 i = 0; i < 16; ++i)
    {
        auto const rand = 2.0 * Vec3<f64>::Random();

        Conformal2<f64> const a{
            {Vec2<f64>::Random().normalized()},
            rand.head<2>(),
            rand[2],
        };

        Conformal2<f64> const b = a.apply_inverse(a);

        ASSERT_TRUE(near_equal(b.rotation.x, identity.rotation.x, eps));
        ASSERT_TRUE(near_equal(b.translation, identity.translation, eps));
        ASSERT_TRUE(near_equal(b.scale, identity.scale, eps));
    }
}

UTEST(affine3, apply_inverse_self)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    Affine3<f64> const identity{};

    for (i8 i = 0; i < 16; ++i)
    {
        Affine3<f64> const a{
            Mat3<f64>::Random(),
            Vec3<f64>::Random(),
        };

        Affine3<f64> const b = a.inverse().apply(a);

        ASSERT_TRUE(all_near_equal(as_span(b.linear), as_span(identity.linear), eps));
        ASSERT_TRUE(all_near_equal(as_span(b.translation), as_span(identity.translation), eps));
    }
}

UTEST(affine2, apply_inverse_self)
{
    using namespace dr;

    constexpr f64 eps = 1.0e-8;

    Affine2<f64> const identity{};

    for (i8 i = 0; i < 16; ++i)
    {
        Affine2<f64> const a{
            Mat2<f64>::Random(),
            Vec2<f64>::Random(),
        };

        Affine2<f64> const b = a.inverse().apply(a);

        ASSERT_TRUE(all_near_equal(as_span(b.linear), as_span(identity.linear), eps));
        ASSERT_TRUE(all_near_equal(as_span(b.translation), as_span(identity.translation), eps));
    }
}
