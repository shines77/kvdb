
#ifndef KVDB_STREAM_STREAMSTORAGE_H
#define KVDB_STREAM_STREAMSTORAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/basic/stdsize.h"

#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <string>
#include <type_traits>

#include "kvdb/stream/Allocator.h"
#include "kvdb/stream/ByteBuffer.h"
#include "kvdb/jstd/StringRef.h"

namespace kvdb {

template <typename StorageTy>
class BasicStreamStorage {
public:
    typedef StorageTy                               storage_type;
    typedef BasicStreamStorage<StorageTy>           this_type;

    typedef typename storage_type::char_type        char_type;
    typedef typename storage_type::size_type        size_type;
    typedef typename storage_type::allocator_type   allocator_type;

    typedef typename storage_type::string_type      string_type;
    typedef typename storage_type::stringref_type   stringref_type;

protected:
    const char_type * cur_;
    storage_type &    storage_;

public:
    BasicStreamStorage(storage_type & storage) : cur_(storage.data()), storage_(storage) {}
    BasicStreamStorage(storage_type && storage) : cur_(storage.data()), storage_(std::forward<storage_type>(storage)) {}
    ~BasicStreamStorage() {
    }

    const char_type * data() const { return const_cast<const char_type *>(this->storage_.data()); }
    const char_type * head() const { return const_cast<const char_type *>(this->storage_.data()); }
    const char_type * tail() const { return const_cast<const char_type *>(this->storage_.data() + this->storage_.size()); }
    const char_type * current() const { return this->cur_; }

    char_type * data() { return this->storage_.data(); }
    char_type * head() { return this->storage_.data(); }
    char_type * tail() { return (this->storage_.data() + this->storage_.size()); }
    char_type * current() { return const_cast<char_type *>(this->cur_); }

    intptr_t position() const { return (this->current() - this->head()); }
    size_type size() const { return static_cast<size_type>(this->position()); }
    size_type remain() const { return (this->capacity() - this->storage_size()); }

    size_type storage_size() const { return this->storage_.size(); }
    size_type storage_capacity() const { return this->storage_.capacity(); }

    void setCurrent(const char_type * cur) { this->cur_ = cur; }

    void setStorageData(char_type * newData) { this->storage_.setData(newData); }
    void setStorageSize(size_type newSize) { this->storage_.setSize(newSize); }

    storage_type & storage() { return this->storage_; }
    const storage_type & storage() const { return this->storage_; }

    storage_type & setStorage(storage_type & storage) {
        this->storage_ = storage;
        return this->storage_;
    }

    void setAllocator(const allocator_type & allocator) {
        this->storage_.setAllocator(allocator);
    }

    bool isValid() const { return this->storage_.isValid(); }

    bool isUnderflow() const { return (this->current() <  this->head()); }
    bool isOverflow()  const { return (this->current() >= this->tail()); }

    void reset() {
        this->cur_ = this->head();
    }

    void clean() {
        ::memset((void *)this->data(), 0, this->capacity() * sizeof(char_type));
    }

    void clear() {
        this->cur_ = nullptr;
        this->storage_.clear();
    }

    void allocate(size_type size) {
        this->storage_.allocate(size);
        this->reset();
    }

    void allocate(size_type size, char_type initVal) {
        this->storage_.allocate(size, initVal);
        this->reset();
    }

    void reserve(size_type newCapacity) {
        intptr_t pos = this->position();
        this->storage_.reserve(newCapacity);
        this->setCurrent(this->head() + pos);
    }

    void reserve(size_type newCapacity, char_type initVal) {
        intptr_t pos = this->position();
        this->storage_.reserve(newCapacity, initVal);
        this->setCurrent(this->head() + pos);
    }

    void inflate(size_type extraSize) {
        intptr_t pos = this->position();
        this->storage_.inflate(extraSize);
        this->setCurrent(this->head() + pos);
    }

    void inflate(size_type extraSize, char_type initVal) {
        intptr_t pos = this->position();
        this->storage_.inflate(extraSize, initVal);
        this->setCurrent(this->head() + pos);
    }

    void resize(size_type newSize) {
        intptr_t pos = this->position();
        this->storage_.resize(newSize);
        this->setCurrent(this->head() + pos);
    }

    void resize(size_type newSize, char_type initVal) {
        intptr_t pos = this->position();
        this->storage_.resize(newSize, initVal);
        this->setCurrent(this->head() + pos);
    }

    void _back() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(char));
    }

    void _back(int offset) {
        this->cur_ = (char_type *)((char *)this->cur_ - offset * sizeof(char));
    }

    void _skip() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(char));
    }

    void _skip(int offset) {
        this->cur_ = (char_type *)((char *)this->cur_ + offset * sizeof(char));
    }
};

typedef BasicStreamStorage< BasicByteBuffer<char> >      StreamStorage;
typedef BasicStreamStorage< BasicByteBuffer<wchar_t> >   StreamStorageW;

} // namespace kvdb

#endif // KVDB_STREAM_STREAMSTORAGE_H
