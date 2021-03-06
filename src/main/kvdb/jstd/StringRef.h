
#ifndef JSTD_STRINGREF_H
#define JSTD_STRINGREF_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>
#include <string.h>
#include <cstdint>  // for std::intptr_t
#include <cstddef>  // for std::ptrdiff_t
#include <string>
#include <type_traits>

#include "kvdb/basic/stddef.h"
#include "kvdb/jstd/string_iterator.h"
#include "kvdb/jstd/string_utils.h"
#include "kvdb/jstd/strlen.h"

namespace jstd {

template <typename CharTy>
class BasicStringRef {
public:
    typedef CharTy          char_type;
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef CharTy *        pointer;
    typedef const CharTy *  const_pointer;
    typedef CharTy &        reference;
    typedef const CharTy &  const_reference;

    typedef jstd::string_iterator<BasicStringRef<CharTy>>       iterator;
    typedef jstd::const_string_iterator<BasicStringRef<CharTy>> const_iterator;

    typedef std::basic_string<char_type>    string_type;
    typedef BasicStringRef<char_type>       stringref_type;

private:
    const char_type * data_;
    size_type length_;

public:
    BasicStringRef() : data_(nullptr), length_(0) {}
    BasicStringRef(const char_type * data)
        : data_(data), length_(detail::StrLen(data)) {}
    BasicStringRef(const char_type * data, size_type length)
        : data_(data), length_(length) {}
    BasicStringRef(const char_type * first, const char_type * last)
        : data_(first), length_(size_type(last - first)) {}
    template <size_type N>
    BasicStringRef(const char_type(&data)[N])
        : data_(data), length_(N - 1) {}
    BasicStringRef(const string_type & src)
        : data_(src.c_str()), length_(src.size()) {}
    BasicStringRef(const stringref_type & src)
        : data_(src.data()), length_(src.length()) {}
    BasicStringRef(string_type && src)
        : data_(src.c_str()), length_(src.size()) {
        // Only reference from src string
    }
    BasicStringRef(stringref_type && src)
        : data_(src.data()), length_(src.length()) {
        src.reset();
    }
    ~BasicStringRef() { /* Do nothing! */ }

    BasicStringRef & operator = (const char_type * data) {
        this->data_ = data;
        this->length_ = detail::StrLen(data);
        return *this;
    }

    BasicStringRef & operator = (const string_type & rhs) {
        this->data_ = rhs.c_str();
        this->length_ = rhs.size();
        return *this;
    }

    BasicStringRef & operator = (const stringref_type & rhs) {
        this->data_ = rhs.data();
        this->length_ = rhs.length();
        return *this;
    }

    char_type operator [] (size_t index) const {
        return (char_type)(this->data_[index]);
    }

    const char_type * data() const { return this->data_; }
    size_t length() const { return this->length_; }

    const char_type * c_str() const { return this->data(); }
    size_t size() const { return this->length(); }

    char_type * data() { return const_cast<char_type *>(this->data_); }
    char_type * c_str() { return const_cast<char_type *>(this->data()); }

    bool empty() const { return (this->length() == 0); }

    iterator begin() const { return iterator(this->data_); }
    iterator end() const { return iterator(this->data_ + this->length_); }

    const_iterator cbegin() const { return const_iterator(this->data_); }
    const_iterator cend() const { return const_iterator(this->data_ + this->length_); }

    void reset() {
        this->data_ = nullptr;
        this->length_ = 0;
    }

    void clear() {
        this->reset();
    }

    void assign(const char_type * data) {
        this->set_data(data, detail::StrLen(data));
    }

    void assign(const char_type * data, size_type length) {
        this->set_data(data, length);
    }

    void assign(const char_type * first, const char_type * last) {
        this->set_data(first, size_type(last - first));
    }

    template <size_type N>
    void assign(const char_type (&data)[N]) {
        this->set_data(data, N - 1);
    }

    void assign(const string_type & src) {
        this->set_data(src.c_str(), src.size());
    }

    void assign(const stringref_type & src) {
        this->set_data(src.data(), src.length());
    }

    void set_data(const char_type * data, size_t length) {
        this->data_ = data;
        this->length_ = length;
    }

    void set_data(const char_type * data) {
        this->set_data(data, detail::StrLen(data));
    }

    void set_data(const char_type * first, const char_type * last) {
        this->set_data(first, size_type(last - first));
    }

    void set_data(const char_type * data, size_type first, size_type last) {
        this->set_data(data + first, size_type(last - first));
    }

    template <size_t N>
    void set_data(const char_type(&data)[N]) {
        this->set_data(data, N - 1);
    }

    void set_data(const string_type & src) {
        this->set_data(src.c_str(), src.size());
    }

    void set_data(const stringref_type & src) {
        this->set_data(src.data(), src.length());
    }

    char_type at(size_t index) const {
        return (char_type)(this->data_[index]);
    }

    string_type toString() const {
        return std::move(string_type(this->data_, this->length_));
    }
}; // class BasicStringRef<CharTy>

template <typename StringTy, typename CharTy>
class BasicStringHelper {
public:
    typedef StringTy string_type;
    typedef CharTy char_type;

private:
    string_type str_;
    bool truncated_;
    char_type save_char_;

public:
    BasicStringHelper()
        : truncated_(false),
          save_char_(static_cast<char_type>('\0')) {
        (void)save_char_;
    }
    ~BasicStringHelper() { detach(); }

    bool attach(const string_type & str) {
        // If the string reference don't recover the truncated char,
        // don't accept the new attach.
        if (likely(!truncated_)) {
            str_ = str;
        }
        return (!truncated_);
    }

    void detach() {
        // If have be truncated, recover the saved terminator char first,
        // and then clear the string reference.
        if (unlikely(truncated_)) {
            recover();
            str_.clear();
        }
    }

    void truncate() {
        if (likely(!truncated_)) {
            char_type * first = (char_type *)str_.data();
            char_type * last = first + str_.size();
            assert(last != nullptr);
            save_char_ = *last;
            *last = static_cast<char_type>('\0');
            truncated_ = true;
        }
    }

    void recover() {
        if (likely(truncated_)) {
            char_type * first = (char_type *)str_.data();
            char_type * last = first + str_.size();
            assert(last != nullptr);
            *last = save_char_;
            truncated_ = false;
        }
    }
};

template <typename CharTy>
inline
bool operator == (const BasicStringRef<CharTy> & lhs, const BasicStringRef<CharTy> & rhs) {
    return jstd::StrUtils::is_equals_fast(lhs, rhs);
}

template <typename CharTy>
inline
bool operator < (const BasicStringRef<CharTy> & lhs, const BasicStringRef<CharTy> & rhs) {
    return (jstd::StrUtils::compare(lhs, rhs) == jstd::StrUtils::IsSmaller);
}

template <typename CharTy>
inline
bool operator > (const BasicStringRef<CharTy> & lhs, const BasicStringRef<CharTy> & rhs) {
    return (jstd::StrUtils::compare(lhs, rhs) == jstd::StrUtils::IsBigger);
}

typedef BasicStringRef<char>                        StringRef;
typedef BasicStringRef<wchar_t>                     StringRefW;

typedef BasicStringHelper<std::string, char>        StringHelper;
typedef BasicStringHelper<std::wstring, wchar_t>    StringHelperW;

typedef BasicStringHelper<StringRef, char>          StringRefHelper;
typedef BasicStringHelper<StringRefW, wchar_t>      StringRefHelperW;

} // namespace jstd

#endif // JSTD_STRINGREF_H
