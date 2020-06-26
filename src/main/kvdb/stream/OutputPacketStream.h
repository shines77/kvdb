
#ifndef KVDB_STREAM_OUTPUTPACKETSTREAM_H
#define KVDB_STREAM_OUTPUTPACKETSTREAM_H

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
#include "kvdb/stream/OutputStream.h"

namespace kvdb {

template <typename T, typename Base = BasicStream<T>>
class BasicOutputPacketStream : public BasicOutputStream<T, Base> {
public:
    typedef BasicOutputStream<T, Base>          base_type;
    typedef BasicOutputPacketStream<T, Base>    this_type;
    typedef base_type                           downcast_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicOutputPacketStream() : base_type() {}
    BasicOutputPacketStream(const char_type * data, size_type size) : base_type(data, size) {}
    template <size_type N>
    BasicOutputPacketStream(const char_type(&data)[N]) : base_type(data, N) {}
    ~BasicOutputPacketStream() {}

    bool isMemoryStream() const { return true; }

    downcast_type & downcastTo() {
        return (*static_cast<downcast_type *>(this));
    }

    const downcast_type & downcastTo() const {
        return (*const_cast<const downcast_type *>(static_cast<downcast_type *>(this)));
    }

    void writeType(uint8_t type) {
        base_type::writeType(type);
    }

    void writeType(uint32_t type) {
        base_type::writeType((uint8_t)type);
    }

    void writeByte(uint8_t value) {
        base_type::writeType(DataType::UInt8);
        base_type::writeByte(value);
    }

    void writeChar(char_type value) {
        base_type::writeType(DataType::UInt8);
        base_type::writeByte(value);
    }

    void writeBool(bool value) {
        base_type::writeType(DataType::Bool);
        base_type::writeBool(value);
    }

    void writeInt8(int8_t value) {
        base_type::writeType(DataType::Int8);
        base_type::writeInt8(value);
    }

    void writeInt16(int16_t value) {
        base_type::writeType(DataType::Int16);
        base_type::writeInt16(value);
    }

    void writeInt32(int32_t value) {
        base_type::writeType(DataType::Int32);
        base_type::writeInt32(value);
    }

    void writeInt64(int64_t value) {
        base_type::writeType(DataType::Int64);
        base_type::writeInt64(value);
    }

    void writeUInt8(uint8_t value) {
        base_type::writeType(DataType::UInt8);
        base_type::writeUInt8(value);
    }

    void writeUInt16(uint16_t value) {
        base_type::writeType(DataType::UInt16);
        base_type::writeUInt16(value);
    }

    void writeUInt32(uint32_t value) {
        base_type::writeType(DataType::UInt32);
        base_type::writeUInt32(value);
    }

    void writeUInt64(uint64_t value) {
        base_type::writeType(DataType::UInt64);
        base_type::writeUInt64(value);
    }

    void writePointer(void * value) {
        base_type::writeType(DataType::Pointer);
        base_type::writePointer(value);
    }

    void writeFloat(float value) {
        base_type::writeType(DataType::Float);
        base_type::writeFloat(value);
    }

    void writeDouble(double value) {
        base_type::writeType(DataType::Double);
        base_type::writeDouble(value);
    }

    template <typename StringType>
    void writeString(const StringType & value) {
        int result = WriteResult::Ok;
        size_t length = value.size();
        if (length < 256) {
            base_type::writeType(DataType::String1B);
            base_type::writeUInt8((uint8_t)length);
            base_type::writeString_Internal(value);
        }
        else if (length < 65536) {
            base_type::writeType(DataType::String2B);
            base_type::writeUInt16((uint16_t)length);
            base_type::writeString_Internal(value);
        }
        else if (length < 16777216) {
#if IS_BIG_ENDIAN
            uint32_t value32 = (DataType::String3B & 0x000000FFUL) | (length & 0xFFFFFF00UL);
            base_type::writeUInt32(value32);
#else
            uint32_t value32 = (DataType::String3B << 24) | (length & 0x00FFFFFFUL);
            base_type::writeUInt32(value32);
#endif
            base_type::writeString_Internal(value);
        }
        else {
            base_type::writeType(DataType::String);
            base_type::writeUInt32((uint32_t)length);
            base_type::writeString_Internal(value);
        }
    }

    int writeValue(Variant & variant) {
        int result = WriteResult::Ok;
        uint8_t type = variant.getType();
        switch (type) {
        case DataType::Bool:
            base_type::writeType(type);
            base_type::writeBool(variant.value.b);
            break;
        case DataType::Int8:
            base_type::writeType(type);
            base_type::writeInt8(variant.value.i8);
            break;
        case DataType::Int16:
            base_type::writeType(type);
            base_type::writeInt16(variant.value.i16);
            break;
        case DataType::Int32:
            base_type::writeType(type);
            base_type::writeInt32(variant.value.i32);
            break;
        case DataType::Int64:
            base_type::writeType(type);
            base_type::writeInt64(variant.value.i64);
            break;
        case DataType::UInt8:
            base_type::writeType(type);
            base_type::writeUInt8(variant.value.u8);
            break;
        case DataType::UInt16:
            base_type::writeType(type);
            base_type::writeUInt16(variant.value.u16);
            break;
        case DataType::UInt32:
            base_type::writeType(type);
            base_type::writeUInt32(variant.value.u32);
            break;
        case DataType::UInt64:
            base_type::writeType(type);
            base_type::writeUInt64(variant.value.u64);
            break;
        case DataType::Pointer:
            base_type::writeType(type);
            base_type::writePointer(variant.value.ptr);
            break;
        case DataType::Float:
            base_type::writeType(type);
            base_type::writeFloat(variant.value.f);
            break;
        case DataType::Double:
            base_type::writeType(type);
            base_type::writeDouble(variant.value.d);
            break;
        default:
            result = WriteResult::Failed;
        }
        return result;
    }
};

typedef BasicOutputPacketStream<char, BasicStream<char>>         OutputPacketStream;
typedef BasicOutputPacketStream<wchar_t, BasicStream<wchar_t>>   OutputPacketStreamW;

typedef BasicOutputPacketStream<char, BasicPrepareStream<char>>         PrepareOutputPacketStream;
typedef BasicOutputPacketStream<wchar_t, BasicPrepareStream<wchar_t>>   PrepareOutputPacketStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_OUTPUTPACKETSTREAM_H
