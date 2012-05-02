// Functor library following Alexandrescu's "Modern C++ Design"

#include "boost/smart_ptr.hpp"

// Typelists

class NullType {};

template<class T, class R>
struct Typelist
{
    typedef T Head;
    typedef R Tail;
};

#define TYPELIST_1(T1) Typelist<T1, NullType>
#define TYPELIST_2(T1, T2) Typelist<T1, TYPELIST_1(T2)>
#define TYPELIST_3(T1, T2, T3) Typelist<T1, TYPELIST_2(T2, T3)>
#define TYPELIST_4(T1, T2, T3, T4) Typelist<T1, TYPELIST_3(T2, T3, T4)>
#define TYPELIST_5(T1, T2, T3, T4, T5) Typelist<T1, TYPELIST_4(T2, T3, T4, T5)>

namespace TL
{

template<class TList>
struct Length;

template<>
struct Length<NullType>
{
    enum { value = 0 };
};

template<class T, class R>
struct Length<Typelist<T, R> >
{
    enum { value = 1 + Length<R>::value };
};

template<class TList, unsigned int i>
struct TypeAt;

template<unsigned int i>
struct TypeAt<NullType, i>
{
    typedef NullType Result;
};

template<class Head, class Tail>
struct TypeAt<Typelist<Head, Tail>, 0>
{
    typedef Head Result;
};

template<class Head, class Tail, unsigned int i>
struct TypeAt<Typelist<Head, Tail>, i>
{
    typedef typename TypeAt<Tail, i-1>::Result Result;
};

}


// Functor implementation classes

template<typename R, class TList>
class FunctorImpl;

template<typename R>
class FunctorImpl<R, NullType>
{
public:
    virtual R operator()() = 0;
    virtual FunctorImpl* Clone() const = 0;
    virtual ~FunctorImpl() {}
};

template<typename R, typename A>
class FunctorImpl<R, TYPELIST_1(A)>
{
public:
    virtual R operator()(A) = 0;
    virtual FunctorImpl* Clone() const = 0;
    virtual ~FunctorImpl() {}
};

template<typename R, typename A, typename B>
class FunctorImpl<R, TYPELIST_2(A, B)>
{
public:
    virtual R operator()(A, B) = 0;
    virtual FunctorImpl* Clone() const = 0;
    virtual ~FunctorImpl() {}
};

template<typename R, typename A, typename B, typename C>
class FunctorImpl<R, TYPELIST_3(A, B, C)>
{
public:
    virtual R operator()(A, B, C) = 0;
    virtual FunctorImpl* Clone() const = 0;
    virtual ~FunctorImpl() {}
};


// Functor handlers

template<class Parent, typename F>
class FunctorHandler : public FunctorImpl<typename Parent::result_type,
                                          typename Parent::arg_list>
{
public:
    typedef typename Parent::result_type result_type;

    FunctorHandler(F const& fun)
        : fun_(fun)
    {
    }

    FunctorHandler* Clone() const
    {
        return new FunctorHandler(*this);
    }

    result_type operator()()
    {
        return fun_();
    }

    result_type operator()(typename Parent::arg1_type a1)
    {
        return fun_(a1);
    }

    result_type operator()(typename Parent::arg1_type a1,
                           typename Parent::arg2_type a2)
    {
        return fun_(a1, a2);
    }

    result_type operator()(typename Parent::arg1_type a1,
                           typename Parent::arg2_type a2,
                           typename Parent::arg3_type a3)
    {
        return fun_(a1, a2, a3);
    }

    result_type operator()(typename Parent::arg1_type a1,
                           typename Parent::arg2_type a2,
                           typename Parent::arg3_type a3,
                           typename Parent::arg4_type a4)
    {
        return fun_(a1, a2, a3, a4);
    }

private:
    F fun_;
};


// Functor classes

template<typename R, class TList>
class Functor
{
    typedef FunctorImpl<R, TList> Impl;

public:
    typedef TList arg_list;

    typedef R result_type;
    
    typedef typename TL::TypeAt<TList, 0>::Result arg1_type;
    typedef typename TL::TypeAt<TList, 1>::Result arg2_type;
    typedef typename TL::TypeAt<TList, 2>::Result arg3_type;
    typedef typename TL::TypeAt<TList, 3>::Result arg4_type;
    typedef typename TL::TypeAt<TList, 4>::Result arg5_type;

    Functor()
    {
    }

    template<typename F>
    Functor(F const& fun)
        : impl_(new FunctorHandler<Functor, F>(fun))
    {
    }

    R operator()()
    {
        return (*impl_)();
    }

    R operator()(arg1_type a1)
    {
        return (*impl_)(a1);
    }

    R operator()(arg1_type a1, arg2_type a2)
    {
        return (*impl_)(a1, a2);
    }

    R operator()(arg1_type a1, arg2_type a2, arg3_type a3)
    {
        return (*impl_)(a1, a2, a3);
    }

    R operator()(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4)
    {
        return (*impl_)(a1, a2, a3, a4);
    }

private:
    boost::shared_ptr<Impl> impl_;
};
