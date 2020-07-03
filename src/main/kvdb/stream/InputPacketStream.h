
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
#include "kvdb/stream/InputStream.h"

namespace kvdb {

template < typename StorageTy, typename Base = BasicStream<StorageTy> >
class BasicInputPacketStream : public BasicInputStream<StorageTy, Base> {
public:
    typedef BasicInputStream<StorageTy, Base>       base_type;
    typedef BasicInputPacketStream<StorageTy, Base> this_type;
    typedef base_type                               downcast_type;

    typedef typename base_type::char_type           char_type;
    typedef typename base_type::size_type           size_type;
    typedef typename base_type::storage_type        storage_type;
    typedef typename base_type::allocator_type      allocator_type;

    typedef typename base_type::string_type         string_type;
    typedef typename base_type::stringref_type      stringref_type;

public:
    BasicInputPacketStream(const storage_type & storage) : base_type(storage) {}
    BasicInputPacketStream(storage_type && storage) : base_type(std::forward<storage_type>(storage)) {}
    BasicInputPacketStream(const char_type * data, size_type size) : base_type(data, size) {}
    template <size_type N>
    BasicInputPacketStream(const char_type(&data)[N])
        : base_type(data, N) {
    }
    ~BasicInputPacketStream() {}

    base_type & downcastTo() {
        return (*static_cast<base_type *>(this));
    }

    const base_type & downcastTo() const {
        return (*const_cast<const base_type *>(static_cast<base_type *>(this)));
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
        uint8_t type = base_type::readType();
        switch (type) {
        case DataType::String:
            length = base_type::readUInt32();
            base_type::iternalReadString(value, length);
            break;

        case DataType::String1B:
            length = base_type::readUInt8();
            base_type::iternalWriteString(value, length);
            break;

        case DataType::String2B:
            length = base_type::readUInt16();
            base_type::iternalWriteString(value, length);
            break;

        case DataType::String3B:
        {
            uint32_t u32 = base_type::readUInt32();
#if IS_BIG_ENDIAN
            length = (u32 & 0xFFFFFF00UL) >> 8;
#else
            length = u32 & 0x00FFFFFFUL;
#endif
            base_type::iternalWriteString(value, length);
            break;
        }

        default:
            result = ReadResult::Failed;
            break;
        }
        return result;
    }
};

typedef BasicInputPacketStream< BasicByteBuffer<char>, BasicStream<BasicByteBuffer<char>> >         InputPacketStream;
typedef BasicInputPacketStream< BasicByteBuffer<wchar_t>, BasicStream<BasicByteBuffer<wchar_t>> >   InputPacketStreamW;

typedef BasicInputPacketStream< BasicByteBuffer<char>, BasicPrepareStream<BasicByteBuffer<char>> >         PrepareInputPacketStream;
typedef BasicInputPacketStream< BasicByteBuffer<wchar_t>, BasicPrepareStream<BasicByteBuffer<wchar_t>> >   PrepareInputPacketStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_INPUTPACKETSTREAM_H
