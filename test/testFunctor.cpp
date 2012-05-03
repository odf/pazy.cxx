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

        long theAnswer()
        {
            return 42;
        }

        double times(int i, double d)
        {
            return i * d;
        }
    };

    Plus f;

    double plus(int i, double d)
    {
        return i + d;
    }

    TEST(FunctorCall)
    {
        Functor<double, TYPELIST_2(int, double)> cmd(f);
        CHECK_EQUAL(3.14, cmd(3, .14));
        CHECK_EQUAL(4, cmd(3, 1));
    }

    TEST(FunctionCall)
    {
        CHECK_EQUAL(3.14, bind(plus)(3, .14));
        CHECK_EQUAL(4, bind(plus)(3, 1));
    }

    TEST(MemberFunctionCall)
    {
        CHECK_EQUAL(42, bind(&Plus::theAnswer)(f));
        CHECK_EQUAL(1.0, bind(&Plus::times)(f, 5, 0.2));
    }

    TEST(Binder)
    {
        CHECK_EQUAL(3.14, bind(plus, 3)(.14));
        CHECK_EQUAL(3.14, bind(plus, 3, .14)());
        CHECK_EQUAL(42, bind(&Plus::theAnswer, f)());
        CHECK_EQUAL(1.0, bind(&Plus::times, f, 5)(0.2));
        CHECK_EQUAL(1.0, bind(&Plus::times, f, 5, 0.2)());
    }

    TEST(Composer)
    {
        CHECK_EQUAL(47, compose(bind(plus, 5), &Plus::theAnswer)(f));
        CHECK_EQUAL(256, compose(bind(&Plus::times, f, 2),
                                 bind(plus, -13),
                                 bind(&Plus::times, f, 3),
                                 bind(plus, 5),
                                 &Plus::theAnswer)(f));
    }
}

int main()
{
    return UnitTest::RunAllTests();
}
