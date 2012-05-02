#ifndef ODF_FUN_HPP
#define ODF_FUN_HPP 1

namespace odf
{

template<typename F>
struct function_traits;

template<typename F>
struct commonFunctionTraits
{
    typedef F wrapper_type;

    static inline wrapper_type wrap(const F fun)
    {
        return wrapper_type(fun);
    }
};

template<typename F>
struct unaryCurrier;

template<typename F>
struct binaryCurrier;

template<typename F>
struct ternaryCurrier;

template<typename Lft, typename Rgt>
struct unaryComposer;

template<typename Lft, typename Rgt>
struct binaryComposer;

template<typename Lft, typename Rgt>
struct ternaryComposer;

template<typename F>
struct nullaryFunctor
{
};

template<typename F>
struct nullaryFunctionTraits
{
    static const std::size_t arity = 0;
};

template<typename F>
struct function_traits<nullaryFunctor<F> > : nullaryFunctionTraits<F>
{
    typename F::result_type typedef result_type;
};

template<typename F>
struct unaryFunctor
{
};

template<typename F>
struct unaryFunctionTraits : commonFunctionTraits<F>
{
    static const std::size_t        arity = 1;
    typedef struct unaryCurrier<F>  currier_type;
    template<typename G>
    struct composer
    {
        typedef struct unaryComposer<F, G> type;
    };
};

template<typename F>
struct function_traits<unaryFunctor<F> > : unaryFunctionTraits<F>
{
    typename F::result_type typedef result_type;
    typename F::arg1_type   typedef arg1_type;
};

template<typename F>
struct binaryFunctor
{
};

template<typename F>
struct binaryFunctionTraits : commonFunctionTraits<F>
{
    static const std::size_t        arity = 2;
    typedef struct binaryCurrier<F> currier_type;
    template<typename G>
    struct composer
    {
        typedef struct binaryComposer<F, G> type;
    };
};

template<typename F>
struct function_traits<binaryFunctor<F> > : binaryFunctionTraits<F>
{
    typename F::result_type typedef result_type;
    typename F::arg1_type   typedef arg1_type;
    typename F::arg2_type   typedef arg2_type;
};

template<typename F>
struct ternaryFunctor
{
};

template<typename F>
struct ternaryFunctionTraits : commonFunctionTraits<F>
{
    static const std::size_t         arity = 3;
    typedef struct ternaryCurrier<F> currier_type;
    template<typename G>
    struct composer
    {
        typedef struct ternaryComposer<F, G> type;
    };
};

template<typename F>
struct function_traits<ternaryFunctor<F> > : ternaryFunctionTraits<F>
{
    typename F::result_type  typedef result_type;
    typename F::arg1_type    typedef arg1_type;
    typename F::arg2_type    typedef arg2_type;
    typename F::arg3_type    typedef arg3_type;
};

template<class K, typename F>
struct nullaryMemberFunctor : unaryFunctor<F>
{
    typename function_traits<F>::result_type typedef result_type;
    typedef K                                        arg1_type;

    nullaryMemberFunctor(const F fun) :
        fun(fun)
    {
    }

    const result_type operator() (K& obj) const
    {
        return ((obj).*(fun))();
    }

private:
    const F fun;
};

template<class K, typename F>
struct function_traits<nullaryMemberFunctor<K, F> > :
        function_traits<unaryFunctor<nullaryMemberFunctor<K, F> > >
{
};

template<class K, typename F>
struct nullaryConstMemberFunctor : unaryFunctor<F>
{
    typename function_traits<F>::result_type typedef result_type;
    typedef K                                        arg1_type;

    nullaryConstMemberFunctor(const F fun) :
        fun(fun)
    {
    }

    const result_type operator() (const K& obj) const
    {
        return ((obj).*(fun))();
    }

private:
    const F fun;
};

template<class K, typename F>
struct function_traits<nullaryConstMemberFunctor<K, F> > :
        function_traits<unaryFunctor<nullaryConstMemberFunctor<K, F> > >
{
};

template<class K, typename F>
struct unaryMemberFunctor : binaryFunctor<F>
{
    typename function_traits<F>::result_type typedef result_type;
    typedef  K                                       arg1_type;
    typename function_traits<F>::arg2_type   typedef arg2_type;

    unaryMemberFunctor(const F fun) :
        fun(fun)
    {
    }

    const result_type operator() (K& obj, const arg2_type arg) const
    {
        return ((obj).*(fun))(arg);
    }

private:
    const F fun;
};

template<class K, typename F>
struct function_traits<unaryMemberFunctor<K, F> > :
        function_traits<binaryFunctor<unaryMemberFunctor<K, F> > >
{
};

template<class K, typename F>
struct unaryConstMemberFunctor : binaryFunctor<F>
{
    typename function_traits<F>::result_type typedef result_type;
    typedef  K                                       arg1_type;
    typename function_traits<F>::arg2_type   typedef arg2_type;

    unaryConstMemberFunctor(const F fun) :
        fun(fun)
    {
    }

    const result_type operator() (const K& obj, const arg2_type arg) const
    {
        return ((obj).*(fun))(arg);
    }

private:
    const F fun;
};

template<class K, typename F>
struct function_traits<unaryConstMemberFunctor<K, F> > :
        function_traits<binaryFunctor<unaryConstMemberFunctor<K, F> > >
{
};

template<class K, typename F>
struct binaryMemberFunctor : ternaryFunctor<F>
{
    typename function_traits<F>::result_type typedef result_type;
    typedef  K                                       arg1_type;
    typename function_traits<F>::arg2_type   typedef arg2_type;
    typename function_traits<F>::arg3_type   typedef arg3_type;

    binaryMemberFunctor(const F fun) :
        fun(fun)
    {
    }

    const result_type operator() (K& obj,
                                  const arg2_type arg1,
                                  const arg3_type arg2) const
    {
        return ((obj).*(fun))(arg1, arg2);
    }

private:
    const F fun;
};

template<class K, typename F>
struct function_traits<binaryMemberFunctor<K, F> > :
        function_traits<ternaryFunctor<binaryMemberFunctor<K, F> > >
{
};

template<class K, typename F>
struct binaryConstMemberFunctor : ternaryFunctor<F>
{
    typename function_traits<F>::result_type typedef result_type;
    typedef  K                                       arg1_type;
    typename function_traits<F>::arg2_type   typedef arg2_type;
    typename function_traits<F>::arg3_type   typedef arg3_type;

    binaryConstMemberFunctor(const F fun) :
        fun(fun)
    {
    }

    const result_type operator() (K& obj,
                                  const arg2_type arg1,
                                  const arg3_type arg2) const
    {
        return ((obj).*(fun))(arg1, arg2);
    }

private:
    const F fun;
};

template<class K, typename F>
struct function_traits<binaryConstMemberFunctor<K, F> > :
        function_traits<ternaryFunctor<binaryConstMemberFunctor<K, F> > >
{
};

template<typename F>
struct unaryCurrier : nullaryFunctor<unaryCurrier<F> >
{
    typename function_traits<F>::result_type typedef result_type;
    typename function_traits<F>::arg1_type   typedef A;

    unaryCurrier(const F fun, const A arg) :
        fun(fun), arg(arg)
    {
    }

    const result_type operator() () const
    {
        return fun(arg);
    }

private:
    const F fun;
    const A arg;
};

template<typename F>
struct function_traits<unaryCurrier<F> > :
        function_traits<nullaryFunctor<unaryCurrier<F> > >
{
};

template<typename F>
struct binaryCurrier : unaryFunctor<binaryCurrier<F> >
{
    typename function_traits<F>::result_type typedef result_type;
    typename function_traits<F>::arg1_type   typedef A;
    typename function_traits<F>::arg2_type   typedef arg1_type;

    binaryCurrier(const F fun, const A arg1) :
        fun(fun), arg1(arg1)
    {
    }

    const result_type operator() (const arg1_type arg2) const
    {
        return fun(arg1, arg2);
    }

private:
    const F fun;
    const A arg1;
};

template<typename F>
struct function_traits<binaryCurrier<F> > :
        function_traits<unaryFunctor<binaryCurrier<F> > >
{
};

template<typename F>
struct ternaryCurrier : binaryFunctor<ternaryCurrier<F> >
{
    typename function_traits<F>::result_type typedef result_type;
    typename function_traits<F>::arg1_type   typedef A;
    typename function_traits<F>::arg2_type   typedef arg1_type;
    typename function_traits<F>::arg3_type   typedef arg2_type;

    ternaryCurrier(const F fun, const A arg1) :
        fun(fun), arg1(arg1)
    {
    }

    const result_type operator() (const arg1_type arg2,
                                  const arg2_type arg3) const
    {
        return fun(arg1, arg2, arg3);
    }

private:
    const F fun;
    const A arg1;
};

template<typename F>
struct function_traits<ternaryCurrier<F> > :
        function_traits<binaryFunctor<ternaryCurrier<F> > >
{
};

template<typename Lft, typename Rgt>
struct unaryComposer : unaryFunctor<unaryComposer<Lft, Rgt> >
{
    typename function_traits<Lft>::result_type typedef result_type;
    typename function_traits<Rgt>::arg1_type   typedef arg1_type;

    unaryComposer(const Lft lft, const Rgt rgt) :
        lft(lft), rgt(rgt)
    {
    }

    const result_type operator() (const arg1_type arg) const
    {
        return lft(rgt(arg));
    }

private:
    const Lft lft;
    const Rgt rgt;
};

template<typename Lft, typename Rgt>
struct function_traits<unaryComposer<Lft, Rgt> > :
        function_traits<unaryFunctor<unaryComposer<Lft, Rgt> > >
{
};

template<typename Lft, typename Rgt>
struct binaryComposer : binaryFunctor<binaryComposer<Lft, Rgt> >
{
    typename function_traits<Lft>::result_type typedef result_type;
    typename function_traits<Rgt>::arg1_type   typedef arg1_type;
    typename function_traits<Lft>::arg2_type   typedef arg2_type;

    binaryComposer(const Lft lft, const Rgt rgt) :
        lft(lft), rgt(rgt)
    {
    }

    const result_type operator() (const arg1_type arg1,
                                  const arg2_type arg2) const
    {
        return lft(rgt(arg1), arg2);
    }

private:
    const Lft lft;
    const Rgt rgt;
};

template<typename Lft, typename Rgt>
struct function_traits<binaryComposer<Lft, Rgt> > :
        function_traits<binaryFunctor<binaryComposer<Lft, Rgt> > >
{
};

template<typename Lft, typename Rgt>
struct ternaryComposer : ternaryFunctor<ternaryComposer<Lft, Rgt> >
{
    typename function_traits<Lft>::result_type typedef result_type;
    typename function_traits<Rgt>::arg1_type   typedef arg1_type;
    typename function_traits<Lft>::arg2_type   typedef arg2_type;
    typename function_traits<Lft>::arg3_type   typedef arg3_type;

    ternaryComposer(const Lft lft, const Rgt rgt) :
        lft(lft), rgt(rgt)
    {
    }

    const result_type operator() (const arg1_type arg1,
                                  const arg2_type arg2,
                                  const arg3_type arg3) const
    {
        return lft(rgt(arg1), arg2, arg3);
    }

private:
    const Lft lft;
    const Rgt rgt;
};

template<typename Lft, typename Rgt>
struct function_traits<ternaryComposer<Lft, Rgt> > :
        function_traits<ternaryFunctor<ternaryComposer<Lft, Rgt> > >
{
};

template<typename R>
struct function_traits<R(*)()> : nullaryFunctionTraits<R(*)()>
{
    static const std::size_t arity = 0;
};

template<typename R, typename A>
struct function_traits<R(*)(A)> : unaryFunctionTraits<R(*)(A)>
{
    typedef R result_type;
    typedef A arg1_type;
};

template<typename R, typename A, typename B>
struct function_traits<R(*)(A, B)> : binaryFunctionTraits<R(*)(A, B)>
{
    typedef R result_type;
    typedef A arg1_type;
    typedef B arg2_type;
};

template<typename R, typename A, typename B, typename C>
struct function_traits<R(*)(A, B, C)> : ternaryFunctionTraits<R(*)(A, B, C)>
{
    typedef R result_type;
    typedef A arg1_type;
    typedef B arg2_type;
    typedef C arg3_type;
};

template<class K, typename R>
struct function_traits<R(K::*)()> :
    unaryFunctionTraits<R(K::*)()>
{
    typedef struct nullaryMemberFunctor<K, R(K::*)()> wrapper_type;

    typedef R result_type;
    typedef K arg1_type;
};

template<class K, typename R>
struct function_traits<R (K::*)() const> :
    unaryFunctionTraits<R (K::*)() const>
{
    typedef struct nullaryConstMemberFunctor<K, R (K::*)() const> wrapper_type;

    typedef R result_type;
    typedef K arg1_type;
};

template<class K, typename R, typename A>
struct function_traits<R(K::*)(A)> : binaryFunctionTraits<R(K::*)(A)>
{
    typedef struct unaryMemberFunctor<K, R(K::*)(A)> wrapper_type;

    typedef R result_type;
    typedef K arg1_type;
    typedef A arg2_type;
};

template<class K, typename R, typename A>
struct function_traits<R (K::*)(A) const> :
    binaryFunctionTraits<R (K::*)(A) const>
{
    typedef struct unaryConstMemberFunctor<K, R (K::*)(A) const> wrapper_type;

    typedef R result_type;
    typedef K arg1_type;
    typedef A arg2_type;
};

template<class K, typename R, typename A, typename B>
struct function_traits<R(K::*)(A, B)> : ternaryFunctionTraits<R(K::*)(A, B)>
{
    typedef struct binaryMemberFunctor<K, R(K::*)(A, B)> wrapper_type;

    typedef R result_type;
    typedef K arg1_type;
    typedef A arg2_type;
    typedef B arg3_type;
};

template<class K, typename R, typename A, typename B>
struct function_traits<R (K::*)(A, B) const> :
    ternaryFunctionTraits<R (K::*)(A, B) const>
{
    typedef
    struct binaryConstMemberFunctor<K, R (K::*)(A, B) const> wrapper_type;

    typedef R result_type;
    typedef K arg1_type;
    typedef A arg2_type;
    typedef B arg3_type;
};

template<typename F>
struct currierTraits
{
    typename function_traits<F>::wrapper_type     typedef wrapF;
    typename function_traits<wrapF>::currier_type typedef result_type;
};

template<typename F>
inline typename currierTraits<F>::result_type
curry(
    const F fun,
    const typename function_traits<F>::arg1_type arg)
{
    return typename currierTraits<F>::result_type(
        function_traits<F>::wrap(fun), arg);
}

template<typename F>
inline
typename function_traits<
    typename currierTraits<F>::result_type
    >::currier_type
curry(
    const F fun,
    const typename function_traits<F>::arg1_type arg1,
    const typename function_traits<F>::arg2_type arg2)
{
    return curry(curry(fun, arg1), arg2);
}

template<typename F>
inline
typename function_traits<
    typename function_traits<
        typename currierTraits<F>::result_type
        >::currier_type
    >::currier_type
curry(
    const F fun,
    const typename function_traits<F>::arg1_type arg1,
    const typename function_traits<F>::arg2_type arg2,
    const typename function_traits<F>::arg3_type arg3)
{
    return curry(curry(fun, arg1, arg2), arg3);
}

template<typename Lft, typename Rgt>
struct composerTraits
{
    typename function_traits<Lft>::wrapper_type typedef wrapLft;
    typename function_traits<Rgt>::wrapper_type typedef wrapRgt;

    typename function_traits<wrapLft>::template composer<wrapRgt>::type
    typedef result_type;
};

template<typename Lft, typename Rgt>
inline typename composerTraits<Lft, Rgt>::result_type
compose(const Lft lft, const Rgt rgt)
{
    return typename composerTraits<Lft, Rgt>::result_type(
        function_traits<Lft>::wrap(lft),
        function_traits<Rgt>::wrap(rgt));
}

template<typename F>
inline typename function_traits<F>::wrapper_type method(const F fun)
{
    return function_traits<F>::wrap(fun);
}

template<typename T>
T identity(const T val)
{
    return val;
}

template<typename T>
inline typename function_traits<T(*)(T)>::currier_type constant(const T val)
{
    return curry(identity<T>, val);
}

}

#endif // !ODF_FUN_HPP
