
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

template <typename T, typename VauleType = T>
class BasicInputStream : public BasicStream<T, VauleType> {
public:
    typedef BasicStream<T, VauleType>           base_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::value_type      value_type;

    typedef std::basic_string<char_type>        string_type;
    typedef jstd::BasicStringRef<char_type>     stringref_type;

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

    value_type readChar() {
        value_type value = getChar();
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

    void readString(string_type & value, size_t length) {
        value.assign((const char_type *)cur_, length);
    }

    void readString(stringref_type & value, size_t length) {
        value.assign((const char_type *)cur_, length);
    }

    int parseString(string_type & value) {
        int result = ParseResult::OK;
        uint32_t length;

        uint8_t data_type = readUInt8();
        switch (data_type) {
        case DataType::String:
            length = readUInt32();
            readString(value, length);
            break;

        case DataType::String1B:
            length = readUInt8();
            readString(value, length);

        case DataType::String2B:
            length = readUInt16();
            readString(value, length);
            break;

        case DataType::String3B:
            length = getUInt32() & 0x00FFFFFFU;
            next(3);
            readString(value, length);
            break;

        default:
            back();
            result = ParseResult::Failed;
            break;
        }

        return result;
    }

    int parseString(stringref_type & value) {
        int result = ParseResult::OK;
        uint32_t length;

        uint8_t data_type = readUInt8();
        switch (data_type) {
        case DataType::String:
            length = readUInt32();
            readString(value, length);
            break;

        case DataType::String1B:
            length = readUInt8();
            readString(value, length);

        case DataType::String2B:
            length = readUInt16();
            readString(value, length);
            break;

        case DataType::String3B:
            length = getUInt32() & 0x00FFFFFFU;
            next(3);
            readString(value, length);
            break;

        default:
            back();
            result = ParseResult::Failed;
            break;
        }

        return result;
    }
};

typedef BasicInputStream<char>      InputStream;
typedef BasicInputStream<wchar_t>   InputStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_INPUTSTREAM_H
