
#ifndef KVDB_STREAM_CONSTBUFFER_H
#define KVDB_STREAM_CONSTBUFFER_H

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

namespace kvdb {

template <typename T>
class BasicConstBuffer : public BasicMemoryBuffer<T> {
public:
    typedef BasicMemoryBuffer<T>                base_type;
    typedef BasicConstBuffer<T>                 this_type;

    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    typedef DummyAllocator<T>                   allocator_type;

public:
    BasicConstBuffer() : base_type() {}
    BasicConstBuffer(const this_type & src)
        : base_type(src.data(), src.size()) {
    }
    BasicConstBuffer(const char_type * data, size_type size)
        : base_type(data, size) {
    }
    template <size_type N>
    BasicConstBuffer(const char_type(&data)[N])
        : base_type(data, N) {
    }
    ~BasicConstBuffer() {
    }

    void attach(const char_type * data, size_type size) {
        this->data_ = (char_type *)data;
        this->size_ = size;
    }

    template <size_type N>
    void attach(const char_type(&data)[N]) {
        this->data_ = (char_type *)data;
        this->size_ = N;
    }

    void clean() {
        assert(this->data() != nullptr);
        ::memset((void *)this->data(), 0, this->size() * sizeof(char_type));
    }

    void clean(char_type initVal) {
        assert(this->data() != nullptr);
        ::memset((void *)this->data(), initVal, this->size() * sizeof(char_type));
    }
};

typedef BasicConstBuffer<char>      ConstBuffer;
typedef BasicConstBuffer<wchar_t>   ConstBufferW;

} // namespace kvdb

#endif // KVDB_STREAM_CONSTBUFFER_H
