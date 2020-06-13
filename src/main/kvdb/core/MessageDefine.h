
#ifndef KVDB_CORE_MESSAGE_DEFINE_H
#define KVDB_CORE_MESSAGE_DEFINE_H

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

struct LoginRequest_v0 : public Request<LoginRequest_v0> {
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 3);
    std::string sUsername;
    std::string sPassword;
    std::string sDatabase;

    LoginRequest_v0(const char * data = nullptr)
        : Request(MessageType::LoginRequest, data) {
        init();
    }
    LoginRequest_v0(std::string username, std::string password, std::string database)
        : Request(MessageType::LoginRequest, nullptr),
          sUsername(username), sPassword(password), sDatabase(database) {
    }
    virtual ~LoginRequest_v0() {}

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

struct LoginResponse_v0 : public Response<LoginResponse_v0> {
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    int iStatusCode;

    LoginResponse_v0(const char * data = nullptr)
        : Response(MessageType::LoginResponse, data) {
        init();
    }
    LoginResponse_v0(int statusCode)
        : Response(MessageType::LoginResponse, nullptr),
          iStatusCode(statusCode) {
    }
    virtual ~LoginResponse_v0() {}

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

struct HandShakeRequest_v0 : public Request<HandShakeRequest_v0> {
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    uint32_t iVersion;

    HandShakeRequest_v0(const char * data = nullptr)
        : Request(MessageType::HandShakeRequest, data) {
        init();
    }
    HandShakeRequest_v0(uint32_t version)
        : Request(MessageType::HandShakeRequest, nullptr),
          iVersion(version) {
    }
    virtual ~HandShakeRequest_v0() {}

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

struct HandShakeResponse_v0 : public Response<HandShakeResponse_v0> {
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    int iStatusCode;

    HandShakeResponse_v0(const char * data = nullptr)
        : Response(MessageType::HandShakeResponse, data) {
        init();
    }
    HandShakeResponse_v0(int statusCode)
        : Response(MessageType::HandShakeResponse, nullptr),
          iStatusCode(statusCode) {
    }
    virtual ~HandShakeResponse_v0() {}

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

struct ConnectRequest_v0 : public Request<ConnectRequest_v0> {
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    uint32_t iVersion;

    ConnectRequest_v0(const char * data = nullptr)
        : Request(MessageType::ConnectRequest, data) {
        init();
    }
    ConnectRequest_v0(uint32_t version)
        : Request(MessageType::ConnectRequest, nullptr),
          iVersion(version) {
    }
    virtual ~ConnectRequest_v0() {}

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

struct ConnectResponse_v0 : public Response<ConnectResponse_v0> {
    static const uint32_t kVerInfo = MAKE_MSG_VERINFO(0, 1);
    int iStatusCode;

    ConnectResponse_v0(const char * data = nullptr)
        : Response(MessageType::ConnectResponse, data) {
        init();
    }
    ConnectResponse_v0(int statusCode)
        : Response(MessageType::ConnectResponse, nullptr),
          iStatusCode(statusCode) {
    }
    virtual ~ConnectResponse_v0() {}

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

#endif // KVDB_CORE_MESSAGE_DEFINE_H
