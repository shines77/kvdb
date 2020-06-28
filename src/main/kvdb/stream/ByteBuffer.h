
#ifndef KVDB_STREAM_BYTEBUFFER_H
#define KVDB_STREAM_BYTEBUFFER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <string>
#include <type_traits>

#include "kvdb/core/DataType.h"
#include "kvdb/stream/StreamBuffer.h"
#include "kvdb/jstd/StringRef.h"
#include "kvdb/support/Power2.h"
#include "kvdb/support/PowerOf2.h"

namespace kvdb {

template <typename T, typename Allocator = GenericAllocator<T>>
class BasicByteBuffer {
public:
    typedef typename std::remove_pointer<
                typename std::remove_reference<
                    typename std::remove_cv<T>::type
                >::type
            >::type     char_type;

    typedef std::size_t                     size_type;

    typedef Allocator                       allocator_type;
    typedef std::basic_string<char_type>    string_type;
    typedef jstd::BasicStringRef<char_type> stringref_type;

protected:
    char_type     * data_;
    size_type       size_;
    size_type       capacity_;
    allocator_type  allocator_;

    size_type calc_capacity(size_type size) {
        return detail::round_up_to_pow2(size);
    }

public:
    BasicByteBuffer() : data_(nullptr), size_(0), capacity_(0) {}
    BasicByteBuffer(const char_type * data, size_type size)
        : data_(const_cast<char_type *>(data)),
          size_(size),
          capacity_(calc_capacity(size)) {
    }
    template <size_type N>
    BasicByteBuffer(const char_type(&data)[N])
        : data_(data), size_(N), capacity_(compile_time::round_up_to_pow2<N>::value) {
    }
    ~BasicByteBuffer() {
        this->destroy();
    }

    char_type * data() const { return this->data_; }
    size_type   size() const { return this->size_; }
    size_type   capacity() const { return this->capacity_; }

    void setData(char_type * data) { this->data_ = data; }
    void setSize(size_type size) { this->size_ = size; this->setCapacity(size); }
    void setCapacity(size_type capacity) { this->capacity_ = calc_capacity(capacity); }

    size_type remain() const   {
        assert(this->capacity_ >= this->data_);
        return size_type(this->capacity_ - this->data_);
    }

    bool isValid() const { return (this->data_ != nullptr); }

    bool isUnderflow() const { return (this->size_ <  0); }
    bool isOverflow()  const { return (this->size_ > this->capacity_); }

    void attach(const char_type * data, size_type size) {
        this->data_ = (char_type *)data;
        this->size_ = size;
        this->capacity_ = calc_capacity(size);
    }

    template <size_t N>
    void attach(const char_type(&data)[N]) {
        this->data_ = (char_type *)data;
        this->size_ = N;
        this->capacity_ = calc_capacity(N);
    }

    void setAllocator(const allocator_type & allocator) {
        this->allocator_ = allocator;
    }

protected:
    void internal_destroy() {
        if (allocator_.isAutoRelease()) {
            if (this->data_ != nullptr) {
                allocator_.deallocate(this->data_);
            }
        }
    }

public:
    void destroy() {
        if (allocator_.isAutoRelease()) {
            if (this->data_ != nullptr) {
                allocator_.deallocate(this->data_);
                this->data_ = nullptr;
                this->size_ = 0;
                this->capacity_ = 0;
            }
        }
    }

    void clean(char_type initVal = 0) {
        assert(this->data() != nullptr);
        ::memset((void *)this->data(), initVal, this->size() * sizeof(char_type));
    }

    void clear() {
        this->size_ = 0;
    }

    void reserve(size_type newCapacity) {
        newCapacity = calc_capacity(newCapacity);
        if (newCapacity > this->capacity()) {
            char_type * newData = allocator_.reallocate(newCapacity);
            if (newData != nullptr) {
                if (newData != this->data()) {
                    this->internal_destroy();
                    this->data_ = newData;
                }

                this->size_ = 0;
                this->capacity_ = newCapacity;
            }
        }
    }

    void reserve(size_type newCapacity, char_type initVal) {
        newCapacity = calc_capacity(newCapacity);
        if (newCapacity > this->capacity()) {
            char_type * newData = allocator_.reallocate(newCapacity);
            if (newData != nullptr) {
                if (newData != this->data()) {
                    ::memset((void *)newData, initVal, newCapacity * sizeof(char_type));
                    this->internal_destroy();
                    this->data_ = newData;
                }

                this->size_ = 0;
                this->capacity_ = newCapacity;
            }
        }
    }

    void inflate(size_type extraSize) {
        size_type newSize = this->size() + extraSize;
        this->resize(newSize);
    }

    void inflate(size_type extraSize, char_type initVal) {
        size_type newSize = this->size() + extraSize;
        this->resize(newSize, initVal);
    }

    void resize(size_type newSize) {
        if (newSize > this->capacity()) {
            size_type newCapacity = calc_capacity(newSize);
            char_type * newData = allocator_.reallocate(newCapacity * sizeof(char_type));
            if (newData != nullptr) {
                if (newData != this->data()) {
                    ::memmove((void *)newData, (const void *)this->data(), this->size() * sizeof(char_type));
                    this->internal_destroy();
                    this->data_ = newData;
                }               

                this->size_ += newSize;
                this->capacity_ = newCapacity;
            }
        }
    }

    void resize(size_type newSize, char_type initVal) {
        if (newSize > this->capacity()) {
            size_type newCapacity = calc_capacity(newSize);
            char_type * newData = allocator_.reallocate(newCapacity * sizeof(char_type));
            if (newData != nullptr) {
                if (newData != this->data()) {
                    ::memmove((void *)newData, (const void *)this->data(), this->size() * sizeof(char_type));
                    this->internal_destroy();
                    this->data_ = newData;
                }

                ::memset((void *)(newData + this->size()), initVal, (newSize - this->size()) * sizeof(char_type));
                
                this->size_ += newSize;
                this->capacity_ = newCapacity;
            }
        }
    }
};

typedef BasicByteBuffer<char>      ByteBuffer;
typedef BasicByteBuffer<wchar_t>   ByteBufferW;

} // namespace kvdb

#endif // KVDB_STREAM_BYTEBUFFER_H
