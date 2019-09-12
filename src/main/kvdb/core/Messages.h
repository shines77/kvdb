
#ifndef KVDB_CORE_MESSAGES_H
#define KVDB_CORE_MESSAGES_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>

#include "kvdb/core/Request.h"
#include "kvdb/core/Response.h"
#include "kvdb/core/StatusCode.h"
#include "kvdb/stream/InputPacketStream.h"
#include "kvdb/stream/OutputPacketStream.h"

namespace kvdb {

struct LoginRequest : public Request<LoginRequest> {
    static const uint32_t kArgs = 3;
    std::string username;
    std::string password;
    std::string database;

    LoginRequest(const char * data = nullptr)
        : Request(Message::LogoutRequest, data) {
        setArgs(kArgs);
    }
    LoginRequest(std::string _username, std::string _password, std::string _database)
        : Request(Message::LogoutRequest, nullptr),
          username(_username), password(_password), database(_database) {
        setArgs(kArgs);
    }
    virtual ~LoginRequest() {}

    template <typename InputStreamTy>
    int readBody(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readString(username);
        readStatus |= is.readString(password);
        readStatus |= is.readString(database);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os) {
        os.writeString(username);
        os.writeString(password);
        os.writeString(database);
    }
};

struct LoginResponse : public Response<LoginResponse> {
    static const uint32_t kArgs = 1;
    int statusCode;

    LoginResponse(const char * data = nullptr)
        : Response(Message::LogoutRequest, data),
          statusCode(StatusCode::Unknown) {
        setArgs(kArgs);
    }
    LoginResponse(int _statusCode)
        : Response(Message::LogoutResponse, nullptr),
          statusCode(_statusCode) {
        setArgs(kArgs);
    }
    virtual ~LoginResponse() {}

    template <typename InputStreamTy>
    int readBody(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readInt32(statusCode);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os) {
        os.writeInt32(statusCode);
    }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGES_H
