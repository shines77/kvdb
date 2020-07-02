
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

#include "kvdb/jstd/StringRef.h"

#include "kvdb/stream/ReadWriteResult.h"
#include "kvdb/stream/StreamStorage.h"
#include "kvdb/stream/ByteBuffer.h"

namespace kvdb {

template <typename StorageTy>
class BasicStream : public BasicStreamStorage<StorageTy> {
public:
    typedef BasicStreamStorage<StorageTy>       base_type;
    typedef BasicStream<StorageTy>              this_type;

    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;
    typedef typename base_type::storage_type    storage_type;
    typedef typename base_type::allocator_type  allocator_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicStream(storage_type & storage) : base_type(storage) {}
    BasicStream(storage_type && storage) : base_type(std::forward<storage_type>(storage)) {}
    BasicStream(const char_type * data, size_type size) : base_type(data, size) {}
    template <size_type N>
    BasicStream(const char_type(&data)[N])
        : base_type(data, N) {
    }

    ~BasicStream() {}

    void back() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(char));
    }

    void back(int offset) {
        this->cur_ = (char_type *)((char *)this->cur_ - offset * sizeof(char));
    }

    void backByte() {
        backUInt8();
    }

    void backByte(int offset) {
        back(offset);
    }

    void backChar() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(char_type));
    }

    void backChar(int offset) {
        this->cur_ = (char_type *)((char *)this->cur_ - offset * sizeof(char_type));
    }

    void backBool() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(bool));
    }

    void backInt8() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(int8_t));
    }

    void backInt16() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(int16_t));
    }

    void backInt32() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(int32_t));
    }

    void backInt64() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(int64_t));
    }

    void backUInt8() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(uint8_t));
    }

    void backUInt16() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(uint16_t));
    }

    void backUInt32() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(uint32_t));
    }

    void backUInt64() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(uint64_t));
    }

    void backPointer() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(void *));
    }

    void backFloat() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(float));
    }

    void backDouble() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(double));
    }

    void skip() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(char));
    }

    void skip(int offset) {
        this->cur_ = (char_type *)((char *)this->cur_ + offset * sizeof(char));
    }

    void next() {
        skip();
    }

    void next(int offset) {
        skip(offset);
    }

    void skipByte() {
        skipUInt8();
    }

    void skipByte(int offset) {
        skip(offset);
    }

    void skipChar() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(char_type));
    }

    void skipChar(int offset) {
        this->cur_ = (char_type *)((char *)this->cur_ + offset * sizeof(char_type));
    }

    void skipBool() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(bool));
    }

    void skipInt8() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(int8_t));
    }

    void skipInt16() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(int16_t));
    }

    void skipInt32() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(int32_t));
    }

    void skipInt64() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(int64_t));
    }

    void skipUInt8() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(uint8_t));
    }

    void skipUInt16() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(uint16_t));
    }

    void skipUInt32() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(uint32_t));
    }

    void skipUInt64() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(uint64_t));
    }

    void skipPointer() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(void *));
    }

    void skipFloat() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(float));
    }

    void skipDouble() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(double));
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

typedef BasicStream< BasicByteBuffer<char> >      Stream;
typedef BasicStream< BasicByteBuffer<wchar_t> >   StreamW;

} // namespace kvdb

#endif // KVDB_STREAM_BASICSTREAM_H
