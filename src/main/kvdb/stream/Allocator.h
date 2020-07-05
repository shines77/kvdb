
#ifndef KVDB_STREAM_ALLOCATOR_H
#define KVDB_STREAM_ALLOCATOR_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <cstdlib>

namespace kvdb {

template <typename T>
struct IAllocator {
    typedef T *         pointer;
    typedef const T *   const_pointer;
    typedef std::size_t size_type;

    virtual pointer allocate(size_type size) = 0;
    virtual pointer reallocate(pointer ptr, size_type new_size) = 0;
    virtual pointer reallocate(void * ptr, size_type new_size) = 0;
    virtual void deallocate(pointer ptr) = 0;
    virtual void deallocate(void * ptr) = 0;

    bool isAutoRelease() { return true; }
};

template <typename T>
struct DummyAllocator {
    typedef T *         pointer;
    typedef const T *   const_pointer;
    typedef std::size_t size_type;

    pointer allocate(size_type size) {
        return nullptr;
    }

    pointer reallocate(pointer ptr, size_type new_size) {
        return nullptr;
    }

    pointer reallocate(void * ptr, size_type new_size) {
        return nullptr;
    }

    void deallocate(pointer ptr) {
    }

    void deallocate(void * ptr) {
    }

    bool isAutoRelease() { return false; }
};

template <typename T>
struct GenericAllocator {
    typedef T *         pointer;
    typedef const T *   const_pointer;
    typedef std::size_t size_type;

    pointer allocate(size_type size) {
        return new T[size];
    }

    pointer reallocate(pointer ptr, size_type new_size) {
        return new T[new_size];
    }

    pointer reallocate(void * ptr, size_type new_size) {
        return new T[new_size];
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

template <typename T>
struct MallocAllocator {
    typedef T *         pointer;
    typedef const T *   const_pointer;
    typedef std::size_t size_type;

    pointer allocate(size_type size) {
        return std::malloc(size);
    }

    pointer reallocate(pointer ptr, size_type new_size) {
        return static_cast<pointer>(std::realloc((void *)ptr, new_size));
    }

    pointer reallocate(void * ptr, size_type new_size) {
        return static_cast<pointer>(std::realloc(ptr, new_size));
    }

    void deallocate(pointer ptr) {
        assert(ptr != nullptr);
        std::free(ptr);
    }

    void deallocate(void * ptr) {
        assert(ptr != nullptr);
        std::free(ptr);
    }

    bool isAutoRelease() { return true; }
};

} // namespace kvdbz

#endif // KVDB_STREAM_ALLOCATOR_H