#ifndef ODF_NULLSTREAM
#define ODF_NULLSTREAM 1

#include <tr1/memory>
#include <iostream>

namespace odf
{

struct nullstream
{
};

template<typename T>
nullstream& operator<<(nullstream& ns, T)
{
    return ns;
}

inline nullstream& operator<<(nullstream& ns, std::ostream& (*)(std::ostream&))
{
    return ns;
}

}

#endif // !ODF_NULLSTREAM
