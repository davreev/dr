#include <utest.h>

#include <dr/math_types.hpp>
#include <dr/meta.hpp>

UTEST(type_pack, append_prepend)
{
    using namespace dr;

    using TypesA = TypePack<i16, i32>;

    using TypesB = TypesA::append<i64>;
    static_assert(std::is_same_v<TypesB::at<2>, i64>);
    static_assert(TypesB::size == 3);

    using TypesC = TypesB::prepend<i8>;
    static_assert(std::is_same_v<TypesC::at<0>, i8>);
    static_assert(TypesC::size == 4);

    ASSERT_TRUE(true);
}

UTEST(type_pack, join)
{
    using namespace dr;

    using TypesA = TypePack<i8, i16>;
    using TypesB = TypePack<u8, u16>;

    using TypesAB = TypesA::join<TypesB>;
    static_assert(TypesAB::includes<i8>);
    static_assert(TypesAB::includes<i16>);
    static_assert(TypesAB::includes<u8>);
    static_assert(TypesAB::includes<u16>);

    ASSERT_TRUE(true);
}

UTEST(value_pack, append_prepend)
{
    using namespace dr;

    using ValuesA = ValuePack<i32, 1, 2>;
    
    using ValuesB = ValuesA::append<3>;
    static_assert(ValuesB::at<2> == 3);
    static_assert(ValuesB::size == 3);

    using ValuesC = ValuesB::prepend<0>;
    static_assert(ValuesC::at<0> == 0);
    static_assert(ValuesC::size == 4);

    ASSERT_TRUE(true);
}

UTEST(value_pack, join)
{
    using namespace dr;

    using TypesA = ValuePack<i32, 0, 1>;
    using TypesB = ValuePack<i32, 2, 3>;

    using TypesAB = TypesA::join<TypesB>;
    static_assert(TypesAB::includes<0>);
    static_assert(TypesAB::includes<1>);
    static_assert(TypesAB::includes<2>);
    static_assert(TypesAB::includes<3>);

    ASSERT_TRUE(true);
}
