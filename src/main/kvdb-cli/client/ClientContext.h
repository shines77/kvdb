
#ifndef KVDB_CLIENT_CONNECTION_CONTEXT_H
#define KVDB_CLIENT_CONNECTION_CONTEXT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <kvdb/core/Messages.h>
#include <kvdb/stream/ByteBuffer.h>
#include <kvdb/stream/ConstBuffer.h>
#include <kvdb/stream/ParseStatus.h>

using namespace boost::asio;

namespace kvdb {
namespace client {

typedef boost::function<void(const boost::system::error_code &, std::size_t)>               asio_write_callback;
typedef boost::function<void(const boost::system::error_code &, std::size_t, std::size_t)>  asio_read_callback;

class KvdbClient;

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class ClientContext : public boost::enable_shared_from_this<ClientContext>,
                      private boost::noncopyable
{
public:
    KvdbClient *                    client;
    /// Socket for the connection.
    boost::asio::ip::tcp::socket    socket;
    boost::asio::ip::tcp::endpoint  remote_endpoint;
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator;

    ByteBuffer                      request_buf;
    ByteBuffer                      response_buf;
    std::size_t                     request_size;
    std::size_t                     response_size;

    /// The incoming request.
    IRequest                        request;

    /// The reply to be sent back to the client.
    IResponse                       response;

    asio_write_callback             write_callback;
    asio_read_callback              read_callback;

public:
    /// Construct a connection context with the given io_context.
    ClientContext(boost::asio::io_service & io_service)
        : client(nullptr), socket(io_service),
          request_size(0), response_size(0) {
    }
};

} // namespace client
} // namespace kvdb

#endif // KVDB_CLIENT_CONNECTION_CONTEXT_H
