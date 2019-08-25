
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

#include "kvdb/stream/BasicStream.h"

namespace kvdb {

template <typename T>
class BasicInputStream : public BasicStream<T> {
public:
    typedef BasicStream<T>                  base_type;
    typedef typename base_type::char_type   char_type;

    typedef std::basic_string<char_type>    string_type;
    typedef jstd::BasicStringRef<char_type> stringref_type;

    BasicInputStream() : base_type(nullptr) {}
    BasicInputStream(const char_type * value) : base_type(value) {}
    template <size_t N>
    BasicInputStream(const char_type(&data)[N]) : base_type(data) {}
    ~BasicInputStream() {}

    uint8_t readType() {
        return readUInt8();
    }

    uint8_t readByte() {
        uint8_t value = getByte();
        nextByte();
        return value;
    }

    char_type readChar() {
        char_type value = getChar();
        nextChar();
        return value;
    }

    bool readBool() {
        bool value = getBool();
        nextBool();
        return value;
    }

    int8_t readInt8() {
        int8_t value = getInt8();
        nextInt8();
        return value;
    }

    int16_t readInt16() {
        int16_t value = getInt16();
        nextInt16();
        return value;
    }

    int32_t readInt32() {
        int32_t value = getInt32();
        nextInt32();
        return value;
    }

    int64_t readInt64() {
        int64_t value = getInt64();
        nextInt64();
        return value;
    }

    uint8_t readUInt8() {
        uint8_t value = getUInt8();
        nextUInt8();
        return value;
    }

    uint16_t readUInt16() {
        uint16_t value = getUInt16();
        nextUInt16();
        return value;
    }

    uint32_t readUInt32() {
        uint32_t value = getUInt32();
        nextUInt32();
        return value;
    }

    uint64_t readUInt64() {
        uint64_t value = getUInt64();
        nextUInt64();
        return value;
    }

    void * readPointer() {
        void * value = getPointer();
        nextPointer();
        return value;
    }

    float readFloat() {
        float value = getFloat();
        nextFloat();
        return value;
    }

    double readDouble() {
        double value = getDouble();
        nextDouble();
        return value;
    }

    template <typename StringType>
    void readString(StringType & value, size_t length) {
        getString(value, length);
        nextChar((int)length);
    }

    template <typename StringType>
    int parseString(StringType & value) {
        int result = ReadResult::Ok;
        uint32_t length;

        uint8_t type = getType();
        switch (type) {
        case DataType::String:
            nextUInt8();
            length = readUInt32();
            readString(value, length);
            break;

        case DataType::String1B:
            nextUInt8();
            length = readUInt8();
            readString(value, length);
            break;

        case DataType::String2B:
            nextUInt8();
            length = readUInt16();
            readString(value, length);
            break;

        case DataType::String3B:
        {
            uint32_t value32 = readUInt32();
#if NOT_IS_LITTLE_ENDIAN
            length = (value32 & 0xFFFFFF00UL) >> 8;
#else
            length = value32 & 0x00FFFFFFUL;
#endif
            readString(value, length);
            break;
        }

        default:
            result = ReadResult::Failed;
            break;
        }

        return result;
    }
};

typedef BasicInputStream<char>      InputStream;
typedef BasicInputStream<wchar_t>   InputStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_INPUTSTREAM_H
