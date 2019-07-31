
#ifndef KVDB_STORE_SLICE_H
#define KVDB_STORE_SLICE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdint>
#include <string>

namespace kvdb {

struct SliceParts;

class Slice {
public:
    // Create an empty slice.
    Slice() : data_(""), size_(0) { }

    // Create a slice that refers to d[0,n-1].
    Slice(const char * data, size_t size) : data_(data), size_(size) { }

    // Create a slice that refers to the contents of "s"
    /* implicit */
    Slice(const std::string & str) : data_(str.data()), size_(str.size()) { }

    // Create a slice that refers to s[0, strlen(s)-1]
    /* implicit */
    Slice(const char * str) : data_(str), size_(::strlen(str)) { }

    template <std::size_t N>
    Slice(const char (&str)[N]) : data_(str), size_(N) { }

    // Create a single slice from SliceParts using buf as storage.
    // buf must exist as long as the returned Slice exists.
    Slice(const struct SliceParts & parts, std::string * buf) { }

    ~Slice() { }

    // Return a pointer to the beginning of the referenced data
    const char * data() const { return data_; }

    // Return the length (in bytes) of the referenced data
    size_t size() const { return size_; }

    // Return true iff the length of the referenced data is zero
    bool empty() const { return (size_ == 0); }

    // Return the ith byte in the referenced data.
    // REQUIRES: n < size()
    char operator [] (size_t index) const {
        assert(index < size());
        return data_[index];
    }

    // Reset this slice
    void reset() { data_ = nullptr; size_ = 0; }

    // Change this slice to refer to an empty array
    void clear() { data_ = ""; size_ = 0; }

    // Drop the first "n" bytes from this slice.
    void remove_prefix(size_t pos) {
        assert(pos <= size());
        data_ += pos;
        size_ -= pos;
    }

    void remove_suffix(size_t pos) {
        assert(pos <= size());
        size_ -= pos;
    }

    // Return a string that contains the copy of the referenced data.
    std::string toString(bool hex = false) const {
        return std::string(data_, size_);
    }

    // Three-way comparison.  Returns value:
    //   <  0 iff "*this" <  "b",
    //   == 0 iff "*this" == "b",
    //   >  0 iff "*this" >  "b"
    int compare(const Slice & b) const;

    // Return true iff "x" is a prefix of "*this"
    bool starts_with(const Slice & x) const {
        return ((size_ >= x.size_) &&
            (::memcmp(data_, x.data_, x.size_) == 0));
    }

    bool ends_with(const Slice & x) const {
        return ((size_ >= x.size_) &&
            (::memcmp(data_ + size_ - x.size_, x.data_, x.size_) == 0));
    }

    // Compare two slices and returns the first byte where they differ
    size_t difference_offset(const Slice & b) const;

    // private: make these public for rocksdbjni access
    const char * data_;
    std::size_t size_;

    // Intentionally copyable
};

// A set of Slices that are virtually concatenated together.  'parts' points
// to an array of Slices.  The number of elements in the array is 'num_parts'.
struct SliceParts {
    SliceParts(const Slice * _parts, int _num_parts) :
        parts(_parts), num_parts(_num_parts) { }
    SliceParts() : parts(nullptr), num_parts(0) {}

    const Slice * parts;
    int num_parts;
};

inline bool operator == (const Slice & x, const Slice & y) {
    return ((x.size() == y.size()) &&
        (::memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator != (const Slice & x, const Slice & y) {
    return !(x == y);
}

inline bool operator < (const Slice & x, const Slice & y) {
    return ((x.size() < y.size()) &&
            (::memcmp(x.data(), y.data(), x.size()) < 0));
}

inline bool operator > (const Slice & x, const Slice & y) {
    return ((x.size() > y.size()) &&
            (::memcmp(x.data(), y.data(), x.size()) > 0));
}

inline int Slice::compare(const Slice & b) const {
    const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
    int r = ::memcmp(data_, b.data_, min_len);
    if (r == 0) {
        if (size_ < b.size_) r = -1;
        else if (size_ > b.size_) r = +1;
    }
    return r;
}

inline size_t Slice::difference_offset(const Slice & b) const {
    size_t off = 0;
    const size_t len = (size_ < b.size_) ? size_ : b.size_;
    for (; off < len; off++) {
        if (data_[off] != b.data_[off]) break;
    }
    return off;
}

} // namespace KVCache

#endif // KVDB_STORE_SLICE_H
