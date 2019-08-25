
#ifndef KVDB_STREAM_NETPACKET_H
#define KVDB_STREAM_NETPACKET_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <vector>
#include <string>

#include "kvdb/core/MessageType.h"
#include "kvdb/core/Variant.h"
#include "kvdb/stream/InputStream.h"
#include "kvdb/stream/OutputStream.h"

namespace kvdb {

struct PacketHeader {
    uint32_t msgType;
    uint32_t msgLength;
    uint32_t varCount;

    PacketHeader() : msgType(MessageType::Unknown), msgLength(0), varCount(0) {}
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

class NetPacket {
protected:
    bool is_little_endian_;

public:
    PacketHeader header;
    std::vector<Variant> values;

public:
    NetPacket() : is_little_endian_(false) { init(); }
    virtual ~NetPacket() {}

    uint32_t getMsgType() const { return header.msgType; }
    uint32_t getMsgLength() const { return header.msgLength; }

    void setMsgType(uint32_t msgType) {
        header.msgType = msgType;
    }
    void setMsgLength(uint32_t msgLength) {
        header.msgLength = msgLength;
    }

    void init() {
        Variant value;
        value.setUInt32(118);
        values.push_back(value);

        value.setUInt64(1118);
        values.push_back(value);

        value.setInt8(11);
        values.push_back(value);

        value.setPointer(NULL);
        values.push_back(value);

        value.setPointer(nullptr);
        values.push_back(value);

        value = true;
        values.push_back(value);

        value = 8ULL;
        values.push_back(value);
    }

    size_t calcRequireSize(size_t & count) {
        size_t totalSize = sizeof(PacketHeader);
        size_t valid_count = values.size();
        // Calculation the body sizes.
        for (size_t i = 0; i < values.size(); ++i) {
            Variant & value = values[i];
            uint32_t type = value.getType();
            // Write the data type.
            totalSize++;
            // Write the data.
            switch (type) {
            case DataType::EndOf:
                break;
            case DataType::Bool:
                totalSize += sizeof(bool);
                break;
            case DataType::Int8:
            case DataType::UInt8:
                totalSize += sizeof(uint8_t);
                break;
            case DataType::Int16:
            case DataType::UInt16:
                totalSize += sizeof(uint16_t);
                break;
            case DataType::Int32:
            case DataType::UInt32:
                totalSize += sizeof(uint32_t);
                break;
            case DataType::Int64:
            case DataType::UInt64:
                totalSize += sizeof(uint64_t);
                break;
            case DataType::Pointer:
                totalSize += sizeof(void *);
                break;
            case DataType::Float:
                totalSize += sizeof(float);
                break;
            case DataType::Double:
                totalSize += sizeof(double);
                break;
            default:
                totalSize--;
                valid_count--;
                break;
            }
        }
        // Write the ending mark.
        totalSize += 1 + sizeof(uint8_t);

        count = valid_count;
        return totalSize;
    }

    int writeTo(OutputStream & stream) {
        size_t count = values.size();

        size_t valid_count;
        size_t totalSize = calcRequireSize(valid_count);

        // Write the header info.
        header.msgLength = (uint32_t)totalSize;
        header.varCount = (uint32_t)valid_count;
        stream.writeUInt32(header.msgLength);
        stream.writeUInt32(header.msgType);
        stream.writeUInt32(header.varCount);

        // Write the body info.
        for (size_t i = 0; i < values.size(); ++i) {
            Variant & var = values[i];
            uint32_t type = var.getType();
            // Write the data type.
            
            // Write the datas.
            switch (type) {
            case DataType::EndOf:
                break;
            case DataType::Bool:
                stream.writeType(type);
                stream.writeBool(var.getValue().b);
                break;
            case DataType::Int8:
                stream.writeType(type);
                stream.writeInt8(var.getValue().i8);
                break;
            case DataType::UInt8:
                stream.writeType(type);
                stream.writeUInt8(var.getValue().u8);
                break;
            case DataType::Int16:
                stream.writeType(type);
                stream.writeInt16(var.getValue().i16);
                break;
            case DataType::UInt16:
                stream.writeType(type);
                stream.writeUInt16(var.getValue().u16);
                break;
            case DataType::Int32:
                stream.writeType(type);
                stream.writeInt32(var.getValue().i32);
                break;
            case DataType::UInt32:
                stream.writeType(type);
                stream.writeUInt32(var.getValue().u32);
                break;
            case DataType::Int64:
                stream.writeType(type);
                stream.writeInt64(var.getValue().i64);
                break;
            case DataType::UInt64:
                stream.writeType(type);
                stream.writeUInt64(var.getValue().u64);
                break;
            case DataType::Pointer:
                stream.writeType(type);
                stream.writePointer(var.getValue().ptr);
                break;
            case DataType::Float:
                stream.writeType(type);
                stream.writeFloat(var.getValue().f);
                break;
            case DataType::Double:
                stream.writeType(type);
                stream.writeDouble(var.getValue().d);
                break;
            default:
                count--;
                break;
            }
        }

        // Write the ending mark.
        stream.writeUInt8((uint8_t)DataType::EndOf);
        stream.writeUInt8(0);
        return (int)count;
    }

    int readFrom(InputStream & stream) {
        size_t valid_count = 0;
        size_t count;

        values.clear();

        // Read the header info.
        header.msgLength = stream.readUInt32();
        header.msgType = stream.readUInt32();
        header.varCount = stream.readUInt32();

        count = valid_count = header.varCount;

        // Read the body info.
        for (size_t i = 0; i < count; ++i) {
            Variant variant;
            Variant::Value value; 
            bool isValidData = true;
            // Read the data type.
            uint32_t type = stream.readType();
            variant.setType(type);

            // Read the datas.
            switch (type) {
            case DataType::EndOf:
                isValidData = false;
                valid_count--;
                // Exit the for loop immediately.
                i = count;
                break;
            case DataType::Bool:
                value.b = stream.readBool();
                variant.setBool(value.b);
                break;
            case DataType::Int8:
                value.i8 = stream.readInt8();
                variant.setInt8(value.i8);
                break;
            case DataType::UInt8:
                value.u8 = stream.readUInt8();
                variant.setUInt8(value.u8);
                break;
            case DataType::Int16:
                value.i16 = stream.readInt16();
                variant.setInt16(value.i16);
                break;
            case DataType::UInt16:
                value.u16 = stream.readUInt16();
                variant.setUInt16(value.u16);
                break;
            case DataType::Int32:
                value.i32 = stream.readInt32();
                variant.setInt32(value.i32);
                break;
            case DataType::UInt32:
                value.u32 = stream.readUInt32();
                variant.setUInt32(value.u32);
                break;
            case DataType::Int64:
                value.i64 = stream.readInt64();
                variant.setInt64(value.i64);
                break;
            case DataType::UInt64:
                value.u64 = stream.readUInt64();
                variant.setUInt64(value.u64);
                break;
            case DataType::Pointer:
                value.ptr = stream.readPointer();
                variant.setPointer(value.ptr);
                break;
            case DataType::Float:
                value.f = stream.readFloat();
                variant.setFloat(value.f);
                break;
            case DataType::Double:
                value.d = stream.readDouble();
                variant.setDouble(value.d);
                break;
            default:
                isValidData = false;
                valid_count--;
                break;
            }

            if (isValidData) {
                values.push_back(variant);
            }
        }

        return (int)valid_count;
    }
};

} // namespace kvdb

#endif // KVDB_STREAM_NETPACKET_H
