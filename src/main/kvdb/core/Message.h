
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

        DisconnectRequest,
        DisconnectResponse,

        LogoutRequest,
        LogoutResponse,

        Last
    };

    MessageHeader header;

protected:
    const char * data_;

public:
    Message(uint32_t type = Message::Unknown, const char * data = nullptr) : data_(data) {
        this->header.sign = kDefaultSignId;
        this->header.type = type;
    }

    virtual ~Message() {}

    char * data() { return (char *)this->data_; }
    const char * data() const { return this->data_; }

    void setData(const char * data) { this->data_ = data; }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_H
