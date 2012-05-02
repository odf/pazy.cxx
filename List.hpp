#ifndef ODF_LIST_HPP
#define ODF_LIST_HPP

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

    List()
        : next_(),
          is_empty_(true),
          first_()
    {
    }

    List(T const& first, Ptr const& next)
        : next_(next),
          is_empty_(false),
          first_(first)
    {
    }
    
    explicit List(T const& first)
        : next_(),
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

    bool operator==(List const& other) const
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
inline List<T> makeList(T const& first)
{
    return List<T>(first);
}

template<typename T>
inline List<T> makeList(T const& first, List<T> const& rest)
{
    return List<T>(first, Thunk<List<T> >(rest));
}

template<typename T, typename Functor>
inline List<T> makeList(T const& first, Functor const code)
{
    return List<T>(first, makeThunk<List<T> >(code));
}


template<typename T>
std::ostream& operator<<(std::ostream& out, List<T> const& list)
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
                                    List<T> const,
                                    boost::forward_traversal_tag,
                                    T>
{
 public:
    ListIterator()
      : list_()
    {
    }

    explicit ListIterator(List<T> const& list)
      : list_(list)
    {
    }

 private:
    friend class boost::iterator_core_access;

    void increment()
    {
        list_ = list_.rest();
    }

    bool equal(ListIterator const& other) const
    {
        return this->list_ == other.list_;
    }

    const T dereference() const
    {
        return list_.first();
    }

    List<T> list_;
};

} // namespace odf

#endif // !ODF_LIST_HPP
