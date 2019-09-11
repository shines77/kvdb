
#ifndef KVDB_STREAM_INPUTPACKETSTREAM_H
#define KVDB_STREAM_INPUTPACKETSTREAM_H

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

#include "kvdb/core/Variant.h"
#include "kvdb/core/Packet.h"
#include "kvdb/stream/InputStream.h"

namespace kvdb {

template <typename T>
class BasicInputPacketStream {
public:
    typedef BasicInputStream<T>             stream_type;
    typedef typename stream_type::char_type char_type;

    typedef std::basic_string<char_type>    string_type;
    typedef jstd::BasicStringRef<char_type> stringref_type;

    stream_type stream;

    BasicInputPacketStream() : stream() {}
    BasicInputPacketStream(const char_type * value) : stream(value) {}
    template <size_t N>
    BasicInputPacketStream(const char_type(&data)[N]) : stream(data) {}
    ~BasicInputPacketStream() {}

    char_type * head() const { return stream.head(); }
    char_type * current() const { return stream.current(); }

    char_type * data() const { return stream.data(); }

    ptrdiff_t length() const { return stream.length(); }
    uint32_t getMsgLength() const {
        return (uint32_t)(stream.length() - sizeof(MessageHeader));
    }

    void reset() {
        stream.reset();
    }

    void skipToHeader() {
        stream.skipToHeader();
    }

    int readHeader(MessageHeader & header) {
        header.sign = stream.readUInt32();
        header.type = stream.readUInt32();
        header.length = stream.readUInt32();
        header.args = stream.readUInt32();
        return ReadResult::Ok;
    }

    uint8_t readType() {
        return stream.readUInt8();
    }

    bool readByte(uint8_t & correct) {
        uint8_t type = stream.readType();
        value = stream.readByte();
        return (type == DataType::Int8 || type == DataType::UInt8);
    }

    bool readChar(char_type & correct) {
        uint8_t type = stream.readType();
        bool correct = ((typeid(char_type) == typeid(int8_t)   && type == DataType::Int8)
                     || (typeid(char_type) == typeid(uint8_t)  && type == DataType::UInt8)
                     || (typeid(char_type) == typeid(int16_t)  && type == DataType::Int16)
                     || (typeid(char_type) == typeid(uint16_t) && type == DataType::UInt16));
        value = stream.readChar();
        return correct;
    }

    bool readBool(bool & value) {
        uint8_t type = stream.readType();
        value = stream.readBool();
        return (type == DataType::Bool);
    }

    bool readInt8(int8_t & value) {
        uint8_t type = stream.readType();
        value = stream.readInt8();
        return (type == DataType::Int8);
    }

    bool readInt16(int16_t & value) {
        uint8_t type = stream.readType();
        value = stream.readInt16();
        return (type == DataType::Int16);
    }

    bool readInt32(int32_t & value) {
        uint8_t type = stream.readType();
        value = stream.readInt32();
        return (type == DataType::Int32);
    }

    bool readInt64(int64_t & value) {
        uint8_t type = stream.readType();
        value = stream.readInt64();
        return (type == DataType::Int64);
    }

    bool readUInt8(uint8_t & value) {
        uint8_t type = stream.readType();
        value = stream.readUInt8();
        return (type == DataType::UInt8);
    }

    bool readUInt16(uint16_t & value) {
        uint8_t type = stream.readType();
        value = stream.readUInt16();
        return (type == DataType::UInt16);
    }

    bool readUInt32(uint32_t & value) {
        uint8_t type = stream.readType();
        value = stream.readUInt32();
        return (type == DataType::UInt32);
    }

    bool readUInt64(uint64_t & value) {
        uint8_t type = stream.readType();
        value = stream.readUInt64();
        return (type == DataType::UInt64);
    }

    bool readPointer(void * & value) {
        uint8_t type = stream.readType();
        value = stream.readPointer();
        return (type == DataType::Pointer);
    }

    bool readFloat(float & value) {
        uint8_t type = stream.readType();
        value = stream.readFloat();
        return (type == DataType::Float);
    }

    bool readDouble(double & value) {
        uint8_t type = stream.readType();
        value = stream.readDouble();
        return (type == DataType::Double);
    }

    int readValue(Variant & variant) {
        int result = ReadResult::Ok;
        uint8_t type = stream.readType();
        variant.type = type;
        switch (type) {
        case DataType::Bool:
            variant.value.b = stream.readBool();
            break;
        case DataType::Int8:
            variant.value.i8 = stream.readInt8();
            break;
        case DataType::Int16:
            variant.value.i16 = stream.readInt16();
            break;
        case DataType::Int32:
            variant.value.i32 = stream.readInt32();
            break;
        case DataType::Int64:
            variant.value.i64 = stream.readInt64();
            break;
        case DataType::UInt8:
            variant.value.u8 = stream.readUInt8();
            break;
        case DataType::UInt16:
            variant.value.u16 = stream.readUInt16();
            break;
        case DataType::UInt32:
            variant.value.u32 = stream.readUInt32();
            break;
        case DataType::UInt64:
            variant.value.u64 = stream.readUInt64();
            break;
        case DataType::Pointer:
            variant.value.ptr = stream.readPointer();
            break;
        case DataType::Float:
            variant.value.f = stream.readFloat();
            break;
        case DataType::Double:
            variant.value.d = stream.readDouble();
            break;
        default:
            result = ReadResult::Failed;
        }
        return result;
    }

    template <typename StringType>
    int readString(StringType & value) {
        size_t length;
        int result = ReadResult::Ok;
        uint8_t type = stream.getType();
        switch (type) {
        case DataType::String:
            stream.nextUInt8();
            length = stream.readUInt32();
            stream.readString(value, length);
            break;

        case DataType::String1B:
            stream.nextUInt8();
            length = stream.readUInt8();
            stream.readString(value, length);
            break;

        case DataType::String2B:
            stream.nextUInt8();
            length = stream.readUInt16();
            stream.readString(value, length);
            break;

        case DataType::String3B:
        {
            uint32_t u32 = stream.readUInt32();
#if IS_BIG_ENDIAN
            length = (u32 & 0xFFFFFF00UL) >> 8;
#else
            length = u32 & 0x00FFFFFFUL;
#endif
            stream.readString(value, length);
            break;
        }

        default:
            result = ReadResult::Failed;
            break;
        }
        return result;
    }
};

typedef BasicInputPacketStream<char>      InputPacketStream;
typedef BasicInputPacketStream<wchar_t>   InputPacketStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_INPUTPACKETSTREAM_H
