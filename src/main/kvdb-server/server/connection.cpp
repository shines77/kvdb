
#include "server/connection.h"

#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/logic/tribool.hpp>

#include "server/connection_manager.h"
#include "server/request_handler.h"

namespace kvdb {
namespace server {

connection::connection(boost::asio::io_service & io_service,
                       connection_manager & manager, request_handler & handler)
    : socket_(io_service),
      connection_manager_(manager),
      request_handler_(handler)
{
}

connection::~connection(void)
{
    //
}

boost::asio::ip::tcp::socket & connection::socket()
{
    return socket_;
}

void connection::start()
{
    socket_.async_read_some(boost::asio::buffer(buffer_),
        boost::bind(&connection::handle_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void connection::stop()
{
    socket_.close();
}

void connection::handle_read(const boost::system::error_code & ec,
                             std::size_t bytes_transferred)
{
    if (!ec)
    {
        boost::tribool result;
        boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
            request_, buffer_.data(), buffer_.data() + bytes_transferred);

        if (result)
        {
            request_handler_.handle_request(request_, response_);
            boost::asio::async_write(socket_, response_.to_buffers(),
                boost::bind(&connection::handle_write, shared_from_this(),
                            boost::asio::placeholders::error));
        }
        else if (!result)
        {
            response_ = response::stock_response(response::bad_request);
            boost::asio::async_write(socket_, response_.to_buffers(),
                boost::bind(&connection::handle_write, shared_from_this(),
                            boost::asio::placeholders::error));
        }
        else
        {
            socket_.async_read_some(boost::asio::buffer(buffer_),
                boost::bind(&connection::handle_read, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        }
    }
    else if (ec != boost::asio::error::operation_aborted)
    {
        connection_manager_.stop(shared_from_this());
    }
}

void connection::handle_write(const boost::system::error_code & ec)
{
    if (!ec)
    {
        // Initiate graceful connection closure.
        boost::system::error_code ignored_ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }

    if (ec != boost::asio::error::operation_aborted)
    {
        connection_manager_.stop(shared_from_this());
    }
}

} // namespace server
} // namespace kvdb
