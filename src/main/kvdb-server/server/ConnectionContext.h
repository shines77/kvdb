
#ifndef KVDB_CONNECTION_CONTEXT_H
#define KVDB_CONNECTION_CONTEXT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace kvdb {
namespace server {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class ConnectionContext : private boost::noncopyable
{
private:
    /// Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    std::string username_;
    std::string password_;
    std::string database_;

public:
    /// Construct a connection context with the given io_context.
    ConnectionContext(boost::asio::io_service & io_service) : socket_(io_service) {
        //
    }

    /// Get the socket associated with the connection.
    boost::asio::ip::tcp::socket & socket() { return socket_; }
};

} // namespace server
} // namespace kvdb

#endif // KVDB_CONNECTION_CONTEXT_H
