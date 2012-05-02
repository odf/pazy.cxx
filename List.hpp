#ifndef ODF_LIST_HPP
#define ODF_LIST_HPP 1

#include <boost/iterator/iterator_facade.hpp>

#include "Thunk.hpp"

namespace odf
{

template<typename T>
class ListIterator;

template<typename T>
class List
{
private:
    typedef Thunk<List<T> > Ptr;

    Ptr next_;
    bool is_empty_;
    T first_;

public:
    typedef T value_type;

    List() :
        next_(),
        is_empty_(true),
        first_()
    {
    }

    List(const T& first, const Ptr& next) :
        next_(next),
        is_empty_(false),
        first_(first)
    {
    }
    
    explicit List(const T& first) :
        next_(),
        is_empty_(false),
        first_(first)
    {
    }

    T first() const
    {
        return first_;
    }
    
    bool isEmpty() const
    {
        return is_empty_;
    }

    List rest() const
    {
        if (next_.isEmpty())
        {
            return List();
        }
        else
        {
            return next_();
        }
    }

    bool operator==(const List& other) const
    {
        return first_ == other.first_ and next_ == other.next_;
    }

    ListIterator<T> begin() const
    {
        return ListIterator<T>(*this);
    }

    ListIterator<T> end() const
    {
        return ListIterator<T>();
    }
};

template<typename T>
inline List<T> makeList(const T& first)
{
    return List<T>(first);
}

template<typename T>
inline List<T> makeList(const T& first, const List<T>& rest)
{
    return List<T>(first, Thunk<List<T> >(rest));
}

template<typename T, typename Functor>
inline List<T> makeList(const T& first, const Functor code)
{
    return List<T>(first, makeThunk<List<T> >(code));
}


template<typename T>
std::ostream& operator<<(std::ostream& out, const List<T>& list)
{
    if (!list.isEmpty())
    {
        out << list.first();
        for (List<T> p = list.rest(); !p.isEmpty(); p = p.rest())
        {
            out << " " << p.first();
        }
    }
    return out;
}

template<typename T>
class ListIterator
    : public boost::iterator_facade<ListIterator<T>,
                                    const List<T>,
                                    boost::forward_traversal_tag,
                                    T>
{
 public:
    ListIterator()
      : list_()
    {
    }

    explicit ListIterator(const List<T>& list)
      : list_(list)
    {
    }

 private:
    friend class boost::iterator_core_access;

    void increment()
    {
        list_ = list_.rest();
    }

    bool equal(const ListIterator& other) const
    {
        return this->list_ == other.list_;
    }

    const T dereference() const
    {
        return list_.first();
    }

    List<T> list_;
};

}

#endif // !ODF_LIST_HPP
