
#ifndef KVDB_ASIO_ASIOHACKSFWD_H
#define KVDB_ASIO_ASIOHACKSFWD_H

#include <boost/version.hpp>

/**
  Collection of forward declarations to improve compile time
 */
namespace boost {

namespace posix_time
{
    class ptime;
}

namespace asio
{
    template <typename Time>
    struct time_traits;

    namespace ip
    {
        class address;

        class tcp;

        template <typename InternetProtocol>
        class basic_endpoint;

        typedef basic_endpoint<tcp> tcp_endpoint;
    }

#if BOOST_VERSION >= 107000

    class executor;

    namespace ip
    {
        template <typename InternetProtocol, typename Executor>
        class basic_resolver;

        typedef basic_resolver<tcp, executor> tcp_resolver;
    }

#elif BOOST_VERSION >= 106600

    namespace ip
    {
        template <typename InternetProtocol>
        class basic_resolver;

        typedef basic_resolver<tcp> tcp_resolver;
    }

#else
    namespace ip
    {
        template <typename InternetProtocol>
        class resolver_service;

        template <typename InternetProtocol, typename ResolverService>
        class basic_resolver;

        typedef basic_resolver<tcp, resolver_service<tcp>> tcp_resolver;
    }

#endif // BOOST_VERSION
} // namespace boost

} // namespace boost

namespace kvdb {
namespace Asio {

class Strand;

} // namespace Asio
} // namespace kvdb

#endif // KVDB_ASIO_ASIOHACKSFWD_H
