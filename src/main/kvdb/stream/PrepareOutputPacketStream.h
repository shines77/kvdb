
#ifndef KVDB_STREAM_PREPARE_OUTPUTPACKETSTREAM_H
#define KVDB_STREAM_PREPARE_OUTPUTPACKETSTREAM_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdint>
#include <string>
#include <type_traits>

#include "kvdb/stream/PrepareOutputStream.h"
#include "kvdb/core/Variant.h"

namespace kvdb {

template <typename T>
class BasicPrepareOutputPacketStream {
public:
    typedef BasicPrepareOutputStream<T>     stream_type;
    typedef typename stream_type::char_type char_type;

    typedef std::basic_string<char_type>    string_type;
    typedef jstd::BasicStringRef<char_type> stringref_type;

    stream_type stream;

    BasicPrepareOutputPacketStream() : stream() {}
    BasicPrepareOutputPacketStream(const char_type * value) : stream(value) {}
    template <size_t N>
    BasicPrepareOutputPacketStream(const char_type(&data)[N]) : stream(data) {}
    ~BasicPrepareOutputPacketStream() {}

    char_type * head() const {
        return stream.head();
    }

    char_type * current() const {
        return stream.current();
    }

    ptrdiff_t length() const { return stream.length(); }
    uint32_t getMsgLength() const {
        return (uint32_t)(stream.length() - sizeof(PacketHeader));
    }

    void reset() {
        stream.reset();
    }

    void skipToHeader() {
        stream.skipToHeader();
    }

    uint32_t requireSize() {
        return (uint32_t)stream.length();
    }

    void writeType(uint8_t type) {
        stream.writeUInt8(type);
    }

    void writeType(uint32_t type) {
        stream.writeUInt8((uint8_t)type);
    }

    void writeByte(uint8_t value) {
        stream.writeType(DataType::UInt8);
        stream.writeByte(value);
    }

    void writeChar(char_type value) {
        stream.writeType(DataType::UInt8);
        stream.writeByte(value);
    }

    void writeBool(bool value) {
        stream.writeType(DataType::Bool);
        stream.writeBool(value);
    }

    void writeInt8(int8_t value) {
        stream.writeType(DataType::Int8);
        stream.writeInt8(value);
    }

    void writeInt16(int16_t value) {
        stream.writeType(DataType::Int16);
        stream.writeInt16(value);
    }

    void writeInt32(int32_t value) {
        stream.writeType(DataType::Int32);
        stream.writeInt32(value);
    }

    void writeInt64(int64_t value) {
        stream.writeType(DataType::Int64);
        stream.writeInt64(value);
    }

    void writeUInt8(uint8_t value) {
        stream.writeType(DataType::UInt8);
        stream.writeUInt8(value);
    }

    void writeUInt16(uint16_t value) {
        stream.writeType(DataType::UInt16);
        stream.writeUInt16(value);
    }

    void writeUInt32(uint32_t value) {
        stream.writeType(DataType::UInt32);
        stream.writeUInt32(value);
    }

    void writeUInt64(uint64_t value) {
        stream.writeType(DataType::UInt64);
        stream.writeUInt64(value);
    }

    void writePointer(void * value) {
        stream.writeType(DataType::Pointer);
        stream.writePointer(value);
    }

    void writeFloat(float value) {
        stream.writeType(DataType::Float);
        stream.writeFloat(value);
    }

    void writeDouble(double value) {
        stream.writeType(DataType::Double);
        stream.writeDouble(value);
    }

    int writeValue(Variant & variant) {
        int result = WriteResult::Ok;
        uint8_t type = variant.getType();
        switch (type) {
        case DataType::Bool:
            stream.writeType(Type);
            stream.writeBool(variant.value.b);
            break;
        case DataType::Int8:
            stream.writeType(Type);
            stream.writeInt8(variant.value.i8);
            break;
        case DataType::Int16:
            stream.writeType(Type);
            stream.writeInt16(variant.value.i16);
            break;
        case DataType::Int32:
            stream.writeType(Type);
            stream.writeInt32(variant.value.i32);
            break;
        case DataType::Int64:
            stream.writeType(Type);
            stream.writeInt64(variant.value.i64);
            break;
        case DataType::UInt8:
            stream.writeType(Type);
            stream.writeUInt8(variant.value.u8);
            break;
        case DataType::UInt16:
            stream.writeType(Type);
            stream.writeUInt16(variant.value.u16);
            break;
        case DataType::UInt32:
            stream.writeType(Type);
            stream.writeUInt32(variant.value.u32);
            break;
        case DataType::UInt64:
            stream.writeType(Type);
            stream.writeUInt64(variant.value.u64);
            break;
        case DataType::Pointer:
            stream.writeType(Type);
            stream.writePointer(variant.value.ptr);
            break;
        case DataType::Float:
            stream.writeType(Type);
            stream.writeFloat(variant.value.f);
            break;
        case DataType::Double:
            stream.writeType(Type);
            stream.writeDouble(variant.value.d);
            break;
        default:
            result = WriteResult::Failed;
        }
        return result;
    }

    template <typename StringType>
    void writeString(const StringType & value) {
        int result = WriteResult::Ok;
        size_t length = value.size();
        if (length < 256) {
            stream.writeType(DataType::String1B);
            stream.writeUInt8((uint8_t)length);
            stream.writeString(value);
        }
        else if (length < 65536) {
            stream.writeType(DataType::String2B);
            stream.writeUInt16((uint16_t)length);
            stream.writeString(value);
        }
        else if (length < 16777216) {
#if IS_BIG_ENDIAN
            uint32_t value32 = (DataType::String3B & 0x000000FFUL) | (length & 0xFFFFFF00UL);
            stream.writeUInt32(value32);
#else
            uint32_t value32 = (DataType::String3B << 24) | (length & 0x00FFFFFFUL);
            stream.writeUInt32(value32);
#endif
            stream.writeString(value);
        }
        else {
            stream.writeType(DataType::String);
            stream.writeUInt32((uint32_t)length);
            stream.writeString(value);
        }
    }
};

typedef BasicPrepareOutputPacketStream<char>      PrepareOutputPacketStream;
typedef BasicPrepareOutputPacketStream<wchar_t>   PrepareOutputPacketStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_PREPARE_OUTPUTPACKETSTREAM_H
