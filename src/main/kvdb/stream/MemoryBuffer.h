
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
    ~BasicMemoryBuffer() {
    }
   
    char_type * data() { return this->data_; }
    const char_type * data() const { return this->data_; }
    size_type   size() const { return this->size_; }

    void setData(char_type * data) { this->data_ = data; }
    void setSize(size_type size) { this->size_ = size; }

    bool isValid() const { return (this->data_ != nullptr); }
};

typedef BasicMemoryBuffer<char>      MemoryBuffer;
typedef BasicMemoryBuffer<wchar_t>   MemoryBufferW;

} // namespace kvdb

#endif // KVDB_STREAM_MEMORYBUFFER_H
