
#ifndef KVDB_CORE_MESSAGE_H
#define KVDB_CORE_MESSAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

namespace kvdb {

static const uint32_t kDefaultSignId = 20190825;

struct MessageHeader {
    uint32_t sign;
    uint32_t type;
    uint32_t length;
    uint32_t args;

    MessageHeader() : sign(0), type(0), length(0), args(0) {}
    ~MessageHeader() {}
};

static const size_t kMsgHeaderSize = sizeof(MessageHeader);

#pragma warning (push)
#pragma warning (disable: 4200)

struct MessageData {
    MessageHeader header;
    char          data[0];
};

#pragma warning (pop)

struct MessageBody
{
    MessageHeader header;
    const char *  data;
};

class Message {
public:
    enum Type {
        Unknown = 0,

        LoginRequest,
        LoginResponse,

        HandShakeRequest,
        HandShakeResponse,

        ConnectRequest,
        ConnectResponse,

        QueryRequest,
        QueryResponse,

        LogoutRequest,
        LogoutResponse,

        Last
    };

private:
    //

public:
    Message() {}
    ~Message() {}
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_H
