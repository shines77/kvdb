
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
#include "kvdb/stream/BasicPrepareStream.h"

namespace kvdb {

template <typename T, typename Base = BasicStream<T>>
class BasicOutputStream : public Base {
public:
    typedef Base                                base_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicOutputStream() : base_type() {}
    BasicOutputStream(const char_type * data, size_type size) : base_type(data, size) {}
    template <size_t N>
    BasicOutputStream(const char_type(&data)[N]) : base_type(data, N) {}
    ~BasicOutputStream() {}

    bool isMemoryStream() const { return true; }

    void writeHeader(const MessageHeader & header) {
        this->writeUInt32(header.sign);
        this->writeUInt32(header.type);
        this->writeUInt32(header.info);
        this->writeUInt32(header.length);
    }

    void writeHeader(uint32_t sign, uint32_t type, uint32_t info, uint32_t length) {
        this->writeUInt32(sign);
        this->writeUInt32(type);
        this->writeUInt32(info);
        this->writeUInt32(length);
    }

    void writeHeaderAndRestore(const MessageHeader & header) {
        char_type * savePos = this->current();
        this->reset();
        this->writeUInt32(header.sign);
        this->writeUInt32(header.type);
        this->writeUInt32(header.info);
        this->writeUInt32(header.length);
        this->setCurrent(savePos);
    }

    void writeHeaderAndRestore(uint32_t sign, uint32_t type, uint32_t info) {
        char_type * savePos = this->current();
        uint32_t length = this->getMsgLength();
        this->reset();
        this->writeUInt32(sign);
        this->writeUInt32(type);
        this->writeUInt32(info);
        this->writeUInt32(length);
        this->setCurrent(savePos);
    }

    void writeHeaderAndRestore(uint32_t sign, uint32_t type, uint32_t info, uint32_t length) {
        char_type * savePos = this->current();
        this->reset();
        this->writeUInt32(sign);
        this->writeUInt32(type);
        this->writeUInt32(info);
        this->writeUInt32(length);
        this->setCurrent(savePos);
    }

    void writeType(uint8_t type) {
        this->writeUInt8(type);
    }

    void writeType32(uint32_t type) {
        this->writeUInt8((uint8_t)type);
    }

    void writeByte(uint8_t value) {
        base_type::setByte(value);
        base_type::nextByte();
    }

    void writeChar(char_type value) {
        base_type::setChar(value);
        base_type::nextChar();
    }

    void writeBool(bool value) {
        base_type::setBool(value);
        base_type::nextBool();
    }

    void writeInt8(int8_t value) {
        base_type::setInt8(value);
        base_type::nextInt8();
    }

    void writeInt16(int16_t value) {
        base_type::setInt16(value);
        base_type::nextInt16();
    }

    void writeInt32(int32_t value) {
        base_type::setInt32(value);
        base_type::nextInt32();
    }

    void writeInt64(int64_t value) {
        base_type::setInt64(value);
        base_type::nextInt64();
    }

    void writeUInt8(uint8_t value) {
        base_type::setUInt8(value);
        base_type::nextUInt8();
    }

    void writeUInt16(uint16_t value) {
        base_type::setUInt16(value);
        base_type::nextUInt16();
    }

    void writeUInt32(uint32_t value) {
        base_type::setUInt32(value);
        base_type::nextUInt32();
    }

    void writeUInt64(uint64_t value) {
        base_type::setUInt64(value);
        base_type::nextUInt64();
    }

    void writePointer(void * value) {
        base_type::setPointer(value);
        base_type::nextPointer();
    }

    void writeFloat(float value) {
        base_type::setFloat(value);
        base_type::nextFloat();
    }

    void writeDouble(double value) {
        base_type::setDouble(value);
        base_type::nextDouble();
    }

    template <typename StringType>
    void writeString(const StringType & value) {
        base_type::setString(value);
        base_type::nextChar((int)value.size());
    }
};

typedef BasicOutputStream<char, BasicStream<char>>          OutputStream;
typedef BasicOutputStream<wchar_t, BasicStream<wchar_t>>    OutputStreamW;

typedef BasicOutputStream<char, BasicPrepareStream<char>>          PrepareOutputStream;
typedef BasicOutputStream<wchar_t, BasicPrepareStream<wchar_t>>    PrepareOutputStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_OUTPUTSTREAM_H
