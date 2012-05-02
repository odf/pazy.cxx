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
    struct TestFunctor
    {
        void operator()(int i, double d)
        {
            std::cout << "TestFunctor::operator()(" << i
                      << ", " << d << ") called." << std::endl;
        }
    };

    TEST(SimpleCall)
    {
        TestFunctor f;
        Functor<void, TYPELIST_2(int, double)> cmd(f);
        cmd(4, 5);
    }
}

int main()
{
    return UnitTest::RunAllTests();
}
