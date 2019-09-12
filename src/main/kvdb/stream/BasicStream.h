
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
#include "kvdb/core/MessageHeader.h"
#include "kvdb/jstd/StringRef.h"

#include "kvdb/stream/StreamBuffer.h"

namespace kvdb {

template <typename T>
class BasicStream : public BasicStreamBuffer<T> {
public:
    typedef BasicStreamBuffer<T>                base_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicStream() : base_type() {}
    BasicStream(const char_type * data, size_type size) : base_type(data, size) {}
    template <size_t N>
    BasicStream(const char_type(&data)[N]) : base_type(data, N) {}
    ~BasicStream() {}

    void back() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(char));
    }

    void back(int skip) {
        this->cur_ = (char_type *)((char *)this->cur_ - skip * sizeof(char));
    }

    void backByte() {
        back();
    }

    void backByte(int skip) {
        back(skip);
    }

    void backChar() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(char_type));
    }

    void backChar(int skip) {
        this->cur_ = (char_type *)((char *)this->cur_ - skip * sizeof(char_type));
    }

    void next() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(char));
    }

    void next(int skip) {
        this->cur_ = (char_type *)((char *)this->cur_ + skip * sizeof(char));
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
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(char_type));
    }

    void nextChar(int skip) {
        this->cur_ = (char_type *)((char *)this->cur_ + skip * sizeof(char_type));
    }

    void nextBool() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(bool));
    }

    void nextInt8() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(int8_t));
    }

    void nextInt16() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(int16_t));
    }

    void nextInt32() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(int32_t));
    }

    void nextInt64() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(int64_t));
    }

    void nextUInt8() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(uint8_t));
    }

    void nextUInt16() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(uint16_t));
    }

    void nextUInt32() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(uint32_t));
    }

    void nextUInt64() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(uint64_t));
    }

    void nextPointer() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(void *));
    }

    void nextFloat() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(float));
    }

    void nextDouble() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(double));
    }

    void skip(int offset) {
        next(offset);
    }

    void skipToHeader() {
        next(sizeof(MessageHeader));
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
        return *(char_type *)this->cur_;
    }

    bool getBool() const {
        return *(bool *)this->cur_;
    }

    int8_t getInt8() const {
        return *(int8_t *)this->cur_;
    }

    int16_t getInt16() const {
        return *(int16_t *)this->cur_;
    }

    int32_t getInt32() const {
        return *(int32_t *)this->cur_;
    }

    int64_t getInt64() const {
        return *(int64_t *)this->cur_;
    }

    uint8_t getUInt8() const {
        return *(uint8_t *)this->cur_;
    }

    uint16_t getUInt16() const {
        return *(uint16_t *)this->cur_;
    }

    uint32_t getUInt32() const {
        return *(uint32_t *)this->cur_;
    }

    uint64_t getUInt64() const {
        return *(uint64_t *)this->cur_;
    }

    void * getPointer() const {
        return *(void **)this->cur_;
    }

    float getFloat() const {
        return *(float *)this->cur_;
    }

    double getDouble() const {
        return *(double *)this->cur_;
    }

    template <typename StringType>
    void getString(StringType & value, size_t length) {
        value.assign((const char_type *)this->cur_, length);
    }

    void setByte(uint8_t value) const {
        setUInt8(value);
    }

    void setChar(char_type value) const {
        *(char_type *)this->cur_ = value;
    }

    void setBool(bool value) {
        *(bool *)this->cur_ = value;
    }

    void setInt8(int8_t value) {
        *(int8_t *)this->cur_ = value;
    }

    void setInt16(int16_t value) {
        *(int16_t *)this->cur_ = value;
    }

    void setInt32(int32_t value) {
        *(int32_t *)this->cur_ = value;
    }

    void setInt64(int64_t value) {
        *(int64_t *)this->cur_ = value;
    }

    void setUInt8(uint8_t value) {
        *(uint8_t *)this->cur_ = value;
    }

    void setUInt16(uint16_t value) {
        *(uint16_t *)this->cur_ = value;
    }

    void setUInt32(uint32_t value) {
        *(uint32_t *)this->cur_ = value;
    }

    void setUInt64(uint64_t value) {
        *(uint64_t *)this->cur_ = value;
    }

    void setPointer(void * value) {
        *(void **)this->cur_ = value;
    }

    void setFloat(float value) {
        *(float *)this->cur_ = value;
    }

    void setDouble(double value) {
        *(double *)this->cur_ = value;
    }

    template <typename StringType>
    void setString(const StringType & value) {
        ::memcpy((void *)this->cur_, (const void *)value.c_str(), value.size() * sizeof(char_type));
    }
};

typedef BasicStream<char>      Stream;
typedef BasicStream<wchar_t>   StreamW;

} // namespace kvdb

#endif // KVDB_STREAM_BASICSTREAM_H
