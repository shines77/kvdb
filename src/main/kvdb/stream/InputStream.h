
#ifndef KVDB_STREAM_INPUTSTREAM_H
#define KVDB_STREAM_INPUTSTREAM_H

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

#include "kvdb/core/DataType.h"
#include "kvdb/stream/BasicStream.h"
#include "kvdb/stream/BasicPrepareStream.h"

namespace kvdb {

template <typename T, typename Base = BasicStream<T>>
class BasicInputStream : public Base {
public:
    typedef Base                                base_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicInputStream() : base_type(nullptr) {}
    BasicInputStream(const char_type * data, size_type size) : base_type(data, size) {}
    template <size_type N>
    BasicInputStream(const char_type(&data)[N]) : base_type(data, N) {}
    ~BasicInputStream() {}

    bool isMemoryStream() const { return true; }

protected:
    template <typename StringType>
    void readString_Internal(StringType & value, size_t length) {
        base_type::getString(value, length);
        base_type::nextChar((int)length);
    }

public:
    uint8_t readType() {
        return readUInt8();
    }

    uint8_t readByte() {
        uint8_t value = base_type::getByte();
        base_type::nextByte();
        return value;
    }

    char_type readChar() {
        char_type value = base_type::getChar();
        base_type::nextChar();
        return value;
    }

    bool readBool() {
        bool value = base_type::getBool();
        base_type::nextBool();
        return value;
    }

    int8_t readInt8() {
        int8_t value = base_type::getInt8();
        base_type::nextInt8();
        return value;
    }

    int16_t readInt16() {
        int16_t value = base_type::getInt16();
        base_type::nextInt16();
        return value;
    }

    int32_t readInt32() {
        int32_t value = base_type::getInt32();
        base_type::nextInt32();
        return value;
    }

    int64_t readInt64() {
        int64_t value = base_type::getInt64();
        base_type::nextInt64();
        return value;
    }

    uint8_t readUInt8() {
        uint8_t value = base_type::getUInt8();
        base_type::nextUInt8();
        return value;
    }

    uint16_t readUInt16() {
        uint16_t value = base_type::getUInt16();
        base_type::nextUInt16();
        return value;
    }

    uint32_t readUInt32() {
        uint32_t value = base_type::getUInt32();
        base_type::nextUInt32();
        return value;
    }

    uint64_t readUInt64() {
        uint64_t value = base_type::getUInt64();
        base_type::nextUInt64();
        return value;
    }

    void * readPointer() {
        void * value = base_type::getPointer();
        base_type::nextPointer();
        return value;
    }

    float readFloat() {
        float value = base_type::getFloat();
        base_type::nextFloat();
        return value;
    }

    double readDouble() {
        double value = base_type::getDouble();
        base_type::nextDouble();
        return value;
    }

    template <typename StringType>
    int readString(StringType & value) {
        uint32_t length = this->readUInt32();
        this->readString_Internal(value, length);
        return ReadResult::Ok;
    }

    template <typename StringType>
    int parseString(StringType & value) {
        int result = ReadResult::Ok;
        uint32_t length;

        uint8_t type = this->readType();
        switch (type) {
        case DataType::String:
            length = this->readUInt32();
            this->readString_Internal(value, length);
            break;

        case DataType::String1B:
            length = base_type::readUInt8();
            this->readString_Internal(value, length);
            break;

        case DataType::String2B:
            length = base_type::readUInt16();
            this->readString_Internal(value, length);
            break;

        case DataType::String3B:
        {
            uint32_t value32 = base_type::readUInt32();
#if NOT_IS_LITTLE_ENDIAN
            length = (value32 & 0xFFFFFF00UL) >> 8;
#else
            length = value32 & 0x00FFFFFFUL;
#endif
            this->readString_Internal(value, length);
            break;
        }

        default:
            result = ReadResult::Failed;
            break;
        }

        return result;
    }
};

typedef BasicInputStream<char, BasicStream<char>>       InputStream;
typedef BasicInputStream<wchar_t, BasicStream<wchar_t>> InputStreamW;

typedef BasicInputStream<char, BasicPrepareStream<char>>        PrepareInputStream;
typedef BasicInputStream<wchar_t, BasicPrepareStream<wchar_t>>  PrepareInputStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_INPUTSTREAM_H
