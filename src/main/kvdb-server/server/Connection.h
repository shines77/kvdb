
#ifndef KVDB_CONNECTION_H
#define KVDB_CONNECTION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <kvdb/core/Request.h>
#include <kvdb/core/Response.h>
#include <kvdb/stream/ByteBuffer.h>

#include "server/ConnectionContext.h"
#include "server/RequestHandler.h"

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
private:
    /// Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    ConnectionContext context_;

    /// The manager for this connection.
    ConnectionManager & connection_manager_;

    /// The handler used to process the incoming request.
    RequestHandler & request_handler_;

    /// The incoming request.
    IRequest request_;

    /// The reply to be sent back to the client.
    IResponse response_;

    /// Buffer for incoming or outgoing data.
    std::vector<char>               request_buf_;
    std::vector<char>               response_buf__;
    ByteBuffer                      response_buf_;
    std::size_t                     request_size_;
    std::size_t                     response_size_;

    /// Buffer for incoming data.
    boost::array<char, 8192> buffer_;

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

    /// Stop all asynchronous operations associated with the connection.
    void stop_connection();

    void shutdown_both();

private:
    void start_read_request();

    /// Handle completion of a read operation.
    void handle_read_some(const boost::system::error_code & ec,
                          std::size_t bytes_transferred,
                          std::size_t bytes_wanted);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code & ec,
                      std::size_t bytes_transferred);
};

typedef boost::shared_ptr<Connection> connection_ptr;

} // namespace server
} // namespace kvdb

#endif // KVDB_CONNECTION_H
