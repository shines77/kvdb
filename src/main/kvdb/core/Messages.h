
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

//#include "kvdb/stream/PrepareInputPacketStream.h"
#include "kvdb/stream/PrepareOutputPacketStream.h"

namespace kvdb {

struct LoginRequest : public Request {
    std::string username;
    std::string password;
    std::string database;

    LoginRequest() {}
    LoginRequest(std::string _username, std::string _password, std::string _database)
        : username(_username), password(_password), database(_database) {
    }
    ~LoginRequest() {}

    template <typename PrepareOutputStreamTy>
    uint32_t prepare(PrepareOutputStreamTy & os) {
        os.reset();
        os.writeString(username);
        os.writeString(password);
        os.writeString(database);
        return os.requireSize();
    }

    template <typename InputStreamTy>
    int readFrom(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readString(username);
        readStatus |= is.readString(password);
        readStatus |= is.readString(database);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeTo(OutputStreamTy & os) {
        os.writeString(username);
        os.writeString(password);
        os.writeString(database);
    }
};

struct LoginResponse : public Response {
    int statusCode;

    LoginResponse(int _statusCode = StatusCode::Unknown)
        : statusCode(_statusCode) {
    }
    ~LoginResponse() {}

    template <typename PrepareOutputStreamTy>
    uint32_t prepare(PrepareOutputStreamTy & os) {
        os.reset();
        os.writeInt32(statusCode);
        return os.requireSize();
    }

    template <typename InputStreamTy>
    int readFrom(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readInt32(statusCode);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeTo(OutputStreamTy & os) {
        os.writeInt32(statusCode);
    }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGES_H
