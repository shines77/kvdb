
#ifndef KVDB_STREAM_NETPACKET_H
#define KVDB_STREAM_NETPACKET_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <vector>
#include <string>

#include "kvdb/stream/Variant.h"
#include "kvdb/stream/InputStream.h"

namespace kvdb {

class OutputStream {

};

struct PacketHeader {
    uint32_t command;
    uint32_t count;
    uint32_t total_size;
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

class NetPacket {
private:
    bool little_endian_;

    std::vector<Variant> elements_;

public:
    NetPacket() : little_endian_(false) { init(); }
    ~NetPacket() {}

    void init() {
        Variant value;
        value.setUInt32(118);
        elements_.push_back(value);

        value.setUInt64(1118);
        elements_.push_back(value);

        value.setInt8(11);
        elements_.push_back(value);

        value.setPointer(NULL);
        elements_.push_back(value);

        value.setPointer(nullptr);
        elements_.push_back(value);

        value = true;
        elements_.push_back(value);

        value = 8ULL;
        elements_.push_back(value);
    }

    int serialize(OutputStream & stream) {
        for (size_t i = 0; i < elements_.size(); ++i) {
            Variant & value = elements_[i];
            value.getType();
            Variant::Value v = value.getValue();
            v.d = v.d;
        }
        return (int)elements_.size();
    }

    int unserialize(InputStream & stream) {}

};

} // namespace kvdb

#endif // KVDB_STREAM_NETPACKET_H
