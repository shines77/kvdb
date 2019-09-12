
#ifndef KVDB_STREAM_STREAMBUFFER_H
#define KVDB_STREAM_STREAMBUFFER_H

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

namespace kvdb {

template <typename T>
struct IAllocator {
    typedef T *         pointer;
    typedef const T *   const_pointer;

    virtual pointer allocate(std::size_t size) = 0;
    virtual pointer reallocate(std::size_t size) = 0;
    virtual void deallocate(pointer ptr) = 0;
    virtual void deallocate(void * ptr) = 0;
};

template <typename T>
struct GenericAllocator {
    typedef T *         pointer;
    typedef const T *   const_pointer;

    pointer allocate(std::size_t size) {
        return new T[size];
    }

    pointer reallocate(std::size_t size) {
        return new T[size];
    }

    void deallocate(pointer ptr) {
        assert(ptr != nullptr);
        delete[] ptr;
    }

    void deallocate(void * ptr) {
        assert(ptr != nullptr);
        delete[] (pointer)ptr;
    }

    bool isAutoRelease() { return true; }
};

template <typename T, typename Allocator = GenericAllocator<T>>
class BasicStreamBuffer {
public:
    typedef typename std::remove_pointer<
                typename std::remove_cv<T>::type
            >::type     char_type;

    typedef std::size_t                     size_type;

    typedef Allocator                       allocator_type;
    typedef std::basic_string<char_type>    string_type;
    typedef jstd::BasicStringRef<char_type> stringref_type;

protected:
    char_type *      cur_;
    char_type *      head_;
    char_type *      tail_;
    bool             autoRelease_;
    allocator_type   allocator_;

public:
    BasicStreamBuffer() : cur_(nullptr), head_(nullptr), tail_(nullptr), autoRelease_(false) {}
    BasicStreamBuffer(const char_type * data, size_type size)
        : cur_(const_cast<char_type *>(data)),
          head_(const_cast<char_type *>(data)),
          tail_(const_cast<char_type *>(data + size)),
          autoRelease_(false) {}
    template <size_t N>
    BasicStreamBuffer(const char_type(&data)[N])
        : cur_(data), head_(data), tail_(data + N), autoRelease_(false) {}
    ~BasicStreamBuffer() {
        this->destroy();
    }

    void setAllocator(const allocator_type & allocator) {
        this->allocator_ = allocator;
    }

    void setBuffer(const char_type * data, size_type size, bool autoRelease = false) {
        this->cur_ = (char_type *)data;
        this->head_ = (char_type *)data;
        this->tail_ = (char_type *)(data + size);
        this->autoRelease_ = autoRelease;
    }

    template <size_t N>
    void setBuffer(const char_type(&data)[N]) {
        this->cur_ = (char_type *)data;
        this->head_ = (char_type *)data;
        this->tail_ = (char_type *)(data + N);
        this->autoRelease_ = false;
    }

    char_type * head() const { return this->head_; }
    char_type * tail() const { return this->tail_; }
    char_type * current() const { return this->cur_; }

    char_type * data() const { return this->head(); }

    bool autoRelease() const { return this->autoRelease_; }

    void setHead(char_type * head) { this->head_ = head; }
    void setTail(char_type * tail) { this->tail_ = tail; }
    void setCurrent(char_type * cur) { this->cur_ = cur; }

    void setAutoRelease(bool needRelease) { this->autoRelease_ = needRelease; }

    ptrdiff_t length() const   { return (this->cur_ - this->head_); }
    ptrdiff_t position() const { return this->length(); }

    size_type capacity() const {
        assert(this->tail_ >= this->head_);
        return size_type(this->tail_ - this->head_);
    }

    bool isValid() const { return (this->head_ != nullptr); }
    bool isGood()  const { return (this->head_ != nullptr && this->tail_ != nullptr); }

    bool isUnderflow() const { return (this->cur_ <  this->head_); }
    bool isOverflow()  const { return (this->cur_ >= this->tail_); }

    bool isMemoryStream() const { return true; }

private:
    void internal_destroy() {
        if (this->autoRelease_) {
            if (this->head_ != nullptr) {
                allocator_.deallocate(this->head_);
            }
        }
    }

public:
    void destroy() {
        if (this->autoRelease_) {
            if (this->head_ != nullptr) {
                allocator_.deallocate(this->head_);
                this->cur_ = nullptr;
                this->head_ = nullptr;
                this->tail_ = nullptr;
            }
        }
    }

    void reset() {
        this->cur_ = this->head_;
    }

    void clean() {
        this->cur_ = nullptr;
        this->head_ = nullptr;
        this->tail_ = nullptr;
    }

    void clear() {
        ::memset(this->cur_, 0, this->capacity() * sizeof(char_type));
    }

    void reserve(size_type newCapacity, bool needClear = false) {
        if (newCapacity > this->capacity()) {
            this->internal_destroy();

            char_type * newData = allocator_.allocate(newCapacity);
            this->cur_ = newData;
            this->head_ = newData;
            this->tail_ = newData + newCapacity;

            if (needClear) {
                this->clear();
            }
        }
    }

    void inflate(size_type extraSize, bool needClear = false) {
        size_type newCapacity = this->length() + extraSize;
        if (newCapacity > this->capacity()) {
            this->reserve(newCapacity);
        }
    }

    void resize(size_type newSize, bool needClear = true) {
        size_type oldSize = this->capacity();
        size_type oldDataSize = oldSize * sizeof(char_type);
        if (newSize > oldSize) {
            char_type * newData = allocator_.allocate(capacity);

            ::memmove((void *)newData, (const void *)this->data(), oldDataSize);
            if (needClear) {
                ::memset(this->newData + oldSize, 0, (newSize - oldSize) * sizeof(char_type));
            }

            this->internal_destroy();

            this->cur_ = data;
            this->head_ = data;
            this->tail_ = data + newSize;
        }
    }

    void _back() {
        this->cur_ = (char_type *)((char *)this->cur_ - sizeof(char));
    }

    void _back(int skip) {
        this->cur_ = (char_type *)((char *)this->cur_ - skip * sizeof(char));
    }

    void _next() {
        this->cur_ = (char_type *)((char *)this->cur_ + sizeof(char));
    }

    void _next(int skip) {
        this->cur_ = (char_type *)((char *)this->cur_ + skip * sizeof(char));
    }
};

typedef BasicStreamBuffer<char>      StreamBuffer;
typedef BasicStreamBuffer<wchar_t>   StreamBufferW;

} // namespace kvdb

#endif // KVDB_STREAM_STREAMBUFFER_H
