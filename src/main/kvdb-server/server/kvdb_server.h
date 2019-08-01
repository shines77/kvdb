
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
#include <boost/asio/signal_set.hpp>

#include "server/common.h"
#include "server/io_service_pool.h"
#include "server/kvdb_connection.h"
#include "server/connection_manager.h"

using namespace boost::asio;

namespace kvdb {
namespace server {

//
// See: http://www.boost.org/doc/libs/1_36_0/doc/html/boost_asio/example/echo/async_tcp_echo_server.cpp
//
class kvdb_server : public boost::enable_shared_from_this<kvdb_server>,
                    private boost::noncopyable
{
private:
    io_service_pool					    io_service_pool_;
    boost::asio::ip::tcp::acceptor	    acceptor_;
    connection_ptr	                    new_connection_;

    /// The connection manager which owns all live connections.
    connection_manager                  connection_manager_;

    std::shared_ptr<std::thread>	    thread_;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set             signals_;

    uint32_t                            buffer_size_;
    uint32_t					        packet_size_;

public:
    kvdb_server(const std::string & address, const std::string & port,
        uint32_t buffer_size = 32768,
        uint32_t packet_size = 64,
        uint32_t pool_size = std::thread::hardware_concurrency())
        : io_service_pool_(pool_size), acceptor_(io_service_pool_.get_first_io_service()),
          signals_(io_service_pool_.get_first_io_service()),
          buffer_size_(buffer_size), packet_size_(packet_size)
    {
        do_async_wait();

        start(address, port);
    }

    kvdb_server(short port, uint32_t buffer_size = 32768,
        uint32_t packet_size = 64,
        uint32_t pool_size = std::thread::hardware_concurrency())
        : io_service_pool_(pool_size), acceptor_(io_service_pool_.get_first_io_service(),
          ip::tcp::endpoint(ip::tcp::v4(), port)),
          signals_(io_service_pool_.get_first_io_service()),
          buffer_size_(buffer_size), packet_size_(packet_size)
    {
        do_async_wait();

        do_accept();
    }

    ~kvdb_server()
    {
        this->stop();
    }

    void do_async_wait() {
        //
        // Register to handle the signals that indicate when the server should exit.
        // It is safe to register for the same signal multiple times in a program,
        // provided all registration for the specified signal is made through Asio.
        //
        signals_.add(SIGINT);
        signals_.add(SIGTERM);
#if defined(SIGQUIT)
        signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

        signals_.async_wait(boost::bind(&kvdb_server::handle_stop, this));
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

        connection_manager_.stop_all();

        io_service_pool_.stop();
    }

    void run()
    {
        thread_ = std::make_shared<std::thread>([this]() {
            io_service_pool_.run();
        });
    }

    void join()
    {
        if (thread_->joinable()) {
            thread_->join();
        }
    }

private:
    void do_accept()
    {
        new_connection_.reset(new kvdb_connection(io_service_pool_.get_io_service(),
                                                  buffer_size_, packet_size_, g_test_mode));
        acceptor_.async_accept(new_connection_->socket(), boost::bind(&kvdb_server::handle_accept,
                               this, boost::asio::placeholders::error, new_connection_));
    }

    void handle_accept(const boost::system::error_code & ec,
                       connection_ptr new_connection)
    {
        //
        // Check whether the server was stopped by a signal before this completion
        // handler had a chance to run.
        //
        if (!acceptor_.is_open()) {
            return;
        }

        if (!ec) {
            if (new_connection) {
                connection_manager_.start(new_connection);
            }
            do_accept();
        }
        else {
            if (ec != boost::asio::error::operation_aborted) {
                // Accept error
                std::cout << "kvdb_server::handle_accept() - Error: (code = " << ec.value() << ") "
                          << ec.message().c_str() << std::endl;
                if (new_connection) {
                    connection_manager_.stop(new_connection);
                    new_connection.reset();
                }
            }
        }
    }

    void handle_stop()
    {
        stop();
    }
};

} // namespace server
} // namespace kvdb
