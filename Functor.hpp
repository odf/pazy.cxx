// Functor library following Alexandrescu's "Modern C++ Design"

#ifndef ODF_FUNCTOR
#define ODF_FUNCTOR

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

template<typename T, class TL>
struct Prepend
{
    typedef Typelist<T, TL> Result;
};

template<class TL>
struct Prepend<NullType, TL>
{
    typedef TL Result;
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
    virtual ~FunctorImpl() {}
};

template<typename R, typename A>
class FunctorImpl<R, TYPELIST_1(A)>
{
public:
    virtual R operator()(A) = 0;
    virtual ~FunctorImpl() {}
};

template<typename R, typename A, typename B>
class FunctorImpl<R, TYPELIST_2(A, B)>
{
public:
    virtual R operator()(A, B) = 0;
    virtual ~FunctorImpl() {}
};

template<typename R, typename A, typename B, typename C>
class FunctorImpl<R, TYPELIST_3(A, B, C)>
{
public:
    virtual R operator()(A, B, C) = 0;
    virtual ~FunctorImpl() {}
};


// Default functor handlers

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


// Functor handlers for member functions

template<class Parent, typename F>
class MemFnHandler : public FunctorImpl<typename Parent::result_type,
                                        typename Parent::arg_list>
{
public:
    typedef typename Parent::result_type result_type;

    MemFnHandler(F const& fun)
        : fun_(fun)
    {
    }

    result_type operator()(typename Parent::arg1_type a1)
    {
        return ((a1).*(fun_))();
    }

    result_type operator()(typename Parent::arg1_type a1,
                           typename Parent::arg2_type a2)
    {
        return ((a1).*(fun_))(a2);
    }

    result_type operator()(typename Parent::arg1_type a1,
                           typename Parent::arg2_type a2,
                           typename Parent::arg3_type a3)
    {
        return ((a1).*(fun_))(a2, a3);
    }

    result_type operator()(typename Parent::arg1_type a1,
                           typename Parent::arg2_type a2,
                           typename Parent::arg3_type a3,
                           typename Parent::arg4_type a4)
    {
        return ((a1).*(fun_))(a2, a3, a4);
    }

private:
    F fun_;
};


// Wrapper class for tagging member functions

template<typename F>
struct MemFnWrapper
{
    typedef F base_type;

    MemFnWrapper(F const& fun)
        : value(fun)
    {
    }

    F value;
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

    template<typename F>
    Functor(MemFnWrapper<F> const& fun)
        : impl_(new MemFnHandler<Functor, F>(fun.value))
    {
    }

    Functor(boost::shared_ptr<Impl> impl, int const dummy)
        : impl_(impl)
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


// Binding

template<class Base>
class Binder
    : public FunctorImpl<typename Base::result_type,
                         typename Base::arg_list::Tail>
{
    typedef typename Base::arg1_type A;

public:
    typedef typename Base::result_type result_type;

    Binder(Base const& fun, A arg1)
        : fun_(fun),
          arg1_(arg1)
    {
    }

    result_type operator()()
    {
        return fun_(arg1_);
    }

    result_type operator()(typename Base::arg2_type arg2)
    {
        return fun_(arg1_, arg2);
    }

    result_type operator()(typename Base::arg2_type arg2,
                           typename Base::arg3_type arg3)
    {
        return fun_(arg1_, arg2, arg3);
    }

    result_type operator()(typename Base::arg2_type arg2,
                           typename Base::arg3_type arg3,
                           typename Base::arg4_type arg4)
    {
        return fun_(arg1_, arg2, arg3, arg4);
    }

private:
    Base fun_;
    A arg1_;
};


// Composition

template<class Fun1, class Fun2>
class Composer
    : public FunctorImpl<typename Fun1::result_type,
                         typename TL::Prepend<typename Fun2::arg_list::Head,
                                              typename Fun1::arg_list::Tail>
                         ::Result>
{
public:
    typedef typename Fun1::result_type result_type;

    Composer(Fun1 const& fun1, Fun2 const& fun2)
        : fun1_(fun1),
          fun2_(fun2)
    {
    }

    result_type operator()()
    {
        return fun1_(fun2_());
    }

    result_type operator()(typename Fun2::arg1_type arg1)
    {
        return fun1_(fun2_(arg1));
    }

    result_type operator()(typename Fun2::arg1_type arg1,
                           typename Fun1::arg2_type arg2)
    {
        return fun1_(fun2_(arg1), arg2);
    }

    result_type operator()(typename Fun2::arg1_type arg1,
                           typename Fun1::arg2_type arg2,
                           typename Fun1::arg3_type arg3)
    {
        return fun1_(fun2_(arg1), arg2, arg3);
    }

    result_type operator()(typename Fun2::arg1_type arg1,
                           typename Fun1::arg2_type arg2,
                           typename Fun1::arg3_type arg3,
                           typename Fun1::arg4_type arg4)
    {
        return fun1_(fun2_(arg1), arg2, arg3, arg4);
    }

private:
    Fun1 fun1_;
    Fun2 fun2_;
};

// Function traits

template<typename F>
struct function_traits
{
    typedef F wrapper_type;
};

template<typename R>
struct function_traits<R(*)()>
{
    typedef R result_type;
    typedef NullType arg_list;

    typedef Functor<result_type, arg_list> functor_type;
};

template<typename R>
struct function_traits<R()> : function_traits<R (*)()>
{
    typedef R (*wrapper_type)();
};

template<typename R, typename A>
struct function_traits<R(*)(A)>
{
    typedef R result_type;
    typedef TYPELIST_1(A) arg_list;

    typedef A arg1_type;

    typedef Functor<result_type, arg_list> functor_type;
};

template<typename R, typename A>
struct function_traits<R(A)> : function_traits<R (*)(A)>
{
    typedef R (*wrapper_type)(A);
};

template<typename R, typename A, typename B>
struct function_traits<R(*)(A, B)>
{
    typedef R result_type;
    typedef TYPELIST_2(A, B) arg_list;

    typedef A arg1_type;
    typedef B arg2_type;

    typedef Functor<result_type, arg_list> functor_type;
};

template<typename R, typename A, typename B>
struct function_traits<R(A, B)> : function_traits<R (*)(A, B)>
{
    typedef R (*wrapper_type)(A, B);
};

template<typename R, typename A, typename B, typename C>
struct function_traits<R(*)(A, B, C)>
{
    typedef R result_type;
    typedef TYPELIST_3(A, B, C) arg_list;

    typedef A arg1_type;
    typedef B arg2_type;
    typedef C arg3_type;

    typedef Functor<result_type, arg_list> functor_type;
};

template<typename R, typename A, typename B, typename C>
struct function_traits<R(A, B, C)> : function_traits<R (*)(A, B, C)>
{
    typedef R (*wrapper_type)(A, B, C);
};

template<class K, typename R>
struct function_traits<R(K::*)()>
{
    typedef R result_type;
    typedef TYPELIST_1(K) arg_list;

    typedef K arg1_type;

    typedef MemFnWrapper<R(K::*)()> wrapper_type;
    typedef Functor<result_type, arg_list> functor_type;
};

template<class K, typename R>
struct function_traits<R(K::*)() const>
{
    typedef R result_type;
    typedef TYPELIST_1(K) arg_list;

    typedef K arg1_type;

    typedef MemFnWrapper<R(K::*)() const> wrapper_type;
    typedef Functor<result_type, arg_list> functor_type;
};

template<class K, typename R, typename A>
struct function_traits<R(K::*)(A)>
{
    typedef R result_type;
    typedef TYPELIST_2(K, A) arg_list;

    typedef K arg1_type;
    typedef A arg2_type;

    typedef MemFnWrapper<R(K::*)(A)> wrapper_type;
    typedef Functor<result_type, arg_list> functor_type;
};

template<class K, typename R, typename A>
struct function_traits<R(K::*)(A) const>
{
    typedef R result_type;
    typedef TYPELIST_2(K, A) arg_list;

    typedef K arg1_type;
    typedef A arg2_type;

    typedef MemFnWrapper<R(K::*)(A) const> wrapper_type;
    typedef Functor<result_type, arg_list> functor_type;
};

template<class K, typename R, typename A, typename B>
struct function_traits<R(K::*)(A, B)>
{
    typedef R result_type;
    typedef TYPELIST_3(K, A, B) arg_list;

    typedef K arg1_type;
    typedef A arg2_type;
    typedef B arg3_type;

    typedef MemFnWrapper<R(K::*)(A, B)> wrapper_type;
    typedef Functor<result_type, arg_list> functor_type;
};

template<class K, typename R, typename A, typename B>
struct function_traits<R(K::*)(A, B) const>
{
    typedef R result_type;
    typedef TYPELIST_3(K, A, B) arg_list;

    typedef K arg1_type;
    typedef A arg2_type;
    typedef B arg3_type;

    typedef MemFnWrapper<R(K::*)(A, B) const> wrapper_type;
    typedef Functor<result_type, arg_list> functor_type;
};

template<typename R, class TList>
struct function_traits<Functor<R, TList> >
{
    typedef R result_type;
    typedef TList arg_list;

    typedef Functor<R, TList> wrapper_type;
    typedef wrapper_type functor_type;

    typedef typename functor_type::arg1_type arg1_type;
    typedef typename functor_type::arg2_type arg2_type;
    typedef typename functor_type::arg3_type arg3_type;
    typedef typename functor_type::arg4_type arg4_type;
    typedef typename functor_type::arg5_type arg5_type;
};


// Convenience functions for creating functors and binding arguments

template<typename F>
typename function_traits<F>::functor_type bind(F const& fun)
{
    return static_cast<typename function_traits<F>::wrapper_type>(fun);
}

template<typename F>
Functor<typename function_traits<F>::result_type,
        typename function_traits<F>::arg_list::Tail>
bind(F const& fun, typename function_traits<F>::arg1_type arg)
{
    typedef typename function_traits<F>::wrapper_type         wrapper_type;
    typedef typename function_traits<F>::result_type          result_type;
    typedef typename function_traits<F>::arg_list::Tail       arg_list;
    typedef Binder<typename function_traits<F>::functor_type> binder_type;

    return Functor<result_type, arg_list>(
        boost::shared_ptr<binder_type>(
            new binder_type(static_cast<wrapper_type>(fun), arg)),
        1);
}

template<typename F>
Functor<typename function_traits<F>::result_type,
        typename function_traits<F>::arg_list::Tail::Tail>
bind(F const& fun,
     typename function_traits<F>::arg1_type arg1,
     typename function_traits<F>::arg2_type arg2)
{
    return bind(bind(fun, arg1), arg2);
}

template<typename F>
Functor<typename function_traits<F>::result_type,
        typename function_traits<F>::arg_list::Tail::Tail::Tail>
bind(F const& fun,
     typename function_traits<F>::arg1_type arg1,
     typename function_traits<F>::arg2_type arg2,
     typename function_traits<F>::arg3_type arg3)
{
    return bind(bind(fun, arg1, arg2), arg3);
}


// Convenience functions for functor composition

template<typename F1, typename F2>
Functor<typename function_traits<F1>::result_type,
        typename TL::Prepend<typename function_traits<F2>::arg_list::Head,
                             typename function_traits<F1>::arg_list::Tail>
        ::Result>
compose(F1 const& fun1, F2 const& fun2)
{
    typedef typename function_traits<F1>::wrapper_type wrapper1;
    typedef typename function_traits<F2>::wrapper_type wrapper2;

    typedef typename function_traits<F1>::result_type    result_type;
    typedef typename function_traits<F2>::arg1_type      arg1_type;
    typedef typename function_traits<F1>::arg_list::Tail rest_args;

    typedef typename TL::Prepend<arg1_type, rest_args>::Result arg_list;

    typedef Composer<typename function_traits<F1>::functor_type,
                     typename function_traits<F2>::functor_type> composer_type;

    return Functor<result_type, arg_list>(
        boost::shared_ptr<composer_type>(
            new composer_type(static_cast<wrapper1>(fun1),
                              static_cast<wrapper2>(fun2))),
        1);
}

template<typename F1, typename F2, typename F3>
Functor<typename function_traits<F1>::result_type,
        typename TL::Prepend<typename function_traits<F3>::arg_list::Head,
                             typename function_traits<F1>::arg_list::Tail>
        ::Result>
compose(F1 const& fun1, F2 const& fun2, F3 const& fun3)
{
    return compose(fun1, compose(fun2, fun3));
}

template<typename F1, typename F2, typename F3, typename F4>
Functor<typename function_traits<F1>::result_type,
        typename TL::Prepend<typename function_traits<F4>::arg_list::Head,
                             typename function_traits<F1>::arg_list::Tail>
        ::Result>
compose(F1 const& fun1, F2 const& fun2, F3 const& fun3, F4 const& fun4)
{
    return compose(fun1, compose(fun2, fun3, fun4));
}

template<typename F1, typename F2, typename F3, typename F4, typename F5>
Functor<typename function_traits<F1>::result_type,
        typename TL::Prepend<typename function_traits<F5>::arg_list::Head,
                             typename function_traits<F1>::arg_list::Tail>
        ::Result>
compose(F1 const& fun1, F2 const& fun2,
        F3 const& fun3, F4 const& fun4, F5 const& fun5)
{
    return compose(fun1, compose(fun2, fun3, fun4, fun5));
}


// Some useful functions

template<typename T>
T identity(const T val)
{
    return val;
}

template<typename T>
inline typename function_traits<T(*)(T)>::currier_type constant(const T val)
{
    return bind(identity<T>, val);
}

#endif // ODF_FUNCTOR
