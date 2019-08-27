
#ifndef KVDB_STREAM_BASICSTREAM_H
#define KVDB_STREAM_BASICSTREAM_H

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
#include "kvdb/jstd/StringRef.h"
#include "kvdb/stream/Packet.h"

namespace kvdb {

struct ReadResult {
    enum Type {
        Failed = -1,
        Ok = 0,
        Last
    };
};

struct WriteResult {
    enum Type {
        Failed = -1,
        Ok = 0,
        Last
    };
};

template <typename T>
class BasicStream {
public:
    typedef typename std::remove_pointer<
                typename std::remove_cv<T>::type
            >::type     char_type;

    typedef std::basic_string<char_type>    string_type;
    typedef jstd::BasicStringRef<char_type> stringref_type;

protected:
    char_type * cur_;
    char_type * head_;

public:
    BasicStream() : cur_(nullptr), head_(nullptr) {}
    BasicStream(const char_type * ptr) : cur_(const_cast<char_type *>(ptr)), head_(const_cast<char_type *>(ptr)) {}
    template <size_t N>
    BasicStream(const char_type(&data)[N]) : cur_(data), head_(data) {}
    ~BasicStream() {}

    void setStream(const char_type * ptr) {
        cur_ = (char_type *)ptr;
        head_ = (char_type *)ptr;
    }

    template <size_t N>
    void setStream(const char_type(&data)[N]) {
        cur_ = (char_type *)data;
        head_ = (char_type *)data;
    }

    char_type * head() const {
        return head_;
    }

    char_type * current() const {
        return cur_;
    }

    ptrdiff_t length() const   { return (cur_ - head_); }
    ptrdiff_t position() const { return this->length(); }

    uint32_t getMsgLength() const {
        return (uint32_t)(this->length() - sizeof(PacketHeader));
    }

    bool isOverflow()  const { return false; }
    bool isUnderflow() const { return (cur_ < head_); }

    void reset() {
        cur_ = head_;
    }

    void setHead(char_type * head) { this->head_ = head; }
    void setCurrent(char_type * cur) { this->cur_ = cur; }

    void writeHeader(const PacketHeader & header) {
        stream.writeUInt32(header.signId);
        stream.writeUInt32(header.msgType);
        stream.writeUInt32(header.msgLength);
        stream.writeUInt32(header.varCount);
    }

    void writeHeader(uint32_t signId, uint32_t msgType, uint32_t msgLength, uint32_t varCount) {
        stream.writeUInt32(signId);
        stream.writeUInt32(msgType);
        stream.writeUInt32(msgLength);
        stream.writeUInt32(varCount);
    }

    void writeHeaderAndRestore(const PacketHeader & header) {
        char_type * saveCur = this->current();
        reset();
        this->writeUInt32(header.signId);
        this->writeUInt32(header.msgType);
        this->writeUInt32(header.msgLength);
        this->writeUInt32(header.varCount);
        this->setCurrent(saveCur);
    }

    void writeHeaderAndRestore(uint32_t signId, uint32_t msgType, uint32_t varCount) {
        char_type * saveCur = this->current();
        uint32_t msgLength = getMsgLength();
        reset();
        writeUInt32(signId);
        writeUInt32(msgType);
        writeUInt32(msgLength);
        writeUInt32(varCount);
        setCurrent(saveCur);
    }

    void writeHeaderAndRestore(uint32_t signId, uint32_t msgType, uint32_t msgLength, uint32_t varCount) {
        char_type * saveCur = this->current();
        reset();
        writeUInt32(signId);
        writeUInt32(msgType);
        writeUInt32(msgLength);
        writeUInt32(varCount);
        setCurrent(saveCur);
    }

    void back() {
        cur_ = (char_type *)((char *)cur_ - sizeof(char));
    }

    void back(int skip) {
        cur_ = (char_type *)((char *)cur_ - skip * sizeof(char));
    }

    void backByte() {
        back();
    }

    void backByte(int skip) {
        back(skip);
    }

    void backChar() {
        cur_ = (char_type *)((char *)cur_ - sizeof(char_type));
    }

    void backChar(int skip) {
        cur_ = (char_type *)((char *)cur_ - skip * sizeof(char_type));
    }

    void next() {
        cur_ = (char_type *)((char *)cur_ + sizeof(char));
    }

    void next(int skip) {
        cur_ = (char_type *)((char *)cur_ + skip);
    }

    void nextType() {
        next();
    }

    void nextByte() {
        next();
    }

    void nextByte(int skip) {
        next(skip);
    }

    void nextChar() {
        cur_ = (char_type *)((char *)cur_ + sizeof(char_type));
    }

    void nextChar(int skip) {
        cur_ = (char_type *)((char *)cur_ + skip * sizeof(char_type));
    }

    void nextBool() {
        cur_ = (char_type *)((char *)cur_ + sizeof(bool));
    }

    void nextInt8() {
        cur_ = (char_type *)((char *)cur_ + sizeof(int8_t));
    }

    void nextInt16() {
        cur_ = (char_type *)((char *)cur_ + sizeof(int16_t));
    }

    void nextInt32() {
        cur_ = (char_type *)((char *)cur_ + sizeof(int32_t));
    }

    void nextInt64() {
        cur_ = (char_type *)((char *)cur_ + sizeof(int64_t));
    }

    void nextUInt8() {
        cur_ = (char_type *)((char *)cur_ + sizeof(uint8_t));
    }

    void nextUInt16() {
        cur_ = (char_type *)((char *)cur_ + sizeof(uint16_t));
    }

    void nextUInt32() {
        cur_ = (char_type *)((char *)cur_ + sizeof(uint32_t));
    }

    void nextUInt64() {
        cur_ = (char_type *)((char *)cur_ + sizeof(uint64_t));
    }

    void nextPointer() {
        cur_ = (char_type *)((char *)cur_ + sizeof(void *));
    }

    void nextFloat() {
        cur_ = (char_type *)((char *)cur_ + sizeof(float));
    }

    void nextDouble() {
        cur_ = (char_type *)((char *)cur_ + sizeof(double));
    }

    void skip(int offset) {
        next(offset);
    }

    void skipToHeader() {
        next(sizeof(PacketHeader));
    }

    char get() const {
        return getInt8();
    }

    unsigned char getu() const {
        return getUInt8();
    }

    uint8_t getType() const {
        return getUInt8();
    }

    uint8_t getByte() const {
        return getUInt8();
    }

    char_type getChar() const {
        return *(char_type *)cur_;
    }

    bool getBool() const {
        return *(bool *)cur_;
    }

    int8_t getInt8() const {
        return *(int8_t *)cur_;
    }

    int16_t getInt16() const {
        return *(int16_t *)cur_;
    }

    int32_t getInt32() const {
        return *(int32_t *)cur_;
    }

    int64_t getInt64() const {
        return *(int64_t *)cur_;
    }

    uint8_t getUInt8() const {
        return *(uint8_t *)cur_;
    }

    uint16_t getUInt16() const {
        return *(uint16_t *)cur_;
    }

    uint32_t getUInt32() const {
        return *(uint32_t *)cur_;
    }

    uint64_t getUInt64() const {
        return *(uint64_t *)cur_;
    }

    void * getPointer() const {
        return *(void **)cur_;
    }

    float getFloat() const {
        return *(float *)cur_;
    }

    double getDouble() const {
        return *(double *)cur_;
    }

    template <typename StringType>
    void getString(StringType & value, size_t length) {
        value.assign((const char_type *)cur_, length);
    }

    void setByte(uint8_t value) const {
        setUInt8(value);
    }

    void setChar(char_type value) const {
        *(char_type *)cur_ = value;
    }

    void setBool(bool value) {
        *(bool *)cur_ = value;
    }

    void setInt8(int8_t value) {
        *(int8_t *)cur_ = value;
    }

    void setInt16(int16_t value) {
        *(int16_t *)cur_ = value;
    }

    void setInt32(int32_t value) {
        *(int32_t *)cur_ = value;
    }

    void setInt64(int64_t value) {
        *(int64_t *)cur_ = value;
    }

    void setUInt8(uint8_t value) {
        *(uint8_t *)cur_ = value;
    }

    void setUInt16(uint16_t value) {
        *(uint16_t *)cur_ = value;
    }

    void setUInt32(uint32_t value) {
        *(uint32_t *)cur_ = value;
    }

    void setUInt64(uint64_t value) {
        *(uint64_t *)cur_ = value;
    }

    void setPointer(void * value) {
        *(void **)cur_ = value;
    }

    void setFloat(float value) {
        *(float *)cur_ = value;
    }

    void setDouble(double value) {
        *(double *)cur_ = value;
    }

    template <typename StringType>
    void setString(const StringType & value) {
        ::memcpy((void *)cur_, (const void *)value.c_str(), value.size() * sizeof(char_type));
    }
};

typedef BasicStream<char>      Stream;
typedef BasicStream<wchar_t>   StreamW;

} // namespace kvdb

#endif // KVDB_STREAM_BASICSTREAM_H
