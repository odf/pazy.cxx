/* -*-c++-*- */

#include <string>
#include <sstream>
#include <typeinfo>

#include <unittest++/UnitTest++.h>

#include "Functor.hpp"

SUITE(Typelist)
{
    typedef TYPELIST_4(double, int, long, char) SomeType;

    TEST(Length)
    {
        CHECK_EQUAL(4, TL::Length<SomeType>::value);
    }

    TEST(TypeAt)
    {
        CHECK_EQUAL(typeid(long).name(),
                    typeid(TL::TypeAt<SomeType, 2>::Result).name());
        CHECK_EQUAL(typeid(NullType).name(),
                    typeid(TL::TypeAt<SomeType, 4>::Result).name());
    }
}

SUITE(Functor)
{
    struct Plus
    {
        double operator()(int i, double d)
        {
            return i + d;
        }
    };

    double plus(int i, double d)
    {
        return i + d;
    }

    TEST(FunctorCall)
    {
        Plus f;
        Functor<double, TYPELIST_2(int, double)> cmd(f);
        CHECK_EQUAL(3.14, cmd(3, .14));
        CHECK_EQUAL(4, cmd(3, 1));
    }

    TEST(FunctionCall)
    {
        Functor<double, TYPELIST_2(int, double)> cmd(&plus);
        CHECK_EQUAL(3.14, cmd(3, .14));
        CHECK_EQUAL(4, cmd(3, 1));
    }
}

int main()
{
    return UnitTest::RunAllTests();
}
