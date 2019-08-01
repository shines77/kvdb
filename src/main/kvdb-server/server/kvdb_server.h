
#pragma once

#include <memory>
#include <thread>
#include <functional>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

#include "server/common.h"
#include "server/io_service_pool.h"
#include "server/kvdb_connection.h"

using namespace boost::asio;

namespace kvdb {

//
// See: http://www.boost.org/doc/libs/1_36_0/doc/html/boost_asio/example/echo/async_tcp_echo_server.cpp
//
class kvdb_server : public boost::enable_shared_from_this<kvdb_server>,
                    private boost::noncopyable
{
private:
    io_service_pool					    io_service_pool_;
    boost::asio::ip::tcp::acceptor	    acceptor_;
    std::shared_ptr<kvdb_connection>	session_;
    std::shared_ptr<std::thread>	    thread_;
    uint32_t                            buffer_size_;
    uint32_t					        packet_size_;

public:
    kvdb_server(const std::string & address, const std::string & port,
        uint32_t buffer_size = 32768,
        uint32_t packet_size = 64,
        uint32_t pool_size = std::thread::hardware_concurrency())
        : io_service_pool_(pool_size), acceptor_(io_service_pool_.get_first_io_service()),
          buffer_size_(buffer_size), packet_size_(packet_size)
    {
        start(address, port);
    }

    kvdb_server(short port, uint32_t buffer_size = 32768,
        uint32_t packet_size = 64,
        uint32_t pool_size = std::thread::hardware_concurrency())
        : io_service_pool_(pool_size), acceptor_(io_service_pool_.get_first_io_service(),
          ip::tcp::endpoint(ip::tcp::v4(), port)),
          buffer_size_(buffer_size), packet_size_(packet_size)
    {
        do_accept();
    }

    ~kvdb_server()
    {
        this->stop();
    }

    void start(const std::string & address, const std::string & port)
    {
        ip::tcp::resolver resolver(io_service_pool_.get_now_io_service());
        ip::tcp::resolver::query query(address, port);
        ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

        boost::system::error_code ec;
        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            // Open endpoint error
            std::cout << "kvdb_server::start() - Error: (code = " << ec.value() << ") "
                      << ec.message().c_str() << std::endl;
            return;
        }

        boost::asio::socket_base::reuse_address option(true);
        acceptor_.set_option(option);
        acceptor_.bind(endpoint);
        acceptor_.listen();

        do_accept();
    }

    void stop()
    {
        if (acceptor_.is_open()) {
            acceptor_.cancel();
            acceptor_.close();
        }

        io_service_pool_.stop();
    }

    void run()
    {
        thread_ = std::make_shared<std::thread>([this] { io_service_pool_.run(); });
    }

    void join()
    {
        if (thread_->joinable())
            thread_->join();
    }

private:
    void handle_accept(const boost::system::error_code & ec, kvdb_connection * connection)
    {
        if (!ec) {
            if (connection) {
                connection->start();
            }
            do_accept();
        }
        else {
            if (ec != boost::asio::error::operation_aborted) {
                // Accept error
                std::cout << "kvdb_server::handle_accept() - Error: (code = " << ec.value() << ") "
                          << ec.message().c_str() << std::endl;
                if (connection) {
                    connection->stop();
                    delete connection;
                }
            }
        }
    }

    void do_accept()
    {
        kvdb_connection * new_connection = new kvdb_connection(io_service_pool_.get_io_service(),
                                                               buffer_size_, packet_size_, g_test_mode);
        acceptor_.async_accept(new_connection->socket(), boost::bind(&kvdb_server::handle_accept,
                               this, boost::asio::placeholders::error, new_connection));
    }
};

} // namespace KVCache
