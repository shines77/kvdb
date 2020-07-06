
#ifndef KVDB_STREAM_MEMORYBUFFER_H
#define KVDB_STREAM_MEMORYBUFFER_H

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
#include "kvdb/jstd/StringRef.h"

namespace kvdb {

template <typename T>
class BasicMemoryBuffer {
public:
    typedef typename std::remove_pointer<
                typename std::remove_reference<
                    typename std::remove_cv<T>::type
                >::type
            >::type     char_type;

    typedef std::size_t                     size_type;

    typedef BasicMemoryBuffer<T>            this_type;
    typedef std::basic_string<char_type>    string_type;
    typedef jstd::BasicStringRef<char_type> stringref_type;

protected:
    char_type * data_;
    size_type   size_;

public:
    BasicMemoryBuffer() : data_(nullptr), size_(0) {}
    BasicMemoryBuffer(const char_type * data, size_type size)
        : data_(const_cast<char_type *>(data)),
          size_(size) {
    }
    template <size_type N>
    BasicMemoryBuffer(const char_type(&data)[N])
        : data_(data), size_(N) {
    }
    BasicMemoryBuffer(const this_type & src)
        : data_(src.data()), size_(src.size()) {
    }
    BasicMemoryBuffer(this_type && right)
        : data_(nullptr), size_(0) {
        this->swap(std::forward<this_type>(right));
    }
    ~BasicMemoryBuffer() {
    }
   
    char_type * data() { return this->data_; }
    const char_type * data() const { return this->data_; }
    size_type   size() const { return this->size_; }

    void setData(char_type * data) { this->data_ = data; }
    void setSize(size_type size) { this->size_ = size; }

    bool isValid() const { return (this->data_ != nullptr); }

    void swap(this_type & right) {
        char_type * tmpData = this->data_;
        this->data_ = right.data();
        right.setData(tmpData);

        size_type tmpSize = this->size_;
        this->size_ = right.size();
        right.setSize(tmpSize);
    }

    char_type & operator [] (size_type pos) {
        assert(pos >= 0 && pos < this->size());
        return *(this->data_ + pos);
    }

    const char_type & operator [] (size_type pos) const {
        assert(pos >= 0 && pos < this->size());
        return *(this->data_ + pos);
    }

    char_type & operator * () const {
        return (*this->data_);
    }
};

typedef BasicMemoryBuffer<char>      MemoryBuffer;
typedef BasicMemoryBuffer<wchar_t>   MemoryBufferW;

} // namespace kvdb

template <typename T>
inline
void swap(kvdb::BasicMemoryBuffer<T> & left, kvdb::BasicMemoryBuffer<T> & right)
{
    left.swap(right);
}

#endif // KVDB_STREAM_MEMORYBUFFER_H
