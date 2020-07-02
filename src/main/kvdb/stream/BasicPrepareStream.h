
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

#include "kvdb/jstd/StringRef.h"

#include "kvdb/stream/ReadWriteResult.h"
#include "kvdb/stream/StreamStorage.h"
#include "kvdb/stream/StreamBuffer.h"

namespace kvdb {

template <typename StorageTy>
class BasicPrepareStream : public BasicStreamStorage<StorageTy> {
public:
    typedef BasicStreamStorage<StorageTy>       base_type;
    typedef BasicPrepareStream<StorageTy>       this_type;

    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;
    typedef typename base_type::storage_type    storage_type;
    typedef typename base_type::allocator_type  allocator_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicPrepareStream(storage_type & storage) : base_type(storage) {}
    BasicPrepareStream(storage_type && storage) : base_type(std::forward<storage_type>(storage)) {}
    BasicPrepareStream(const char_type * data, size_type size) : base_type(data, size) {}
    template <size_type N>
    BasicPrepareStream(const char_type(&data)[N])
        : base_type(data, N) {
    }

    ~BasicPrepareStream() {}

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

typedef BasicPrepareStream< BasicByteBuffer<char> >    PrepareStream;
typedef BasicPrepareStream< BasicByteBuffer<wchar_t> > PrepareStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_BASIC_PREPARESTREAM_H
