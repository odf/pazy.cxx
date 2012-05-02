/* --------------------------------------------------------------------	*
 *	shared_array.hpp		 2012/01/29  by ODF             *
 * --------------------------------------------------------------------	*/

#ifndef _shared_array_h
#define _shared_array_h 1

#include <tr1/memory>

template<typename T>
class shared_array : public std::tr1::shared_ptr<T>
{
private:
  typedef std::tr1::shared_ptr<T> Ptr;

  struct array_deleter {
    void operator()(T* p) { delete [] p; }
  };
  
public:
  shared_array<T>(const size_t n) : Ptr(new T[n], array_deleter()) { }
};


#endif // !_shared_array_h
