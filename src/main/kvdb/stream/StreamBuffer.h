
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

#include "kvdb/stream/Allocator.h"
#include "kvdb/jstd/StringRef.h"

namespace kvdb {

template <typename T, typename Allocator = GenericAllocator<T>>
class BasicStreamBuffer {
public:
    typedef typename std::remove_pointer<
                typename std::remove_reference<
                    typename std::remove_cv<T>::type
                >::type
            >::type     char_type;

    typedef std::size_t                     size_type;

    typedef std::basic_string<char_type>    string_type;
    typedef jstd::BasicStringRef<char_type> stringref_type;

    typedef Allocator                       allocator_type;

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
          autoRelease_(false) {
    }
    template <size_type N>
    BasicStreamBuffer(const char_type(&data)[N])
        : cur_(data), head_(data), tail_(data + N), autoRelease_(false) {
    }
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

    size_type size() const   {
        assert(this->cur_ >= this->head_);
        return size_type(this->cur_ - this->head_);
    }
    size_type position() const { return this->size(); }

    size_type remain() const   {
        assert(this->tail_ >= this->cur_);
        return size_type(this->tail_ - this->cur_);
    }

    size_type capacity() const {
        assert(this->tail_ >= this->head_);
        return size_type(this->tail_ - this->head_);
    }

    bool isValid() const { return (this->head_ != nullptr); }
    bool isGood()  const { return (this->head_ != nullptr && this->tail_ != nullptr); }

    bool isUnderflow() const { return (this->cur_ <  this->head_); }
    bool isOverflow()  const { return (this->cur_ >= this->tail_); }

    bool isMemoryStream() const { return true; }

protected:
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
        ::memset((void *)this->cur_, 0, this->capacity() * sizeof(char_type));
    }

    void clear() {
        this->cur_ = nullptr;
        this->head_ = nullptr;
        this->tail_ = nullptr;
    }

    void reserve(size_type newCapacity, bool needClear = false) {
        if (newCapacity > this->capacity()) {
            size_type oldPos = this->position();
            this->internal_destroy();

            char_type * newData = allocator_.allocate(newCapacity);
            this->cur_ = newData + oldPos;
            this->head_ = newData;
            this->tail_ = newData + newCapacity;
            this->autoRelease_ = allocator_.isAutoRelease();

            if (needClear) {
                this->clear();
            }
        }
    }

    void inflate(size_type extraSize, bool needClear = false) {
        size_type oldPos = this->position();
        size_type newCapacity = oldPos + extraSize;
        if (newCapacity > this->capacity()) {
            char_type * newData = allocator_.allocate(newCapacity * sizeof(char_type));

            ::memmove((void *)newData, (const void *)this->data(), oldPos * sizeof(char_type));
            if (needClear) {
                ::memset((void *)(newData + oldPos), 0, extraSize * sizeof(char_type));
            }

            this->internal_destroy();

            this->cur_ = newData + oldPos;
            this->head_ = newData;
            this->tail_ = newData + newCapacity;
            this->autoRelease_ = allocator_.isAutoRelease();
        }
    }

    void resize(size_type newSize, bool needClear = true) {
        size_type oldCapacity = this->capacity();
        if (newSize > oldCapacity) {
            size_type oldPos = this->position();
            char_type * newData = allocator_.allocate(newSize * sizeof(char_type));

            ::memmove((void *)newData, (const void *)this->data(), oldCapacity * sizeof(char_type));
            if (needClear) {
                ::memset((void *)(newData + oldCapacity), 0, (newSize - oldCapacity) * sizeof(char_type));
            }

            this->internal_destroy();

            this->cur_ = newData + oldPos;
            this->head_ = newData;
            this->tail_ = newData + newSize;
            this->autoRelease_ = allocator_.isAutoRelease();
        }
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

typedef BasicStreamBuffer<char>      StreamBuffer;
typedef BasicStreamBuffer<wchar_t>   StreamBufferW;

} // namespace kvdb

#endif // KVDB_STREAM_STREAMBUFFER_H
