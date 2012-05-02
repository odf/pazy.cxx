#ifndef ODF_LIST_FUN_HPP
#define ODF_LIST_FUN_HPP 1

#include <functional>
#include "List.hpp"
#include "fun.hpp"

namespace odf
{

template<typename T>
List<T> listFrom(const T start)
{
    return makeList(start, curry(listFrom<T>, start + 1));
}

template<typename Iter>
List<typename Iter::value_type> asList(Iter iter, const Iter end)
{
    List<typename Iter::value_type> result;

    while (iter != end)
    {
        result = makeList(*iter++, result);
    }

    return reverseList(result);
}

template<typename C>
inline List<typename C::value_type> asList(const C& collection)
{
    return asList(collection.begin(), collection.end());
}

template<typename T>
List<T> arraySlice(T const a[], const int from, const int to)
{
    if (from >= to)
    {
        return List<T>();
    }
    else
    {
        return makeList(a[from], curry(arraySlice<T>, a, from+1, to));
    }
}

template<typename T, std::size_t N>
inline List<T> asList(const T(&a)[N])
{
    return arraySlice<T>(a, 0, N);
}

template<typename L, typename F>
inline void forEach(const L& list, const F f)
{
    for (L p = list; !p.isEmpty(); p = p.rest())
    {
        f(p.first());
    }
}

template<typename L, typename F>
List<typename function_traits<F>::result_type>
mapList(const L src, const F fun)
{
    if (src.isEmpty())
    {
        return List<typename function_traits<F>::result_type>();
    }
    else
    {
        return makeList(fun(src.first()),
                        curry(compose(mapList<L, F>, &L::rest), src, fun));
   }
}

template<typename L, typename F>
L zipLists(const L lft, const L rgt, const F fun)
{
    if (lft.isEmpty() or rgt.isEmpty())
    {
        return L();
    }
    else
    {
        return makeList(fun(lft.first(), rgt.first()),
                        curry(compose(curry(compose(zipLists<L, F>, &L::rest),
                                            lft),
                                      &L::rest),
                              rgt, fun));
    }
}

template<typename L>
L operator+(const L& lft, const L& rgt)
{
    return zipLists(lft, rgt, std::plus<typename L::value_type>());
}

template<typename L>
L operator-(const L& lft, const L& rgt)
{
    return zipLists(lft, rgt, std::minus<typename L::value_type>());
}

template<typename L>
L operator*(const L& lft, const L& rgt)
{
    return zipLists(lft, rgt, std::multiplies<typename L::value_type>());
}

template<typename L>
L operator/(const L& lft, const L& rgt)
{
    return zipLists(lft, rgt, std::divides<typename L::value_type>());
}

template<typename L, typename F>
L filterList(const L src, const F pred)
{
    L p = src;
    while (not (p.isEmpty() or pred(p.first())))
    {
        p = p.rest();
    }

    if (p.isEmpty())
    {
        return p;
    }
    else
    {
        return makeList(p.first(),
                        curry(compose(filterList<L, F>, &L::rest), p, pred));
    }
}

template<typename L>
L takeList(const L list, const int n)
{
    if (list.isEmpty() or n <= 0)
    {
        return L();
    }
    else
    {
        return makeList(list.first(),
                        curry(compose(takeList<L>, &L::rest), list, n-1));
    }
}

template<typename L>
L dropList(const L list, const int n)
{
    L p = list;
    int i = n;
    while (not (p.isEmpty() or i <= 0))
    {
        --i;
        p = p.rest();
    }

    return p;
}

template<typename L>
typename L::value_type pickList(L const list, int const n)
{
    return dropList(list, n).first();
}

template<typename L>
L reverseList(const L& list)
{
    L result;

    for(L p = list; not p.isEmpty(); p = p.rest())
    {
        result = makeList(p.first(), result);
    }

    return result;
}

template<typename L, typename F>
typename L::value_type reduceList(const L& list,
                                  const typename L::value_type init,
                                  const F combine)
{
    typename L::value_type result = init;

    for (L p = list; !p.isEmpty(); p = p.rest())
    {
        result = combine(result, p.first());
    }

    return result;
}

template<typename L, typename F>
typename L::value_type reduceList(const L& list, const F combine)
{
    return reduceList(list.rest(), list.first(), combine);
}

template<typename L>
size_t lengthList(const L& list)
{
    size_t count = 0;

    for (L p = list; !p.isEmpty(); p = p.rest())
    {
        ++count;
    }

    return count;
}

template<typename L>
typename L::value_type sum(const L& list)
{
    return reduceList(list, std::plus<typename L::value_type>());
}

template<typename L>
typename L::value_type product(const L& list)
{
    return reduceList(list, std::multiplies<typename L::value_type>());
}

template<typename L, typename F>
L lazyConcat(const L a, const F b)
{
    if (a.isEmpty())
    {
        return b();
    }
    else
    {
        return makeList(a.first(),
                        curry(compose(lazyConcat<L, F>, &L::rest), a, b));
    }
}

template<typename L>
inline L concat(const L a, const L b)
{
    return lazyConcat(a, constant(b));
}

template<typename L>
typename L::value_type flatten(const L list)
{
    if (list.isEmpty())
    {
        return typename L::value_type();
    }
    else
    {
        return lazyConcat<typename L::value_type>(
            list.first(),
            curry(compose(flatten<L>, &L::rest), list));
    }
}

template<typename L, typename F>
inline typename function_traits<F>::result_type
flatMap(const L& list, const F fun)
{
    return flatten(mapList(list, fun));
}

}

#endif // !ODF_LIST_FUN_HPP
