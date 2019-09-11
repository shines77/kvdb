
#ifndef KVDB_STREAM_OUTPUTSTREAM_H
#define KVDB_STREAM_OUTPUTSTREAM_H

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

#include "kvdb/core/Message.h"
#include "kvdb/stream/BasicStream.h"

namespace kvdb {

template <typename T>
class BasicOutputStream : public BasicStream<T> {
public:
    typedef BasicStream<T>                      base_type;
    typedef typename base_type::char_type       char_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicOutputStream() : base_type() {}
    BasicOutputStream(const char_type * value) : base_type(value) {}
    template <size_t N>
    BasicOutputStream(const char_type(&data)[N]) : base_type(data) {}
    ~BasicOutputStream() {}

    void writeHeader(const MessageHeader & header) {
        this->writeUInt32(header.sign);
        this->writeUInt32(header.type);
        this->writeUInt32(header.args);
        this->writeUInt32(header.length);
    }

    void writeHeader(uint32_t sign, uint32_t type, uint32_t args, uint32_t length) {
        this->writeUInt32(sign);
        this->writeUInt32(type);
        this->writeUInt32(args);
        this->writeUInt32(length);
    }

    void writeHeaderAndRestore(const MessageHeader & header) {
        char_type * savePos = this->current();
        this->reset();
        this->writeUInt32(header.sign);
        this->writeUInt32(header.type);
        this->writeUInt32(header.args);
        this->writeUInt32(header.length);
        this->setCurrent(savePos);
    }

    void writeHeaderAndRestore(uint32_t sign, uint32_t type, uint32_t args) {
        char_type * savePos = this->current();
        uint32_t length = this->getMsgLength();
        this->reset();
        this->writeUInt32(sign);
        this->writeUInt32(type);
        this->writeUInt32(args);
        this->writeUInt32(length);
        this->setCurrent(savePos);
    }

    void writeHeaderAndRestore(uint32_t sign, uint32_t type, uint32_t args, uint32_t length) {
        char_type * savePos = this->current();
        this->reset();
        this->writeUInt32(sign);
        this->writeUInt32(type);
        this->writeUInt32(args);
        this->writeUInt32(length);
        this->setCurrent(savePos);
    }

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

typedef BasicOutputStream<char>      OutputStream;
typedef BasicOutputStream<wchar_t>   OutputStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_OUTPUTSTREAM_H
