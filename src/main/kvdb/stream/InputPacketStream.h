
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
#include "kvdb/core/MessageHeader.h"
#include "kvdb/stream/InputStream.h"

namespace kvdb {

template <typename T, typename Base = BasicStream<T>>
class BasicInputPacketStream : public BasicInputStream<T, Base> {
public:
    typedef BasicInputStream<T, Base>           base_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicInputPacketStream() : base_type() {}
    BasicInputPacketStream(const char_type * data, size_type size) : base_type(data, size) {}
    template <size_t N>
    BasicInputPacketStream(const char_type(&data)[N]) : base_type(data, N) {}
    ~BasicInputPacketStream() {}

    int readHeader(MessageHeader & header) {
        header.sign = base_type::readUInt32();
        header.type = base_type::readUInt32();
        header.args = base_type::readUInt32();
        header.length = base_type::readUInt32();
        return ReadResult::Ok;
    }

    uint8_t readType() {
        return base_type::readUInt8();
    }

    bool readByte(uint8_t & value) {
        uint8_t type = base_type::readType();
        value = base_type::readByte();
        return (type == DataType::Int8 || type == DataType::UInt8);
    }

    bool readChar(char_type & value) {
        uint8_t type = base_type::readType();
        bool correct = ((typeid(char_type) == typeid(int8_t)   && type == DataType::Int8)
                     || (typeid(char_type) == typeid(uint8_t)  && type == DataType::UInt8)
                     || (typeid(char_type) == typeid(int16_t)  && type == DataType::Int16)
                     || (typeid(char_type) == typeid(uint16_t) && type == DataType::UInt16));
        value = base_type::readChar();
        return correct;
    }

    bool readBool(bool & value) {
        uint8_t type = base_type::readType();
        value = base_type::readBool();
        return (type == DataType::Bool);
    }

    bool readInt8(int8_t & value) {
        uint8_t type = base_type::readType();
        value = base_type::readInt8();
        return (type == DataType::Int8);
    }

    bool readInt16(int16_t & value) {
        uint8_t type = base_type::readType();
        value = base_type::readInt16();
        return (type == DataType::Int16);
    }

    bool readInt32(int32_t & value) {
        uint8_t type = base_type::readType();
        value = base_type::readInt32();
        return (type == DataType::Int32);
    }

    bool readInt64(int64_t & value) {
        uint8_t type = base_type::readType();
        value = base_type::readInt64();
        return (type == DataType::Int64);
    }

    bool readUInt8(uint8_t & value) {
        uint8_t type = base_type::readType();
        value = base_type::readUInt8();
        return (type == DataType::UInt8);
    }

    bool readUInt16(uint16_t & value) {
        uint8_t type = base_type::readType();
        value = base_type::readUInt16();
        return (type == DataType::UInt16);
    }

    bool readUInt32(uint32_t & value) {
        uint8_t type = base_type::readType();
        value = base_type::readUInt32();
        return (type == DataType::UInt32);
    }

    bool readUInt64(uint64_t & value) {
        uint8_t type = base_type::readType();
        value = base_type::readUInt64();
        return (type == DataType::UInt64);
    }

    bool readPointer(void * & value) {
        uint8_t type = base_type::readType();
        value = base_type::readPointer();
        return (type == DataType::Pointer);
    }

    bool readFloat(float & value) {
        uint8_t type = base_type::readType();
        value = base_type::readFloat();
        return (type == DataType::Float);
    }

    bool readDouble(double & value) {
        uint8_t type = base_type::readType();
        value = base_type::readDouble();
        return (type == DataType::Double);
    }

    int readValue(Variant & variant) {
        int result = ReadResult::Ok;
        uint8_t type = base_type::readType();
        variant.type = type;
        switch (type) {
        case DataType::Bool:
            variant.value.b = base_type::readBool();
            break;
        case DataType::Int8:
            variant.value.i8 = base_type::readInt8();
            break;
        case DataType::Int16:
            variant.value.i16 = base_type::readInt16();
            break;
        case DataType::Int32:
            variant.value.i32 = base_type::readInt32();
            break;
        case DataType::Int64:
            variant.value.i64 = base_type::readInt64();
            break;
        case DataType::UInt8:
            variant.value.u8 = base_type::readUInt8();
            break;
        case DataType::UInt16:
            variant.value.u16 = base_type::readUInt16();
            break;
        case DataType::UInt32:
            variant.value.u32 = base_type::readUInt32();
            break;
        case DataType::UInt64:
            variant.value.u64 = base_type::readUInt64();
            break;
        case DataType::Pointer:
            variant.value.ptr = base_type::readPointer();
            break;
        case DataType::Float:
            variant.value.f = base_type::readFloat();
            break;
        case DataType::Double:
            variant.value.d = base_type::readDouble();
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
        uint8_t type = base_type::getType();
        switch (type) {
        case DataType::String:
            base_type::nextUInt8();
            length = base_type::readUInt32();
            base_type::readString(value, length);
            break;

        case DataType::String1B:
            base_type::nextUInt8();
            length = base_type::readUInt8();
            base_type::readString(value, length);
            break;

        case DataType::String2B:
            base_type::nextUInt8();
            length = base_type::readUInt16();
            base_type::readString(value, length);
            break;

        case DataType::String3B:
        {
            uint32_t u32 = base_type::readUInt32();
#if IS_BIG_ENDIAN
            length = (u32 & 0xFFFFFF00UL) >> 8;
#else
            length = u32 & 0x00FFFFFFUL;
#endif
            base_type::readString(value, length);
            break;
        }

        default:
            result = ReadResult::Failed;
            break;
        }
        return result;
    }
};

typedef BasicInputPacketStream<char, BasicStream<char>>         InputPacketStream;
typedef BasicInputPacketStream<wchar_t, BasicStream<wchar_t>>   InputPacketStreamW;

typedef BasicInputPacketStream<char, BasicPrepareStream<char>>         PrepareInputPacketStream;
typedef BasicInputPacketStream<wchar_t, BasicPrepareStream<wchar_t>>   PrepareInputPacketStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_INPUTPACKETSTREAM_H
