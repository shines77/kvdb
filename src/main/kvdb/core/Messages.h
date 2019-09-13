
#ifndef KVDB_CORE_MESSAGES_H
#define KVDB_CORE_MESSAGES_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>

#include "kvdb/core/Request.h"
#include "kvdb/core/Response.h"
#include "kvdb/core/StatusCode.h"
#include "kvdb/stream/BasicStream.h"
#include "kvdb/stream/InputPacketStream.h"
#include "kvdb/stream/OutputPacketStream.h"

namespace kvdb {

struct LoginRequest : public Request<LoginRequest> {
    static const uint32_t kArgs = 3;
    std::string sUsername;
    std::string sPassword;
    std::string sDatabase;

    LoginRequest(const char * data = nullptr)
        : Request(Message::LoginRequest, data) {
        setArgs(kArgs);
        init();
    }
    LoginRequest(std::string username, std::string password, std::string database)
        : Request(Message::LoginRequest, nullptr),
          sUsername(username), sPassword(password), sDatabase(database) {
        setArgs(kArgs);
    }
    virtual ~LoginRequest() {}

    void init() {
    }

    template <typename InputStreamTy>
    int readBody(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readString(sUsername);
        readStatus |= is.readString(sPassword);
        readStatus |= is.readString(sDatabase);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os) {
        os.writeString(sUsername);
        os.writeString(sPassword);
        os.writeString(sDatabase);
    }
};

struct LoginResponse : public Response<LoginResponse> {
    static const uint32_t kArgs = 1;
    int iStatusCode;

    LoginResponse(const char * data = nullptr)
        : Response(Message::LoginResponse, data),
          iStatusCode(StatusCode::Unknown) {
        setArgs(kArgs);
        init();
    }
    LoginResponse(int statusCode)
        : Response(Message::LoginResponse, nullptr),
          iStatusCode(statusCode) {
        setArgs(kArgs);
    }
    virtual ~LoginResponse() {}

    void init() {
        iStatusCode = StatusCode::Unknown;
    }

    template <typename InputStreamTy>
    int readBody(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readInt32(iStatusCode);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os) {
        os.writeInt32(iStatusCode);
    }
};

struct HandShakeRequest : public Request<HandShakeRequest> {
    static const uint32_t kArgs = 3;
    uint32_t iVersion;

    HandShakeRequest(const char * data = nullptr)
        : Request(Message::HandShakeRequest, data),
          iVersion(0) {
        setArgs(kArgs);
        init();
    }
    HandShakeRequest(uint32_t version)
        : Request(Message::HandShakeRequest, nullptr),
          iVersion(version) {
        setArgs(kArgs);
    }
    virtual ~HandShakeRequest() {}

    void init() {
        iVersion = 0;
    }

    template <typename InputStreamTy>
    int readBody(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readUInt32(iVersion);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os) {
        os.writeUInt32(iVersion);
    }
};

struct HandShakeResponse : public Response<HandShakeResponse> {
    static const uint32_t kArgs = 1;
    int iStatusCode;

    HandShakeResponse(const char * data = nullptr)
        : Response(Message::HandShakeResponse, data),
          iStatusCode(StatusCode::Unknown) {
        setArgs(kArgs);
        init();
    }
    HandShakeResponse(int statusCode)
        : Response(Message::HandShakeResponse, nullptr),
          iStatusCode(statusCode) {
        setArgs(kArgs);
    }
    virtual ~HandShakeResponse() {}

    void init() {
        iStatusCode = 0;
    }

    template <typename InputStreamTy>
    int readBody(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readInt32(iStatusCode);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os) {
        os.writeInt32(iStatusCode);
    }
};

struct ConnectRequest : public Request<ConnectRequest> {
    static const uint32_t kArgs = 3;
    uint32_t iVersion;

    ConnectRequest(const char * data = nullptr)
        : Request(Message::ConnectRequest, data),
          iVersion(0) {
        setArgs(kArgs);
        init();
    }
    ConnectRequest(uint32_t version)
        : Request(Message::ConnectRequest, nullptr),
          iVersion(version) {
        setArgs(kArgs);
    }
    virtual ~ConnectRequest() {}

    void init() {
        iVersion = 0;
    }

    template <typename InputStreamTy>
    int readBody(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readUInt32(iVersion);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os) {
        os.writeUInt32(iVersion);
    }
};

struct ConnectResponse : public Response<ConnectResponse> {
    static const uint32_t kArgs = 1;
    int iStatusCode;

    ConnectResponse(const char * data = nullptr)
        : Response(Message::ConnectResponse, data),
          iStatusCode(StatusCode::Unknown) {
        setArgs(kArgs);
        init();
    }
    ConnectResponse(int statusCode)
        : Response(Message::ConnectResponse, nullptr),
          iStatusCode(statusCode) {
        setArgs(kArgs);
    }
    virtual ~ConnectResponse() {}

    void init() {
        iStatusCode = 0;
    }

    template <typename InputStreamTy>
    int readBody(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readInt32(iStatusCode);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os) {
        os.writeInt32(iStatusCode);
    }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGES_H
