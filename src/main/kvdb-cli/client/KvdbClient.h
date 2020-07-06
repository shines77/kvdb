
#ifndef KVDB_CLIENT_H
#define KVDB_CLIENT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <iostream>
#include <streambuf>
#include <cstdint>
#include <cstddef>
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

#include "client/Common.h"
#include "client/KvdbClientApp.h"
#include "client/ClientContext.h"
#include "client/SendClientRequest.h"
#include "client/HandleClientResponse.h"

#include <kvdb/core/Messages.h>
#include <kvdb/stream/ByteBuffer.h>
#include <kvdb/stream/ConstBuffer.h>
#include <kvdb/stream/ParseStatus.h>

namespace kvdb {
namespace client {

//
// See: http://www.boost.org/doc/libs/1_36_0/doc/html/boost_asio/example/echo/async_tcp_echo_server.cpp
//
class KvdbClient : public boost::enable_shared_from_this<KvdbClient>,
                   private boost::noncopyable
{
protected:
    boost::asio::io_service &       io_service_;

    /// Buffer for incoming or outgoing data.
    ClientContext                   context_;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set         signals_;

    std::string                     local_;
    std::string                     address_;
    std::uint16_t                   port_;

    std::shared_ptr<std::thread>    thread_;

    asio_write_callback             func_handle_write_request_done_;
    asio_read_callback              func_handle_read_some_;

public:
    KvdbClient(boost::asio::io_service & io_service)
        : io_service_(io_service), context_(io_service), signals_(io_service),
          address_(""), port_(0) {
        do_signal_set();
    }
    KvdbClient(boost::asio::io_service & io_service, const std::string & address, uint16_t port)
        : io_service_(io_service), context_(io_service), signals_(io_service),
          address_(address), port_(port) {
        do_signal_set();
    }

    ~KvdbClient() {
        this->stop();
        this->wait();
    }

    ClientContext & context()             { return this->context_; }
    const ClientContext & context() const { return this->context_; }

    void init_callback() {
        this->context_.client = this;

        func_handle_write_request_done_ = boost::bind(&KvdbClient::handle_write_request_done,
                                                      this,
                                                      boost::asio::placeholders::error,
                                                      boost::asio::placeholders::bytes_transferred);

        func_handle_read_some_ = boost::bind(&KvdbClient::handle_read_some,
                                             this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred,
                                             _3);

        this->context_.write_callback = func_handle_write_request_done_;
        this->context_.read_callback  = func_handle_read_some_;
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

    void start(const std::string & endpoint)
    {
        const char * token = std::strstr(endpoint.c_str(), ":");
        if (token != nullptr) {
            std::string address, port;
            address.append(endpoint.c_str(), token - 1);
            port.append(token + 1, endpoint.c_str() + endpoint.size());

            std::uint16_t port_num = std::atoi(port.c_str());
            this->start(address, port_num);
        }
        else {
            std::cout << "KvdbClient::start(endpoint): Invalid endpoint string." << std::endl;
        }
    }

    void start(const std::string & address, std::uint16_t port_num)
    {
        std::cout << "KvdbClient::start(address, port)" << std::endl;

        if (port_num == 0) {
            std::cout << "KvdbClient::start(address, port): Invalid port number." << std::endl;
            return;
        }

        init_callback();

        try {
            //
            // Start an asynchronous resolve to translate the server and service names
            // into a list of endpoints.
            //
            boost::asio::ip::address remote_ip;
            remote_ip.from_string(address);

            boost::asio::ip::tcp::endpoint endpoint(remote_ip, port_num);
            context_.remote_endpoint = endpoint;

            std::string port = std::to_string(port_num);

            address_ = address;
            port_ = port_num;

            boost::asio::ip::tcp::resolver resolver(io_service_);
            boost::asio::ip::tcp::resolver::query query(address, port);
            context_.endpoint_iterator = resolver.resolve(query);

            // Form the request. We specify the "Connection: close" header so that the
            // server will close the socket after transmitting the response. This will
            // allow us to treat all data up until the EOF as the content.
            /*
            std::ostream request_stream(&request_buf_);
            request_stream << "GET " << port << " HTTP/1.1\r\n";
            request_stream << "Host: " << address << "\r\n";
            request_stream << "Accept: * / *\r\n";
            request_stream << "Connection: close\r\n\r\n";
            //*/
        }
        catch (const std::exception & ex) {
            std::cerr << "Exception: " << ex.what() << std::endl;
        }

        this->connect();
    }

    void start(const std::string & address, const std::string & port)
    {
        int n_port = std::atoi(port.c_str());
        if (n_port < 0 || n_port >= 65536) {
            std::cout << "KvdbClient::start(address, port): port number ["
                      << n_port << "] is not in range [0, 65536)."
                      << std::endl;
            return;
        }

        std::uint16_t port_num = static_cast<std::uint16_t>(n_port);
        this->start(address, port_num);
    }

    void start()
    {
        this->start(address_, port_);
    }

    void stop()
    {
        std::cout << "stop()" << std::endl;
        this->disconnect();

        io_service_.stop();
    }

    //
    // See: https://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/example/http/client/async_client.cpp
    //
    void connect()
    {
        std::cout << "connect()" << std::endl;
        //
        // Attempt a connection to each endpoint in the list until we
        // successfully establish a connection.
        //
        boost::asio::async_connect(context_.socket, context_.endpoint_iterator,
            boost::bind(&KvdbClient::handle_connect,
                        this,
                        boost::asio::placeholders::error));
    }

    void disconnect()
    {
        std::cout << "disconnect()" << std::endl;
        context_.socket.close();
    }

    void run()
    {
        std::cout << "run()" << std::endl;
        thread_ = std::make_shared<std::thread>([this]() {
            io_service_.run();
        });
    }

    void wait()
    {
        if (thread_.get() != nullptr) {
            if (thread_->joinable()) {
                thread_->join();
            }
            thread_.reset();
        }
    }

private:
    void handle_resolve(const boost::system::error_code & err,
                        const boost::asio::ip::tcp::resolver::iterator & endpoint_iterator)
    {
        std::cout << "KvdbClient::handle_resolve()" << std::endl;
        std::cout << std::endl;

        if (!err) {
            //
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            //
            boost::asio::async_connect(context_.socket, endpoint_iterator,
                boost::bind(&KvdbClient::handle_connect,
                            this,
                            boost::asio::placeholders::error));
        }
        else {
            std::cout << "KvdbClient::handle_resolve() Error: " << err.message() << "\n";
        }
    }

    void handle_connect(const boost::system::error_code & err)
    {
        std::cout << "KvdbClient::handle_connect()" << std::endl;
        std::cout << std::endl;

        if (!err) {
            //
            // The connection was successful. Send the request.
            //
            sendHankShakeRequest(context_);
        }
        else {
            std::cout << "KvdbClient::handle_connect() Error: " << err.message() << "\n";
        }
    }

    void handle_stop()
    {
        this->stop();
    }

    void handle_write_request_done(const boost::system::error_code & err, std::size_t bytes_transferred)
    {
        std::cout << "KvdbClient::handle_write_request_done()" << std::endl;
        std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
        std::cout << "error_code = " << err.value() << std::endl;
        std::cout << std::endl;

        if (!err) {
            start_read_response();
        }
        else {
            std::cout << "KvdbClient::handle_write_request_done() Error: " << err.message() << "\n";
        }
    }

    void start_read_response()
    {
        std::cout << "KvdbClient::start_read_response()" << std::endl;
        std::cout << std::endl;

        char header_buf[kMsgHeaderSize];
        size_t readBytes = boost::asio::read(context_.socket, boost::asio::buffer(header_buf));
        if (readBytes == kMsgHeaderSize) {
            MessageHeader header;
            header.readHeader(header_buf);
            uint32_t bodySize = header.bodySize();
            if (header.verifySign() && bodySize > 0) {
                //
                // Receive the part data of response, if it's not completed, continue to read. 
                //
                context_.response.header = header;
                context_.response_buf.reserve(bodySize);
                context_.response_size = bodySize;
                context_.socket.async_read_some(boost::asio::buffer(context_.response_buf.data(), bodySize),
                    boost::bind(&KvdbClient::handle_read_some,
                                this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred,
                                bodySize));
            }
            else {
                // The sign is dismatch
                std::cout << "KvdbClient::handle_write_request() Error: The sign is dismatch.\n" << std::endl;
            }
        }
        else {
            // Read message header error.
            std::cout << "KvdbClient::handle_write_request() Error: Read message header error.\n" << std::endl;
        }
    }

    void handle_read_some(const boost::system::error_code & err,
                          std::size_t bytes_transferred,
                          std::size_t bytes_wanted)
    {
        std::cout << "KvdbClient::handle_read_some()" << std::endl;
        std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
        std::cout << "bytes_wanted = " << bytes_wanted << std::endl;
        std::cout << "error_code = " << err.value() << std::endl;
        std::cout << std::endl;

        if (!err) {
            if (bytes_transferred < bytes_wanted) {
                std::cout << "KvdbClient::handle_read_some(): Receive next partment." << std::endl;
                //
                // Receive the data of next partment.
                //
                // response_buf_.consume(bytes_transferred);
                // boost::asio::buffer(response_buf_.data(), bytes_wanted - bytes_transferred),
                //
                context_.socket.async_read_some(boost::asio::buffer(&context_.response_buf[bytes_transferred],
                    bytes_wanted - bytes_transferred),
                    boost::bind(&KvdbClient::handle_read_some,
                                this,
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
                std::cout << "KvdbClient::handle_read_some(): Read all wanted bytes already." << std::endl;
                //
                // The connection was successful, send the request.
                //
                // response_buf_.commit(request_size_);
                //

                OutputStream os(context_.request_buf);
                context_.response.setBody(context_.response_buf.data());
                int result = handleClientResponse(context_, context_.response, os);
                if (result == ParseStatus::Success) {
                    start_read_response();
                }
                else if (result == ParseStatus::Failed) {
                    //
                }
                else if (result == ParseStatus::TooSmall) {
                    //
                }
                else {
                    // Unknown error
                }
            }
        }
        else {
            std::cout << "KvdbClient::handle_read_some() Error: " << err.message() << "\n";
        }
    }
};

} // namespace client
} // namespace kvdb

#endif // KVDB_CLIENT_H
