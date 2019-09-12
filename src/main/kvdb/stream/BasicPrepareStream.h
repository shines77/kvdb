
#ifndef KVDB_STREAM_BASIC_PREPARESTREAM_H
#define KVDB_STREAM_BASIC_PREPARESTREAM_H

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
class BasicPrepareStream : public BasicStreamBuffer<T> {
public:
    typedef BasicStreamBuffer<T>                base_type;
    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

public:
    BasicPrepareStream() : base_type() {}
    BasicPrepareStream(const char_type * data, size_type size) : base_type(data, size) {}
    template <size_t N>
    BasicPrepareStream(const char_type(&data)[N]) : base_type(data, N) {}
    ~BasicPrepareStream() {}

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
        this->cur_ = (char_type *)((char *)this->cur_ + skip);
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
        return '0';
    }

    bool getBool() const {
        return false;
    }

    int8_t getInt8() const {
        return 0;
    }

    int16_t getInt16() const {
        return 0;
    }

    int32_t getInt32() const {
        return 0;
    }

    int64_t getInt64() const {
        return 0;
    }

    uint8_t getUInt8() const {
        return 0;
    }

    uint16_t getUInt16() const {
        return 0;
    }

    uint32_t getUInt32() const {
        return 0;
    }

    uint64_t getUInt64() const {
        return 0;
    }

    void * getPointer() const {
        return nullptr;
    }

    float getFloat() const {
        return 0.0f;
    }

    double getDouble() const {
        return 0.0;
    }

    template <typename StringType>
    void getString(StringType & value, size_t length) {
    }

    void setByte(uint8_t value) const {
    }

    void setChar(char_type value) const {
    }

    void setBool(bool value) {
    }

    void setInt8(int8_t value) {
    }

    void setInt16(int16_t value) {
    }

    void setInt32(int32_t value) {
    }

    void setInt64(int64_t value) {
    }

    void setUInt8(uint8_t value) {
    }

    void setUInt16(uint16_t value) {
    }

    void setUInt32(uint32_t value) {
    }

    void setUInt64(uint64_t value) {
    }

    void setPointer(void * value) {
    }

    void setFloat(float value) {
    }

    void setDouble(double value) {
    }

    template <typename StringType>
    void setString(const StringType & value) {
    }
};

typedef BasicPrepareStream<char>    PrepareStream;
typedef BasicPrepareStream<wchar_t> PrepareStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_BASIC_PREPARESTREAM_H
