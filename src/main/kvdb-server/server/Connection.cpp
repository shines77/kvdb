
#include "server/Connection.h"

#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/logic/tribool.hpp>

#include "server/ConnectionManager.h"
#include "server/RequestParser.h"
#include "server/RequestHandler.h"

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
    socket_.async_read_some(boost::asio::buffer(buffer_),
        boost::bind(&Connection::handle_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Connection::stop()
{
    if (socket_.is_open()) {
        shutdown_both();

        socket_.close();
    }
}

void Connection::handle_read(const boost::system::error_code & err,
                             std::size_t bytes_transferred)
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

    std::cout << "Connection::handle_read()" << std::endl;
    std::cout << "error_code = " << err.value() << std::endl;
    std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
    std::cout << std::endl;

    if (!err) {
        int result = request_parser_.parse(context_, request_, buffer_.data(),
                                           buffer_.data() + bytes_transferred);
        if (result == ParseStatus::Success) {
            request_handler_.handle_request(request_, response_);
            boost::asio::async_write(socket_, response_.to_buffers(),
                boost::bind(&Connection::handle_write, shared_from_this(),
                            boost::asio::placeholders::error));
        }
        else if (result == ParseStatus::Failed) {
            response_ = Response::stock_response(Response::bad_request);
            boost::asio::async_write(socket_, response_.to_buffers(),
                boost::bind(&Connection::handle_write, shared_from_this(),
                            boost::asio::placeholders::error));
        }
        else if (result == ParseStatus::TooSmall) {
            socket_.async_read_some(boost::asio::buffer(buffer_),
                boost::bind(&Connection::handle_read, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        }
        else {
            //
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

        socket_.async_read_some(boost::asio::buffer(buffer_),
            boost::bind(&Connection::handle_read, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
    else if (err != boost::asio::error::operation_aborted) {
        connection_manager_.stop(shared_from_this());
    }
}

} // namespace server
} // namespace kvdb
