
#ifndef KVDB_ASIO_RESOLVER_H
#define KVDB_ASIO_RESOLVER_H

#include "kvdb/utils/Optional.h"
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <type_traits>

namespace kvdb {
namespace Net {

inline Optional<boost::asio::ip::tcp::endpoint> Resolve(boost::asio::ip::tcp::resolver& resolver, boost::asio::ip::tcp const & protocol,
    std::string const & host, std::string const & service)
{
    boost::system::error_code ec;
#if BOOST_VERSION >= 106600
    boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(protocol, host, service, ec);
    if (results.empty() || ec)
        return {};

    return results.begin()->endpoint();
#else
    boost::asio::ip::tcp::resolver::query query(std::move(protocol), std::move(host), std::move(service));
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query, ec);
    boost::asio::ip::tcp::resolver::iterator end;
    if (iter == end || ec)
        return {};

    return iter->endpoint();
#endif
}

} // namespace Net
} // namespace kvdb

#endif // KVDB_ASIO_RESOLVER_H
