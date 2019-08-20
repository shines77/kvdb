
#ifndef KVDB_STREAM_NETPACKET_H
#define KVDB_STREAM_NETPACKET_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <vector>
#include <string>

#include "kvdb/stream/Variant.h"
#include "kvdb/stream/InputStream.h"
#include "kvdb/stream/OutputStream.h"

namespace kvdb {

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

    std::vector<Variant> values_;

public:
    NetPacket() : little_endian_(false) { init(); }
    ~NetPacket() {}

    void init() {
        Variant value;
        value.setUInt32(118);
        values_.push_back(value);

        value.setUInt64(1118);
        values_.push_back(value);

        value.setInt8(11);
        values_.push_back(value);

        value.setPointer(NULL);
        values_.push_back(value);

        value.setPointer(nullptr);
        values_.push_back(value);

        value = true;
        values_.push_back(value);

        value = 8ULL;
        values_.push_back(value);
    }

    int serialize(OutputStream & stream) {
        size_t count = values_.size();
        for (size_t i = 0; i < values_.size(); ++i) {
            Variant & value = values_[i];
            int type = value.getType();
            Variant::Value v = value.getValue();
            switch (type) {
            case DataType::Bool:
                stream.writeBool(value.getValue().b);
                break;
            case DataType::Int8:
                stream.writeInt8(value.getValue().i8);
                break;
            case DataType::UInt8:
                stream.writeUInt8(value.getValue().u8);
                break;
            case DataType::Int16:
                stream.writeInt16(value.getValue().i16);
                break;
            case DataType::UInt16:
                stream.writeUInt16(value.getValue().u16);
                break;
            case DataType::Int32:
                stream.writeInt32(value.getValue().i32);
                break;
            case DataType::UInt32:
                stream.writeUInt32(value.getValue().u32);
                break;
            case DataType::Int64:
                stream.writeInt64(value.getValue().i64);
                break;
            case DataType::UInt64:
                stream.writeUInt64(value.getValue().u64);
                break;
            case DataType::Pointer:
                stream.writePointer(value.getValue().ptr);
                break;
            case DataType::Float:
                stream.writeFloat(value.getValue().f);
                break;
            case DataType::Double:
                stream.writeDouble(value.getValue().d);
                break;
            default:
                count--;
                break;
            }
        }
        return (int)count;
    }

    int unserialize(InputStream & stream) {}

};

} // namespace kvdb

#endif // KVDB_STREAM_NETPACKET_H
