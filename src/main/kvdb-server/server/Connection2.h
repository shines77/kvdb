
#ifndef KVDB_CONNECTION_H
#define KVDB_CONNECTION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "server/Response.h"
#include "server/Request.h"
#include "server/RequestHandler.h"
#include "server/RequestParser.h"

//
// Since boost.asio 1.66, it has changed from asio::io_service to asio::io_context.
//

namespace kvdb {
namespace server {

class ConnectionManager;

/// Represents a single connection from a client.
class Connection : public boost::enable_shared_from_this<Connection>,
                   private boost::noncopyable
{
public:
    /// Construct a connection with the given io_context.
    explicit Connection(boost::asio::io_service & io_service,
                        ConnectionManager & manager, RequestHandler & handler);

    ~Connection();

    /// Get the socket associated with the connection.
    boost::asio::ip::tcp::socket & socket();

    /// Start the first asynchronous operation for the connection.
    void start();

    /// Stop all asynchronous operations associated with the connection.
    void stop();

private:
    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code & ec,
                     std::size_t bytes_transferred);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code & ec);

    /// Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    /// The manager for this connection.
    ConnectionManager & connection_manager_;

    /// The handler used to process the incoming request.
    RequestHandler & request_handler_;

    /// Buffer for incoming data.
    boost::array<char, 8192> buffer_;

    /// The incoming request.
    Request request_;

    /// The parser for the incoming request.
    RequestParser request_parser_;

    /// The reply to be sent back to the client.
    Response response_;
};

//typedef boost::shared_ptr<connection> connection_ptr;

} // namespace server
} // namespace kvdb

#endif // KVDB_CONNECTION_H
