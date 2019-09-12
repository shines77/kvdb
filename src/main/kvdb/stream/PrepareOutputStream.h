
#ifndef KVDB_STREAM_PREPARE_OUTPUTSTREAM_H
#define KVDB_STREAM_PREPARE_OUTPUTSTREAM_H

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

#include "kvdb/stream/BasicPrepareStream.h"

namespace kvdb {

#if 0

template <typename T>
class BasicPrepareOutputStream : public BasicPrepareStream<T> {
public:
    typedef BasicPrepareStream<T>               base_type;
    typedef typename base_type::char_type       char_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicPrepareOutputStream() : base_type() {}
    BasicPrepareOutputStream(const char_type * value) : base_type(value) {}
    template <size_t N>
    BasicPrepareOutputStream(const char_type(&data)[N]) : base_type(data) {}
    ~BasicPrepareOutputStream() {}

    bool isMemoryStream() const { return true; }

    void writeType(uint8_t type) {
        writeUInt8(type);
    }

    void writeType32(uint32_t type) {
        writeUInt8((uint8_t)type);
    }

    void writeByte(uint8_t value) {
        setByte(value);
        nextByte();
    }

    void writeChar(char_type value) {
        setChar(value);
        nextChar();
    }

    void writeBool(bool value) {
        setBool(value);
        nextBool();
    }

    void writeInt8(int8_t value) {
        setInt8(value);
        nextInt8();
    }

    void writeInt16(int16_t value) {
        setInt16(value);
        nextInt16();
    }

    void writeInt32(int32_t value) {
        setInt32(value);
        nextInt16();
    }

    void writeInt64(int64_t value) {
        setInt64(value);
        nextInt64();
    }

    void writeUInt8(uint8_t value) {
        setUInt8(value);
        nextUInt8();
    }

    void writeUInt16(uint16_t value) {
        setUInt16(value);
        nextUInt16();
    }

    void writeUInt32(uint32_t value) {
        setUInt32(value);
        nextUInt32();
    }

    void writeUInt64(uint64_t value) {
        setUInt64(value);
        nextUInt64();
    }

    void writePointer(void * value) {
        setPointer(value);
        nextPointer();
    }

    void writeFloat(float value) {
        setFloat(value);
        nextFloat();
    }

    void writeDouble(double value) {
        setDouble(value);
        nextDouble();
    }

    template <typename StringType>
    void writeString(const StringType & value) {
        setString(value);
        nextChar((int)value.size());
    }
};

typedef BasicPrepareOutputStream<char>      PrepareOutputStream;
typedef BasicPrepareOutputStream<wchar_t>   PrepareOutputStreamW;

#endif

} // namespace kvdb

#endif // KVDB_STREAM_PREPARE_OUTPUTSTREAM_H
