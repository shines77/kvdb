
#include "server/Connection.h"

#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/logic/tribool.hpp>

#include "server/ConnectionManager.h"
#include "server/RequestHandler.h"

#include "kvdb/core/Request.h"
#include "kvdb/core/Response.h"
#include "kvdb/core/Messages.h"
#include "kvdb/stream/ParseStatus.h"
#include "kvdb/stream/InputStream.h"
#include "kvdb/stream/OutputStream.h"
#include "kvdb/stream/InputPacketStream.h"
#include "kvdb/stream/OutputPacketStream.h"

namespace kvdb {
namespace server {

Connection::Connection(boost::asio::io_service & io_service,
                       ConnectionManager & manager, RequestHandler & handler)
    : socket_(io_service),
      context_(io_service),
      connection_manager_(manager),
      request_handler_(handler)
{
}

Connection::~Connection(void)
{
    //
}

boost::asio::ip::tcp::socket & Connection::socket()
{
    return socket_;
}

void Connection::shutdown_both()
{
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
}

void Connection::start()
{
    start_read_request();
}

void Connection::stop()
{
    if (socket_.is_open()) {
        shutdown_both();

        socket_.close();
    }
}

void Connection::start_read_request()
{
    char header_buf[kMsgHeaderSize];
    size_t readBytes = boost::asio::read(socket_, boost::asio::buffer(header_buf));
    if (readBytes == kMsgHeaderSize) {
        MessageHeader header;
        InputPacketStream istream(header_buf);
        istream.readHeader(header);
        uint32_t requestSize = header.length;
        if (header.sign == kDefaultSignId && requestSize > 0) {
            //
            // Receive the part data of response, if it's not completed, continue to read. 
            //
            request_.header = header;
            request_buf_.reserve(requestSize);
            request_size_ = requestSize;
            socket_.async_read_some(boost::asio::buffer(request_buf_.data(), requestSize),
                boost::bind(&Connection::handle_read_some, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred,
                            requestSize));
        }
        else {
            // The signId is dismatch
            std::cout << "Connection::start_read_request() Error: The signId is dismatch.\n" << std::endl;
        }
    }
    else {
        // Read packet header error.
        std::cout << "Connection::start_read_request() Error: Read packet header error.\n" << std::endl;
    }
}

void Connection::handle_read_some(const boost::system::error_code & err,
                                  std::size_t bytes_transferred,
                                  std::size_t bytes_wanted)
{
    //
    // See: https://www.cnblogs.com/pengyusong/p/6433516.html
    //
    // boost::asio::error::operation_aborted = 955,
    // boost::asio::error::connection_was_aborted = 10053,
    //   A established connection was aborted by the software in your host machine.
    // boost::asio::error::connection_has_closed = 10054,
    //   Connection closed by peer.
    //

    std::cout << "Connection::handle_read_some()" << std::endl;
    std::cout << "error_code = " << err.value() << std::endl;
    std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
    std::cout << "bytes_wanted = " << bytes_wanted << std::endl;
    std::cout << std::endl;

    if (!err) {
        if (bytes_transferred < bytes_wanted) {
            std::cout << "KvdbClient::handle_read_some(): Receive next partment." << std::endl;
            //
            // Receive the data of next partment.
            //
            // request_buf_.consume(bytes_transferred);
            // boost::asio::buffer(request_buf_.data(), bytes_wanted - bytes_transferred),
            //
            socket_.async_read_some(boost::asio::buffer(&request_buf_[bytes_transferred],
                bytes_wanted - bytes_transferred),
                boost::bind(&Connection::handle_read_some, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred,
                    bytes_wanted - bytes_transferred)
            );
        }
        else if (bytes_transferred == 0) {
            //
            // Error: no data read.
            //
            this->stop();
        }
        else {
            request_.setBody(&request_buf_[0]);
            int result = request_handler_.handleRequest(context_, request_);
            if (result == ParseStatus::Success) {
                //
            }
            else if (result == ParseStatus::Failed) {
                //
            }
            else if (result == ParseStatus::TooSmall) {
                //
            }
            else {
                //
            }
        }
    }
    else if (err != boost::asio::error::operation_aborted) {
        connection_manager_.stop(shared_from_this());
    }
}

void Connection::handle_write(const boost::system::error_code & err)
{
    std::cout << "Connection::handle_write()" << std::endl;
    std::cout << "error_code = " << err.value() << std::endl;
    std::cout << std::endl;

    if (!err) {
        // Initiate graceful connection closure.
        //boost::system::error_code ignored_ec;
        //socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

        start_read_request();
    }
    else if (err != boost::asio::error::operation_aborted) {
        connection_manager_.stop(shared_from_this());
    }
}

} // namespace server
} // namespace kvdb
