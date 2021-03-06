
#ifndef KVDB_SERVER_H
#define KVDB_SERVER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <iostream>
#include <streambuf>
#include <string>
#include <memory>
#include <thread>
#include <stdexcept>
#include <functional>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/signal_set.hpp>

#include "server/Common.h"
#include "server/IoServicePool.h"
#include "server/KvdbConnection.h"
#include "server/ConnectionManager.h"

using namespace boost::asio::ip;

#define USE_KVDB_CONNECTION     0

namespace kvdb {
namespace server {

//
// See: http://www.boost.org/doc/libs/1_36_0/doc/html/boost_asio/example/echo/async_tcp_echo_server.cpp
//
class KvdbServer : public boost::enable_shared_from_this<KvdbServer>,
                   private boost::noncopyable
{
private:
    IoServicePool                   io_service_pool_;
    tcp::acceptor                   acceptor_;
    connection_ptr                  new_connection_;

    std::string                     remote_address_;
    std::string                     remote_port_;

    /// The connection manager which owns all live connections.
    ConnectionManager               connection_manager_;

    ///
    RequestHandler                  request_handler_;

    std::shared_ptr<std::thread>    thread_;
    std::atomic<bool>               startting_;
    std::atomic<bool>               running_;
    std::atomic<bool>               stopping_;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set         signals_;

    uint32_t                        buffer_size_;
    uint32_t                        packet_size_;

public:
    KvdbServer(const std::string & address, const std::string & port,
               const std::string & docRoot,
               uint32_t pool_size = std::thread::hardware_concurrency())
        : io_service_pool_(pool_size), acceptor_(io_service_pool_.get_first_io_service()),
          remote_address_(address), remote_port_(port),
          request_handler_(docRoot),
          startting_(false), running_(false), stopping_(false),
          signals_(io_service_pool_.get_first_io_service())
    {
        do_signal_set();
    }

    KvdbServer(const std::string & address, uint16_t port,
               const std::string & docRoot,
               uint32_t pool_size = std::thread::hardware_concurrency())
        : io_service_pool_(pool_size), acceptor_(io_service_pool_.get_first_io_service()),
          remote_address_(address), remote_port_(std::to_string(port)),
          request_handler_(docRoot),
          startting_(false), running_(false), stopping_(false),
          signals_(io_service_pool_.get_first_io_service())
    {
        do_signal_set();
    }

    ~KvdbServer()
    {
        this->stop();
        this->wait();
    }

    void do_signal_set() {
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

        signals_.async_wait(boost::bind(&KvdbServer::handle_stop, this));
    }

    bool start()
    {
        return start(remote_address_, remote_port_);
    }

    bool start(const std::string & address, const std::string & port)
    {
        if (this->startting_ || this->running_)
            return false;

        this->startting_ = true;

        tcp::resolver resolver(io_service_pool_.get_first_io_service());
        tcp::resolver::query query(address, port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::endpoint endpoint = *endpoint_iterator;

        boost::system::error_code err;
        acceptor_.open(endpoint.protocol(), err); 
        if (err) {
            // Open endpoint error
            std::stringbuf err_info;
            std::ostream os(&err_info);
            os << "kvdb_server::start() - Error: (code = " << err.value() << ") "
               << err.message().c_str();
            std::string str_info = err_info.str();
            std::cout << str_info.c_str() << std::endl;
            std::logic_error logic_err(str_info.c_str());
            throw std::exception(logic_err);
            return false;
        }

        boost::asio::socket_base::reuse_address option(true);
        acceptor_.set_option(option);
        acceptor_.bind(endpoint);
        acceptor_.listen();

        do_accept();

        this->run();

        return true;
    }

    void stop()
    {
        if (this->running_ && !this->stopping_) {
            this->stopping_ = true;

            if (this->acceptor_.is_open()) {
                this->acceptor_.cancel();
                this->acceptor_.close();
            }

            this->connection_manager_.stop_all();

            this->io_service_pool_.stop();
        }
    }

    void wait()
    {
        if (this->startting_ || this->running_) {
            if (this->thread_.get() != nullptr) {
                if (this->thread_->joinable()) {
                    this->thread_->join();
                }
                this->thread_.reset();
            }

            this->stopping_ = false;
            this->running_ = false;
        }
    }

private:
    void run()
    {
        if (this->startting_ && !this->running_) {
            this->thread_ = std::make_shared<std::thread>([this]() {
                io_service_pool_.start();

                this->startting_ = false;
                this->running_ = true;
                this->stopping_ = false;

                io_service_pool_.wait();
            });
        }
    }

    void do_accept()
    {
        if (this->acceptor_.is_open()) {
#if USE_KVDB_CONNECTION
            new_connection_.reset(new KvdbConnection(io_service_pool_.get_io_service(),
                                                     buffer_size_, packet_size_, g_need_echo));
#else
            new_connection_.reset(new Connection(io_service_pool_.get_io_service(),
                                                 connection_manager_, request_handler_));
#endif
            acceptor_.async_accept(new_connection_->socket(), boost::bind(&KvdbServer::handle_accept,
                                   this, boost::asio::placeholders::error, new_connection_));
        }
    }

    void handle_accept(const boost::system::error_code & err,
                       connection_ptr new_connection)
    {
        //
        // Check whether the server was stopped by a signal before this completion
        // handler had a chance to run.
        //
        if (!this->acceptor_.is_open()) {
            return;
        }

        if (!err) {
            if (new_connection) {
                connection_manager_.start(new_connection);
            }

            do_accept();
        }
        else if (err != boost::asio::error::operation_aborted) {
            // Operation aborted
            std::cout << "kvdb_server::handle_accept() - Error: operation_aborted (code = "
                      << err.value() << ") "
                      << err.message().c_str() << std::endl;
        }
        else {
            // Accept error
            std::cout << "kvdb_server::handle_accept() - Error: (code = " << err.value() << ") "
                        << err.message().c_str() << std::endl;
            if (new_connection) {
                this->connection_manager_.stop(new_connection);
                new_connection.reset();
            }
        }
    }

    void handle_stop()
    {
        this->stop();
    }
};

} // namespace server
} // namespace kvdb

#endif // KVDB_SERVER_H
