
#ifndef KVDB_STREAM_ALLOCATOR_H
#define KVDB_STREAM_ALLOCATOR_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

#include <assert.h>

#include <cstdint>
#include <cstddef>

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

} // namespace kvdbz

#endif // KVDB_STREAM_ALLOCATOR_H