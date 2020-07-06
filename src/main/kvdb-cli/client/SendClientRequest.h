
#ifndef KVDB_CLIENT_SENDREQUEST_H
#define KVDB_CLIENT_SENDREQUEST_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "client/Common.h"
#include "client/KvdbClientApp.h"
#include "client/ClientContext.h"

#include <kvdb/basic/stdint.h>
#include <kvdb/basic/stddef.h>
#include <kvdb/basic/stdsize.h>

#include <kvdb/stream/ByteBuffer.h>
#include <kvdb/stream/ConstBuffer.h>
#include <kvdb/core/Messages.h>

#include <stdint.h>
#include <stddef.h>

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

using namespace boost::asio;

namespace kvdb {
namespace client {

int sendHankShakeRequest(ClientContext & context) {
    HandShakeRequest_v0 request;
    request.iVersion = 0x0101;

    OutputStream os(context.request_buf);
    uint32_t totalSize = request.writeTo(os);
    context.request_size = totalSize;

    std::cout << "KvdbClient::sendHankShakeRequest()" << std::endl;
    std::cout << std::endl;
    std::cout << "request.size() = " << totalSize << std::endl;
    std::cout << std::endl;

    boost::asio::async_write(context.socket, boost::asio::buffer(os.data(), os.size()),
                             context.write_callback);
    return (int)totalSize;
}

int sendLoginRequest(ClientContext & context) {
    KvdbClientConfig & config = KvdbClientApp::get_client_config();
    LoginRequest_v0 request;
    request.sUsername = config.username;
    request.sPassword = config.password;
    request.sDatabase = config.database;

    OutputStream os(context.request_buf);
    uint32_t totalSize = request.writeTo(os);
    context.request_size = totalSize;

    std::cout << "KvdbClient::sendLoginRequest()" << std::endl;
    std::cout << std::endl;
    std::cout << "request.size() = " << totalSize << std::endl;
    std::cout << std::endl;

    boost::asio::async_write(context.socket, boost::asio::buffer(os.data(), os.size()),
                             context.write_callback);
    return (int)totalSize;
}

int sendLogoutRequest(ClientContext & context) {
    LogoutRequest_v0 request;
    request.iVersion = 0x0101;

    OutputStream os(context.request_buf);
    uint32_t totalSize = request.writeTo(os);
    context.request_size = totalSize;

    std::cout << "KvdbClient::sendLogoutRequest()" << std::endl;
    std::cout << std::endl;
    std::cout << "request.size() = " << totalSize << std::endl;
    std::cout << std::endl;

    boost::asio::async_write(context.socket, boost::asio::buffer(os.data(), os.size()),
                             context.write_callback);
    return (int)totalSize;
}

} // namespace client
} // namespace kvdb

#endif // KVDB_CLIENT_SENDREQUEST_H
