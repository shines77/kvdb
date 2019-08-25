
#ifndef KVDB_STREAM_PACKET_H
#define KVDB_STREAM_PACKET_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/core/MessageType.h"

namespace kvdb {

static const uint32_t kDefaultSignId = 20190825;

struct PacketHeader {
    uint32_t signId;
    uint32_t msgType;
    uint32_t msgLength;
    uint32_t varCount;

    PacketHeader() : signId(0), msgType(MessageType::Unknown), msgLength(0), varCount(0) {}
    ~PacketHeader() {}
};

#pragma warning (push)
#pragma warning (disable: 4200)

struct PacketData {
    PacketHeader header;
    char         data[0];
};

#pragma warning (pop)

struct PacketBody
{
    PacketHeader  header;
    const char *  data;
};

} // namespace kvdb

#endif // KVDB_STREAM_PACKET_H
