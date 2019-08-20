
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

#include "kvdb/stream/DataType.h"
#include "kvdb/jstd/StringRef.h"

namespace kvdb {

template <typename T, typename VauleType = T>
class BasicStream {
public:
    typedef typename std::remove_pointer<
                typename std::remove_cv<T>::type
            >::type     char_type;
    typedef typename std::remove_pointer<
                typename std::remove_cv<VauleType>::type
            >::type     value_type;

    typedef std::basic_string<char_type>        string_type;
    typedef jstd::BasicStringRef<char_type>     stringref_type;

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

    value_type * currentPtr() const {
        return (value_type *)cur_;
    }

    ptrdiff_t length() const   { return (cur_ - head_); }
    ptrdiff_t position() const { return this->length(); }

    bool isOverflow()  const { return false; }
    bool isUnderflow() const { return (cur_ < head_); }

    void reset() {
        cur_ = head_;
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

    void backChar() {
        cur_ = (char_type *)((char *)cur_ - sizeof(value_type));
    }

    void backChar(int skip) {
        cur_ = (char_type *)((char *)cur_ - skip * sizeof(value_type));
    }

    void next() {
        cur_ = (char_type *)((char *)cur_ + sizeof(char));
    }

    void next(int skip) {
        cur_ = (char_type *)((char *)cur_ + skip);
    }

    void nextByte() {
        next();
    }

    void nextChar() {
        cur_ = (char_type *)((char *)cur_ + sizeof(value_type));
    }

    void nextChar(int skip) {
        cur_ = (char_type *)((char *)cur_ + skip * sizeof(value_type));
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

    char get() const {
        return getInt8();
    }

    unsigned char getu() const {
        return getUInt8();
    }

    uint8_t getByte() const {
        return getUInt8();
    }

    value_type getChar() const {
        return *(value_type *)cur_;
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

    void setByte(uint8_t value) const {
        setUInt8(value);
    }

    void setChar(value_type value) const {
        *(value_type *)cur_ = value;
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
};

typedef BasicStream<char>      Stream;
typedef BasicStream<wchar_t>   StreamW;

} // namespace kvdb

#endif // KVDB_STREAM_BASICSTREAM_H
