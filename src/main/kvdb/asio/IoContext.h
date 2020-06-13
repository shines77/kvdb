
#ifndef KVDB_ASIO_IOCONTEXT_H
#define KVDB_ASIO_IOCONTEXT_H

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <boost/version.hpp>

#if BOOST_VERSION >= 106600
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#define IoContextBaseNamespace boost::asio
#define IoContextBase io_context
#else
#include <boost/asio/io_service.hpp>
#define IoContextBaseNamespace boost::asio
#define IoContextBase io_service
#endif // BOOST_VERSION >= 106600

using namespace boost::asio;

namespace kvdb {
namespace Asio {

class IoContext {
private:
    IoContextBaseNamespace::IoContextBase _impl;

public:
    IoContext() : _impl() { }
    explicit IoContext(int concurrency_hint) : _impl(concurrency_hint) { }

    operator IoContextBaseNamespace::IoContextBase & () {
        return _impl;
    }
    operator IoContextBaseNamespace::IoContextBase const & () const {
        return _impl;
    }

    void stop() {
        _impl.stop();
    }

    std::size_t run() {
        return _impl.run();
    }

#if BOOST_VERSION >= 106600
    boost::asio::io_context::executor_type get_executor() noexcept {
        return _impl.get_executor();
    }
#endif
};

template <typename T>
inline
#if BOOST_VERSION >= 106600
boost::asio::async_initiate<T, void()>
#else
auto
#endif
post(IoContextBaseNamespace::IoContextBase & ioContext, T && t)
{
#if BOOST_VERSION >= 106600
    return boost::asio::post(ioContext, std::forward<T>(t));
#else
    return ioContext.post(std::forward<T>(t));
#endif
}

template <typename T>
inline
#if BOOST_VERSION >= 106600
    IoContextBaseNamespace::IoContextBase
#else
    boost::asio::io_service
#endif
get_io_context(T && ioObject)
{
#if BOOST_VERSION >= 106600
    return ioObject.get_executor().context();
#else
    return ioObject.get_io_service();
#endif
}

} // namespace Asio
} // namespace kvdb

#endif // KVDB_ASIO_IOCONTEXT_H
