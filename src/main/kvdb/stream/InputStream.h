
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

#include "kvdb/stream/DataType.h"
#include "kvdb/jstd/StringRef.h"

namespace kvdb {

template <typename T, typename VauleType = T>
class BasicInputStream {
public:
    typedef typename std::remove_pointer<
                typename std::remove_cv<T>::type
            >::type     char_type;
    typedef typename std::remove_pointer<
                typename std::remove_cv<VauleType>::type
            >::type     value_type;

    typedef std::basic_string<char_type>        string_type;
    typedef jstd::BasicStringRef<char_type>     stringref_type;

private:
    char_type * current_;

public:
    BasicInputStream() : current_(nullptr) {}
    BasicInputStream(const char_type * value) : current_(const_cast<char_type *>(value)) {}
    BasicInputStream(value_type * value) : current_(static_cast<char_type *>(value)) {}
    ~BasicInputStream() {}

    char * current() const {
        return (char *)current_;
    }

    char_type * currentPtr() const {
        return current_;
    }

    void back() {
        backChar();
    }

    void back(int skip) {
        current_ = (char_type *)((char *)current_ - skip);
    }

    void backChar() {
        current_ = (char_type *)((char *)current_ - sizeof(char));
    }

    void backUChar() {
        current_ = (char_type *)((char *)current_ - sizeof(unsigned char));
    }

    void next() {
        nextChar();
    }

    void next(int skip) {
        current_ = (char_type *)((char *)current_ + skip);
    }

    void nextChar() {
        current_ = (char_type *)((char *)current_ + sizeof(char));
    }

    void nextUChar() {
        current_ = (char_type *)((char *)current_ + sizeof(unsigned char));
    }

    void nextInt8() {
        current_ = (char_type *)((char *)current_ + sizeof(int8_t));
    }

    void nextInt16() {
        current_ = (char_type *)((char *)current_ + sizeof(int16_t));
    }

    void nextInt32() {
        current_ = (char_type *)((char *)current_ + sizeof(int32_t));
    }

    void nextInt64() {
        current_ = (char_type *)((char *)current_ + sizeof(int64_t));
    }

    void nextUInt8() {
        current_ = (char_type *)((char *)current_ + sizeof(uint8_t));
    }

    void nextUInt16() {
        current_ = (char_type *)((char *)current_ + sizeof(uint16_t));
    }

    void nextUInt32() {
        current_ = (char_type *)((char *)current_ + sizeof(uint32_t));
    }

    void nextUInt64() {
        current_ = (char_type *)((char *)current_ + sizeof(uint64_t));
    }

    char get() const {
        return *(char *)current_;
    }

    unsigned char getu() const {
        return *(unsigned char *)current_;
    }

    char getChar() const {
        return *(char *)current_;
    }

    unsigned char getUChar() const {
        return *(unsigned char *)current_;
    }

    int8_t getInt8() const {
        return *(int8_t *)current_;
    }

    int16_t getInt16() const {
        return *(int16_t *)current_;
    }

    int32_t getInt32() const {
        return *(int32_t *)current_;
    }

    int64_t getInt64() const {
        return *(int64_t *)current_;
    }

    uint8_t getUInt8() const {
        return *(uint8_t *)current_;
    }

    uint16_t getUInt16() const {
        return *(uint16_t *)current_;
    }

    uint32_t getUInt32() const {
        return *(uint32_t *)current_;
    }

    uint64_t getUInt64() const {
        return *(uint64_t *)current_;
    }

    char readChar() {
        char value = getChar();
        nextChar();
        return value;
    }

    unsigned char readUChar() {
        unsigned char value = getUChar();
        nextUChar();
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

    void readString(string_type & value, size_t length) {
        value.assign((const char_type *)current_, length);
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

    void readString(stringref_type & value, size_t length) {
        value.assign((const char_type *)current_, length);
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
