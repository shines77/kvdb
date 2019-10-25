
#ifndef KVDB_CORE_MESSAGES_DEFINE_H
#define KVDB_CORE_MESSAGES_DEFINE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/core/Request.h"
#include "kvdb/core/Response.h"
#include "kvdb/core/StatusCode.h"
#include "kvdb/stream/BasicStream.h"
#include "kvdb/stream/InputPacketStream.h"
#include "kvdb/stream/OutputPacketStream.h"

#include <string>

namespace kvdb {

struct LoginRequest : public Request<LoginRequest> {
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 3);
    std::string sUsername;
    std::string sPassword;
    std::string sDatabase;

    LoginRequest(const char * data = nullptr)
        : Request(MessageType::LoginRequest, data) {
        init();
    }
    LoginRequest(std::string username, std::string password, std::string database)
        : Request(MessageType::LoginRequest, nullptr),
          sUsername(username), sPassword(password), sDatabase(database) {
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
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    int iStatusCode;

    LoginResponse(const char * data = nullptr)
        : Response(MessageType::LoginResponse, data) {
        init();
    }
    LoginResponse(int statusCode)
        : Response(MessageType::LoginResponse, nullptr),
          iStatusCode(statusCode) {
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
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    uint32_t iVersion;

    HandShakeRequest(const char * data = nullptr)
        : Request(MessageType::HandShakeRequest, data) {
        init();
    }
    HandShakeRequest(uint32_t version)
        : Request(MessageType::HandShakeRequest, nullptr),
          iVersion(version) {
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
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    int iStatusCode;

    HandShakeResponse(const char * data = nullptr)
        : Response(MessageType::HandShakeResponse, data) {
        init();
    }
    HandShakeResponse(int statusCode)
        : Response(MessageType::HandShakeResponse, nullptr),
          iStatusCode(statusCode) {
    }
    virtual ~HandShakeResponse() {}

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

struct ConnectRequest : public Request<ConnectRequest> {
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    uint32_t iVersion;

    ConnectRequest(const char * data = nullptr)
        : Request(MessageType::ConnectRequest, data) {
        init();
    }
    ConnectRequest(uint32_t version)
        : Request(MessageType::ConnectRequest, nullptr),
          iVersion(version) {
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
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    int iStatusCode;

    ConnectResponse(const char * data = nullptr)
        : Response(MessageType::ConnectResponse, data) {
        init();
    }
    ConnectResponse(int statusCode)
        : Response(MessageType::ConnectResponse, nullptr),
          iStatusCode(statusCode) {
    }
    virtual ~ConnectResponse() {}

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

} // namespace kvdb

#endif // KVDB_CORE_MESSAGES_DEFINE_H
