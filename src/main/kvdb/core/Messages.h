
#ifndef KVDB_CORE_MESSAGES_H
#define KVDB_CORE_MESSAGES_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>
#include <stddef.h>

#include <cstdint>
#include <cstddef>
#include <string>

#include "kvdb/basic/stdint.h"
#include "kvdb/basic/stdsize.h"
#include "kvdb/core/Request.h"
#include "kvdb/core/Response.h"
#include "kvdb/core/StatusCode.h"
#include "kvdb/stream/ReadWriteResult.h"
#include "kvdb/stream/BasicStream.h"
#include "kvdb/stream/PackagedInputStream.h"
#include "kvdb/stream/PackagedOutputStream.h"

#include <string>

namespace kvdb {

struct LoginRequest_v0 : public Request<LoginRequest_v0> {
    static const uint8_t kMsgVer = 0;

    std::string sUsername;
    std::string sPassword;
    std::string sDatabase;

    LoginRequest_v0(const char * data = nullptr)
        : Request(Opcode::LoginRequest, data) {
        init();
    }
    LoginRequest_v0(std::string username, std::string password, std::string database)
        : Request(Opcode::LoginRequest, nullptr),
          sUsername(username), sPassword(password), sDatabase(database) {
    }
    virtual ~LoginRequest_v0() {}

    void init() {
    }

    template <typename InputStreamTy>
    int read(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readString(sUsername);
        readStatus |= is.readString(sPassword);
        readStatus |= is.readString(sDatabase);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void write(OutputStreamTy & os) {
        os.writeString(sUsername);
        os.writeString(sPassword);
        os.writeString(sDatabase);
    }
};

struct LoginResponse_v0 : public Response<LoginResponse_v0> {
    static const uint8_t kMsgVer = 0;

    int iStatusCode;

    LoginResponse_v0(const char * data = nullptr)
        : Response(Opcode::LoginResponse, data) {
        init();
    }
    LoginResponse_v0(int statusCode)
        : Response(Opcode::LoginResponse, nullptr),
          iStatusCode(statusCode) {
    }
    virtual ~LoginResponse_v0() {}

    void init() {
        iStatusCode = StatusCode::Unknown;
    }

    template <typename InputStreamTy>
    int read(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readInt32(iStatusCode);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void write(OutputStreamTy & os) {
        os.writeInt32(iStatusCode);
    }
};

struct HandShakeRequest_v0 : public Request<HandShakeRequest_v0> {
    static const uint8_t kMsgVer = 0;

    uint32_t iVersion;

    HandShakeRequest_v0(const char * data = nullptr)
        : Request(Opcode::HandShakeRequest, data) {
        init();
    }
    HandShakeRequest_v0(uint32_t version)
        : Request(Opcode::HandShakeRequest, nullptr),
          iVersion(version) {
    }
    virtual ~HandShakeRequest_v0() {}

    void init() {
        iVersion = 0;
    }

    template <typename InputStreamTy>
    int read(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readUInt32(iVersion);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void write(OutputStreamTy & os) {
        os.writeUInt32(iVersion);
    }
};

struct HandShakeResponse_v0 : public Response<HandShakeResponse_v0> {
    static const uint8_t kMsgVer = 0;

    int iStatusCode;

    HandShakeResponse_v0(const char * data = nullptr)
        : Response(Opcode::HandShakeResponse, data) {
        init();
    }
    HandShakeResponse_v0(int statusCode)
        : Response(Opcode::HandShakeResponse, nullptr),
          iStatusCode(statusCode) {
    }
    virtual ~HandShakeResponse_v0() {}

    void init() {
        iStatusCode = StatusCode::Unknown;
    }

    template <typename InputStreamTy>
    int read(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readInt32(iStatusCode);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void write(OutputStreamTy & os) {
        os.writeInt32(iStatusCode);
    }
};

struct LogoutRequest_v0 : public Request<LogoutRequest_v0> {
    static const uint8_t kMsgVer = 0;

    uint32_t iVersion;

    LogoutRequest_v0(const char * data = nullptr)
        : Request(Opcode::LogoutRequest, data) {
        init();
    }
    LogoutRequest_v0(uint32_t version)
        : Request(Opcode::LogoutRequest, nullptr),
          iVersion(version) {
    }
    virtual ~LogoutRequest_v0() {}

    void init() {
        iVersion = 0;
    }

    template <typename InputStreamTy>
    int read(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readUInt32(iVersion);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void write(OutputStreamTy & os) {
        os.writeUInt32(iVersion);
    }
};

struct LogoutResponse_v0 : public Response<LogoutResponse_v0> {
    static const uint8_t kMsgVer = 0;

    int iStatusCode;

    LogoutResponse_v0(const char * data = nullptr)
        : Response(Opcode::LogoutResponse, data) {
        init();
    }
    LogoutResponse_v0(int statusCode)
        : Response(Opcode::LogoutResponse, nullptr),
          iStatusCode(statusCode) {
    }
    virtual ~LogoutResponse_v0() {}

    void init() {
        iStatusCode = StatusCode::Unknown;
    }

    template <typename InputStreamTy>
    int read(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readInt32(iStatusCode);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void write(OutputStreamTy & os) {
        os.writeInt32(iStatusCode);
    }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGES_H
