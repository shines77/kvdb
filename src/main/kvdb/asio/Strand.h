
#ifndef KVDB_ASIO_STRAND_H
#define KVDB_ASIO_STRAND_H

#include "kvdb/asio/IoContext.h"
#include <type_traits>
#include <boost/asio/strand.hpp>

#if BOOST_VERSION >= 106600
#include <boost/asio/bind_executor.hpp>
#endif

namespace kvdb {
namespace Asio {

/**
    Hack to make it possible to forward declare strand (which is a inner class)
*/
class Strand : public IoContextBaseNamespace::IoContextBase::strand {
public:
    Strand(IoContext & ioContext) : IoContextBaseNamespace::IoContextBase::strand(ioContext) { }
};

#if BOOST_VERSION >= 106600
using boost::asio::bind_executor;
#else
template<typename T>
inline decltype(auto) bind_executor(Strand & strand, T && t)
{
    return strand.wrap(std::forward<T>(t));
}
#endif

} // namespace Asio
} // namespace kvdb

#endif // KVDB_ASIO_STRAND_H
