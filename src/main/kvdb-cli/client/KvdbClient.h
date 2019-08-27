
#ifndef KVDB_CLIENT_H
#define KVDB_CLIENT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

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
#include "client/KvdbClientApp.h"

#include "kvdb/stream/InputStream.h"
#include "kvdb/stream/OutputStream.h"
#include "kvdb/stream/InputPacketStream.h"
#include "kvdb/stream/OutputPacketStream.h"
#include "kvdb/core/Messages.h"

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

    /// Buffer for incoming or outgoing data.
    std::vector<char>               request_;
    std::vector<char>               response_;
    std::size_t                     request_size_;
    std::size_t                     response_size_;

public:
    KvdbClient(boost::asio::io_service & io_service, const std::string & address, uint16_t port)
        : io_service_(io_service), socket_(io_service), signals_(io_service),
          address_(address), port_(port), request_size_(0), response_size_(0) {
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
        std::cout << "start(address, port)" << std::endl;
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

        connect();
    }

    void start()
    {
        start(address_, port_);
    }

    void stop()
    {
        std::cout << "stop()" << std::endl;
        this->disconnect();

        io_service_.stop();
        this->join();
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
        boost::asio::async_connect(socket_, endpoint_iterator_,
            boost::bind(&KvdbClient::handle_connect, this,
                        boost::asio::placeholders::error));
    }

    void disconnect()
    {
        std::cout << "disconnect()" << std::endl;
        socket_.close();
    }

    void run()
    {
        std::cout << "run()" << std::endl;
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
                        const boost::asio::ip::tcp::resolver::iterator & endpoint_iterator)
    {
        std::cout << "KvdbClient::handle_resolve()" << std::endl;
        std::cout << std::endl;

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
            KvdbClientConfig & config = KvdbClientApp::client_config;
            LoginRequest request;
            request.username = config.username;
            request.password = config.password;
            request.database = config.database;

            size_t msgLength = request.prepare();
            size_t requireSize = msgLength + kMsgHeaderSize;
            request_.resize(requireSize);
            request_size_ = requireSize;

            OutputPacketStream ostream(request_.data());
            ostream.writeHeader(kDefaultSignId, Message::Login, msgLength, 3);
            request.writeTo(ostream);

            std::cout << "KvdbClient::handle_connect()" << std::endl;
            std::cout << "request_.size() = " << request_.size() << std::endl;
            std::cout << std::endl;

            boost::asio::async_write(socket_, boost::asio::buffer(request_.data(), request_.size()),
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
        std::cout << "KvdbClient::handle_write_request()" << std::endl;
        std::cout << std::endl;

        if (!err) {
            char header_buf[kMsgHeaderSize];
            size_t readBytes = boost::asio::read(socket_, boost::asio::buffer(header_buf));
            if (readBytes == kMsgHeaderSize) {
                PacketHeader header;
                InputPacketStream istream(header_buf);
                istream.readHeader(header);
                if (header.signId == kDefaultSignId && header.msgLength > 0) {
                    //
                    // Receive the part data of response, if it's not completed, continue to read. 
                    //
                    response_.reserve(header.msgLength);
                    response_size_ = header.msgLength;
                    socket_.async_read_some(boost::asio::buffer(response_.data(), header.msgLength),
                        boost::bind(&KvdbClient::handle_read_some, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred,
                                    header.msgLength));
                }
            }
            else {
                // Read packet header error.
            }
        }
        else {
            std::cout << "KvdbClient::handle_write_request() Error: " << err.message() << "\n";
        }
    }

    void handle_read_some(const boost::system::error_code & err,
                          std::size_t bytes_transferred,
                          std::size_t bytes_wanted)
    {
        std::cout << "KvdbClient::handle_read_some()" << std::endl;
        std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
        std::cout << "bytes_wanted = " << bytes_wanted << std::endl;
        std::cout << std::endl;

        if (!err) {
            if (bytes_transferred < bytes_wanted) {
                std::cout << "KvdbClient::handle_read_some(): Receive next partment." << std::endl;
                //
                // Receive the data of next partment.
                //
                // response_buf_.consume(bytes_transferred);
                // boost::asio::buffer(response_.data(), bytes_wanted - bytes_transferred),
                socket_.async_read_some(boost::asio::buffer(&response_[bytes_transferred],
                    bytes_wanted - bytes_transferred),
                    boost::bind(&KvdbClient::handle_read_some, this,
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
                std::cout << "KvdbClient::handle_read_some(): Read all wanted bytes." << std::endl;
                //
                // The connection was successful, send the request.
                //
                // response_buf_.commit(request_size_);

                KvdbClientConfig & config = KvdbClientApp::client_config;
                LoginRequest request;
                request.username = config.username;
                request.password = config.password;
                request.database = config.database;

                size_t msgLength = request.prepare();
                size_t requireSize = msgLength + kMsgHeaderSize;
                request_.resize(requireSize);
                request_size_ = requireSize;

                OutputPacketStream ostream(request_.data());
                ostream.writeHeader(kDefaultSignId, Message::HandShake, msgLength, 3);
                request.writeTo(ostream);  

                std::cout << "KvdbClient::handle_read_some()" << std::endl;
                std::cout << "request_.size() = " << request_.size() << std::endl;
                std::cout << std::endl;

                boost::asio::async_write(socket_, boost::asio::buffer(request_.data(), request_.size()),
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

#endif // KVDB_CLIENT_H
