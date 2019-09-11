
#ifndef KVDB_STREAM_PACKET_H
#define KVDB_STREAM_PACKET_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/core/Message.h"

namespace kvdb {

struct PacketHeader {
    uint32_t sign;
    uint32_t type;
    uint32_t length;
    uint32_t count;

    PacketHeader() : sign(0), type(0), length(0), count(0) {}
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

class Packet {
public:
    enum Type {
        Unknown = 0,

        Last
    };

private:
    //

public:
    Packet() {}
    ~Packet() {}
};

} // namespace kvdb

#endif // KVDB_STREAM_PACKET_H
