
#ifndef KVDB_CORE_MESSAGE_HEADER_H
#define KVDB_CORE_MESSAGE_HEADER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

#define MAKE_MSG_VERINFO(ver, args)     (((ver << 16) & 0xFFFF0000U) | (args & 0x0000FFFFU))
#define GET_MSG_VERSION(info)           ((info & 0xFFFF0000U) >> 16)
#define GET_MSG_ARGS(info)              (info & 0x0000FFFFU)

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

struct VersionInfo {
    uint16_t version;
    uint16_t args;

    VersionInfo() : version(0), args(0) {}
    explicit VersionInfo(uint32_t info) : version(GET_MSG_VERSION(info)), args(GET_MSG_ARGS(info)) {}
    VersionInfo(uint16_t ver, uint16_t _args) : version(ver), args(_args) {}
    ~VersionInfo() {}

    uint32_t value() const { return *reinterpret_cast<const uint32_t *>(this); }

    void setValue(uint32_t value) {
        *reinterpret_cast<uint32_t *>(this) = value;
    }

    VersionInfo & operator = (uint32_t value) {
        this->setValue(value);
        return *this;
    }

    operator uint32_t() {
        return this->value();
    }
};

struct MessageHeader {
    uint32_t    sign;
    uint32_t    type;
    VersionInfo info;
    uint32_t    length;

    MessageHeader() : sign(0), type(0), info(0), length(0) {}
    ~MessageHeader() {}
};

static const uint32_t kMsgHeaderSize = sizeof(MessageHeader);

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_HEADER_H
