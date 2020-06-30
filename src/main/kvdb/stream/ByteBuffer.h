
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

#include "kvdb/stream/Allocator.h"
#include "kvdb/stream/MemoryBuffer.h"
#include "kvdb/jstd/StringRef.h"
#include "kvdb/support/Power2.h"
#include "kvdb/support/PowerOf2.h"

namespace kvdb {

template <typename T, typename Allocator = GenericAllocator<T>>
class BasicByteBuffer : public BasicMemoryBuffer<T> {
public:
    typedef BasicMemoryBuffer<T>                base_type;
    typedef BasicByteBuffer<T, Allocator>       this_type;

    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    typedef Allocator                           allocator_type;

protected:
    size_type       capacity_;
    allocator_type  allocator_;

    size_type calc_capacity(size_type size) {
        return detail::round_up_to_pow2(size);
    }

public:
    BasicByteBuffer() : base_type(nullptr, 0), capacity_(0) {}
    BasicByteBuffer(const char_type * data, size_type size)
        : base_type(nullptr, 0), capacity_(0) {
        this->copy(data, size);
    }
    template <size_type N>
    BasicByteBuffer(const char_type(&data)[N])
        : base_type(nullptr, 0), capacity_(0) {
        this->internal_copy<N>(data);
    }
    ~BasicByteBuffer() {
        this->destroy();
    }

    size_type capacity() const { return this->capacity_; }

    void setCapacity(size_type capacity) {
        this->capacity_ = calc_capacity(capacity);
    }

    size_type remain() const   {
        assert(this->capacity_ >= this->data_);
        return size_type(this->capacity_ - this->data_);
    }

    bool isUnderflow() const { return (this->size_ < 0); }
    bool isOverflow()  const { return (this->size_ > this->capacity_); }

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

    template <size_type Size>
    void internal_copy(const char_type * data) {
        assert(data != nullptr);
        assert(data != this->data());
        const size_type n_capacity = compile_time::_round_up_to_power2<Size>::value;
        this->reserve(n_capacity);
        if (Size <= this->capacity()) {
            ::memmove((void *)this->data(), (const void *)data, Size * sizeof(char_type));
            this->size_ = Size;
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

    void copy(const char_type * data, size_type size) {
        assert(data != nullptr);
        assert(data != this->data());
        size_type n_capacity = calc_capacity(size);
        this->reserve(n_capacity);
        if (size <= this->capacity()) {
            ::memmove((void *)this->data(), (const void *)data, size * sizeof(char_type));
            this->size_ = size;
        }
    }

    template <size_type N>
    void copy(const char_type(&data)[N]) {
        this->internal_copy<N>(data);
    }

    void clean(char_type initVal = 0) {
        assert(this->data() != nullptr);
        ::memset((void *)this->data(), initVal, this->size() * sizeof(char_type));
    }

    void clear() {
        this->size_ = 0;
    }

    void reserve(size_type newCapacity) {
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
