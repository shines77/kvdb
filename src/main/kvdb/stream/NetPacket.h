
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
#include "kvdb/server/CommandType.h"

namespace kvdb {

struct PacketHeader {
    uint32_t command;
    uint32_t count;
    uint32_t total_size;

    PacketHeader() : command(CommandType::Unknown), count(0), total_size(0) {}
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
private:
    bool little_endian_;

    PacketHeader header_;
    std::vector<Variant> values_;

public:
    NetPacket() : little_endian_(false) { init(); }
    ~NetPacket() {}

    uint32_t getCommand() const { return header_.command; }

    void setCommand(uint32_t command) {
        header_.command = command;
    }

    PacketHeader & getHeaderInfo() { return header_; }
    const PacketHeader & getHeaderInfo() const { return header_; }

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

    size_t calcRequireSize(OutputStream & stream, size_t & count) {
        size_t totalSize = sizeof(PacketHeader);
        size_t valid_cnt = values_.size();
        // Calculation the body sizes.
        for (size_t i = 0; i < values_.size(); ++i) {
            Variant & value = values_[i];
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
                valid_cnt--;
                break;
            }
        }
        // Write the ending mark.
        totalSize += 1 + sizeof(uint8_t);

        count = valid_cnt;
        return totalSize;
    }

    int writeTo(OutputStream & stream) {
        size_t count = values_.size();

        size_t valid_cnt;
        size_t totalSize = calcRequireSize(stream, valid_cnt);

        // Write the header info.
        header_.count = (uint32_t)valid_cnt;
        header_.total_size = (uint32_t)totalSize;
        stream.writeUInt32(header_.command);
        stream.writeUInt32(header_.count);
        stream.writeUInt32(header_.total_size);

        // Write the body info.
        for (size_t i = 0; i < values_.size(); ++i) {
            Variant & var = values_[i];
            uint32_t type = var.getType();
            // Write the data type.
            stream.writeType(type);
            // Write the datas.
            switch (type) {
            case DataType::EndOf:
                break;
            case DataType::Bool:
                stream.writeBool(var.getValue().b);
                break;
            case DataType::Int8:
                stream.writeInt8(var.getValue().i8);
                break;
            case DataType::UInt8:
                stream.writeUInt8(var.getValue().u8);
                break;
            case DataType::Int16:
                stream.writeInt16(var.getValue().i16);
                break;
            case DataType::UInt16:
                stream.writeUInt16(var.getValue().u16);
                break;
            case DataType::Int32:
                stream.writeInt32(var.getValue().i32);
                break;
            case DataType::UInt32:
                stream.writeUInt32(var.getValue().u32);
                break;
            case DataType::Int64:
                stream.writeInt64(var.getValue().i64);
                break;
            case DataType::UInt64:
                stream.writeUInt64(var.getValue().u64);
                break;
            case DataType::Pointer:
                stream.writePointer(var.getValue().ptr);
                break;
            case DataType::Float:
                stream.writeFloat(var.getValue().f);
                break;
            case DataType::Double:
                stream.writeDouble(var.getValue().d);
                break;
            default:
                stream.back();
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
        size_t valid_counts = 0;
        size_t count;

        values_.clear();

        // Read the header info.
        header_.command = stream.readUInt32();
        header_.count = stream.readUInt32();
        header_.total_size = stream.readUInt32();

        count = valid_counts = header_.count;

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
                value.u8 = stream.readUInt8();
                variant.setUInt8(value.u8);
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
                valid_counts--;
                break;
            }

            if (isValidData) {
                values_.push_back(variant);
            }
        }

        return (int)valid_counts;
    }
};

} // namespace kvdb

#endif // KVDB_STREAM_NETPACKET_H
