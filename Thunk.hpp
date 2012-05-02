#ifndef ODF_THUNK_HPP
#define ODF_THUNK_HPP 1

#include <tr1/memory>
#include <iostream>

namespace odf
{

#ifdef DEBUG
std::ostream& log = std::cout;
#else
#include "nullstream.hpp"

odf::nullstream log;
#endif


template<typename T>
class AbstractThunkImpl
{
public:
    virtual T operator() () const = 0;
    virtual ~AbstractThunkImpl() {};
};

template<typename T, typename Functor>
class ThunkImpl : public AbstractThunkImpl<T>
{
private:
    typename std::tr1::shared_ptr<Functor> typedef FunPtr;

    mutable FunPtr code_;
    mutable bool pending_;
    mutable T value_;

public:
    ThunkImpl() :
        code_(),
        pending_(false)
    {
        log << "--Making empty ThunkImpl   " << this << std::endl;
    }

    explicit ThunkImpl(Functor const& code)
        : code_(FunPtr(new Functor(code))),
          pending_(true)
    {
        log << "--Making delayed ThunkImpl " << this << std::endl;
    }

    explicit ThunkImpl(T const& value)
        : code_(),
          pending_(false),
          value_(value)
    {
        log << "--Making direct ThunkImpl  " << this << std::endl;
    }

    ThunkImpl(ThunkImpl const& other)
        : code_(other.code_),
          pending_(other.pending_),
          value_(other.value_)
    {
        log << "----Copying ThunkImpl      " << other << "  => "
            << this << std::endl;
    }

    ~ThunkImpl()
    {
        log << "----Destroying ThunkImpl   " << this << std::endl;
    }

    T operator() () const
    {
        if (pending_)
        {
            log << "----Forcing ThunkImpl      " << this << std::endl;
            value_ = (*code_)();
            code_ = FunPtr();
            pending_ = false;
        }
        return value_;
    }
};

template<typename T>
class Thunk
{
    typename std::tr1::shared_ptr<AbstractThunkImpl<T> const> typedef ThunkPtr;
    typedef T(*FunPtr)();

public:
    Thunk() :
        content_()
    {
    }

    explicit Thunk(FunPtr const& code)
        : content_(ThunkPtr(new ThunkImpl<T, FunPtr>(code)))
    {
    }

    explicit Thunk(T const& value)
        : content_(ThunkPtr(new ThunkImpl<T, FunPtr>(value)))
    {
    }

    const T operator() () const
    {
        return content_->operator()();
    }

    bool isEmpty() const
    {
        return content_.get() == 0;
    }

    bool operator==(Thunk const& other) const
    {
        return content_.get() == other.content_.get();
    }

private:
    ThunkPtr content_;

    Thunk(AbstractThunkImpl<T> const* code) :
        content_(ThunkPtr(code))
    {
    }

    template<typename S, typename Functor>
    friend Thunk<S> makeThunk(Functor const& code);
};

template<typename T, typename Functor>
Thunk<T> makeThunk(Functor const& code)
{
    return Thunk<T>(new ThunkImpl<T, Functor>(code));
}

}

#endif // !ODF_THUNK_HPP
