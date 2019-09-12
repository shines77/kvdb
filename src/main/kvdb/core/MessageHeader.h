
#ifndef KVDB_CORE_MESSAGE_HEADER_H
#define KVDB_CORE_MESSAGE_HEADER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

namespace kvdb {

struct ReadResult {
    enum Type {
        Failed = -1,
        Ok = 0,
        Last
    };
};

struct WriteResult {
    enum Type {
        Failed = -1,
        Ok = 0,
        Last
    };
};

struct MessageHeader {
    uint32_t sign;
    uint32_t type;
    uint32_t args;
    uint32_t length;

    MessageHeader() : sign(0), type(0), args(0), length(0) {}
    ~MessageHeader() {}
};

static const uint32_t kMsgHeaderSize = sizeof(MessageHeader);

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_HEADER_H
