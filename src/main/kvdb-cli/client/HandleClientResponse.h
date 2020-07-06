
#ifndef KVDB_CLIENT_HANDLERESPONSE_H
#define KVDB_CLIENT_HANDLERESPONSE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "client/Common.h"
#include "client/KvdbClientApp.h"
#include "client/ClientContext.h"
#include "client/SendClientRequest.h"

#include <kvdb/basic/stdint.h>
#include <kvdb/basic/stddef.h>
#include <kvdb/basic/stdsize.h>

#include <kvdb/core/Opcode.h>
#include <kvdb/core/Messages.h>
#include <kvdb/stream/ByteBuffer.h>
#include <kvdb/stream/ConstBuffer.h>
#include <kvdb/stream/ParseStatus.h>
#include <kvdb/stream/ParseResult.h>

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

int stopKvdbClient(ClientContext & context);

template <typename OutputStreamTy>
static inline
int handleHandShakeResponse(ClientContext & context,
                            InputStream & is,
                            OutputStreamTy & os)
{
    std::cout << "KvdbClient::handleHandShakeResponse()" << std::endl;
    std::cout << std::endl;

    uint32_t iStatusCode = is.readUInt32();
    if (iStatusCode == 0) {
        int result = sendLoginRequest(context);
        return ParseStatus::Success;
    }
    
    return ParseStatus::Failed;
}

template <typename OutputStreamTy>
static inline
int handleLoginResponse(ClientContext & context,
                        InputStream & is,
                        OutputStreamTy & os)
{
    std::cout << "KvdbClient::handleLoginResponse()" << std::endl;
    std::cout << std::endl;

    uint32_t iStatusCode = is.readUInt32();
    if (iStatusCode == 0) {
        int result = sendLogoutRequest(context);
        return ParseStatus::Success;
    }
    return ParseStatus::Failed;
}

template <typename OutputStreamTy>
static inline
int handleLogoutResponse(ClientContext & context,
                         InputStream & is,
                         OutputStreamTy & os)
{
    std::cout << "KvdbClient::handleLogoutResponse()" << std::endl;
    std::cout << std::endl;

    uint32_t iStatusCode = is.readUInt32();
    if (iStatusCode == 0) {
        stopKvdbClient(context);

        return ParseStatus::Success;
    }

    return ParseStatus::Failed;
}

template <typename OutputStreamTy>
static inline
int handleQueryResponse(ClientContext & context,
                        InputStream & is,
                        OutputStreamTy & os)
{
    std::cout << "KvdbClient::handleQueryResponse()" << std::endl;
    std::cout << std::endl;

    uint32_t iStatusCode = is.readUInt32();
    if (iStatusCode == 0) {
        return ParseStatus::Success;
    }

    return ParseStatus::Failed;
}

template <typename OutputStreamTy>
static inline
int handleClientResponse(ClientContext & context, const IResponse & response, OutputStreamTy & os) {
    InputStream is(response.body(), response.bodySize());
    const MessageHeader & header = response.header;

    if (header.bodySize() > 0) {
        const char * first = is.current();
        int result;
        switch (header.opcode()) {
            case Opcode::HandShakeResponse:
                result = handleHandShakeResponse(context, is, os);
                break;

            case Opcode::LoginResponse:
                result = handleLoginResponse(context, is, os);
                break;

            case Opcode::LogoutResponse:
                result = handleLogoutResponse(context, is, os);
                break;

            case Opcode::QueryResponse:
                result = handleQueryResponse(context, is, os);
                break;

            default:
                // Unknown opcode message
                std::cout << "KvdbClient::handleClientResponse(): Error - Unknown opcode message" << std::endl;
                std::cout << std::endl;
                result = ParseStatus::Failed;
                break;
        }

        if (result < 0)
            return result;

        const char * last = is.current();
        if ((last - first) == (ptrdiff_t)header.bodySize()) {
            return ParseStatus::Success;
        }
        else {
            return ParseStatus::Error;
        }
    }
    else {
        return ParseStatus::TooSmall;
    }
}

} // namespace client
} // namespace kvdb

#endif // KVDB_CLIENT_HANDLERESPONSE_H
