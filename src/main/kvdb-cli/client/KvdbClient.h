
#pragma once

#include <iostream>
#include <streambuf>
#include <string>
#include <iostream>
#include <memory>
#include <thread>
#include <functional>

#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/signal_set.hpp>

#include "client/common.h"

using namespace boost::asio;

namespace kvdb {
namespace client {

//
// See: http://www.boost.org/doc/libs/1_36_0/doc/html/boost_asio/example/echo/async_tcp_echo_server.cpp
//
class KvdbClient : public boost::enable_shared_from_this<KvdbClient>,
                   private boost::noncopyable
{
private:
    boost::asio::io_service &       io_service_;
    boost::asio::ip::tcp::socket    socket_;
    boost::asio::ip::tcp::endpoint  remote_endpoint_;
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator_;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set         signals_;

    std::string                     local_;
    std::string                     address_;
    uint16_t                        port_;

    std::shared_ptr<std::thread>    thread_;

    boost::asio::streambuf          request_;
    boost::asio::streambuf          response_;

    static const std::size_t kRevBufSize = 8192;

    /// Buffer for incoming data.
    boost::array<char, kRevBufSize> rev_buffer_;
    std::size_t                     rev_bufsize_;

public:
    KvdbClient(boost::asio::io_service & io_service, const std::string & address, uint16_t port)
        : io_service_(io_service), socket_(io_service), signals_(io_service),
          address_(address), port_(port), rev_bufsize_(kRevBufSize) {
        do_signal_set();
    }

    ~KvdbClient() {
        this->stop();
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

        signals_.async_wait(boost::bind(&KvdbClient::handle_stop, this));
    }

    void start(const std::string & address, uint16_t port)
    {
        //
        // Start an asynchronous resolve to translate the server and service names
        // into a list of endpoints.
        //
        boost::asio::ip::address local;
        local.from_string(address);

        boost::asio::ip::tcp::endpoint endpoint(local, port);
        remote_endpoint_ = endpoint;

        std::string s_port;
        s_port = std::to_string(port);

        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(address, s_port);
        endpoint_iterator_ = resolver.resolve(query);

        connect();
    }

    void start()
    {
        start(address_, port_);
    }

    void stop()
    {
        this->disconnect();

        io_service_.stop();
    }

    //
    // See: https://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/example/http/client/async_client.cpp
    //
    void connect()
    {
        //
        // Attempt a connection to each endpoint in the list until we
        // successfully establish a connection.
        //
        boost::asio::async_connect(socket_, endpoint_iterator_,
            boost::bind(&KvdbClient::handle_connect, this,
                        boost::asio::placeholders::error));
    }

    void disconnect()
    {
        socket_.close();
    }

    void run()
    {
        thread_ = std::make_shared<std::thread>([this]() {
            io_service_.run();
        });
    }

    void join()
    {
        if (thread_->joinable()) {
            thread_->join();
        }
    }

private:
    void handle_resolve(const boost::system::error_code & err,
                        const ip::tcp::resolver::iterator & endpoint_iterator)
    {
        /*
        if (!err) {
            //
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            //
            boost::asio::async_connect(socket_, endpoint_iterator,
                boost::bind(&KvdbClient::handle_connect, this,
                            boost::asio::placeholders::error));
        }
        else {
            std::cout << "Error: " << err.message() << "\n";
        }
        //*/
    }

    void handle_connect(const boost::system::error_code & err)
    {
        if (!err) {
            //
            // The connection was successful. Send the request.
            //
            boost::asio::async_write(socket_, request_,
                boost::bind(&KvdbClient::handle_write_request, this,
                            boost::asio::placeholders::error));
        }
        else {
            std::cout << "KvdbClient::handle_connect() Error: " << err.message() << "\n";
        }
    }

    void handle_stop()
    {
        stop();
    }

    void handle_write_request(const boost::system::error_code & err)
    {
        if (!err) {
            //
            // Receive the part data of response, if it's not completed, continue to read. 
            //
            socket_.async_read_some(boost::asio::buffer(rev_buffer_),
                boost::bind(&KvdbClient::handle_read_some, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred,
                            rev_bufsize_));
        }
        else {
            std::cout << "KvdbClient::handle_write_request() Error: " << err.message() << "\n";
        }
    }

    void handle_read_some(const boost::system::error_code & err,
                          std::size_t bytes_transferred,
                          std::size_t bytes_wanted)
    {
        if (!err) {
            if (bytes_transferred < bytes_wanted) {
                //
                // Receive the data of next partment.
                //
                socket_.async_read_some(boost::asio::buffer(&rev_buffer_[bytes_transferred], bytes_wanted - bytes_transferred),
                    boost::bind(&KvdbClient::handle_read_some, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred,
                        bytes_wanted - bytes_transferred)
                );
            }
            else if (bytes_transferred <= 0) {
                //
                // Error: no data read.
                //
                this->stop();
            }
            else {
                //
                // The connection was successful, send the request.
                //
                boost::asio::async_write(socket_, request_,
                    boost::bind(&KvdbClient::handle_write_request, this,
                                boost::asio::placeholders::error));
            }
        }
        else {
            std::cout << "KvdbClient::handle_read_some() Error: " << err.message() << "\n";
        }
    }
};

} // namespace client
} // namespace kvdb