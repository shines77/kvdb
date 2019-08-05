//
// request_parser.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER2_REQUEST_PARSER_HPP
#define HTTP_SERVER2_REQUEST_PARSER_HPP

#include <type_traits>

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

namespace kvdb {
namespace server {

template <typename T>
class InputStream {
public:
    typedef typename std::remove_pointer<T>::type   value_type;

private:
    value_type * current_;

public:
    InputStream() : current_(nullptr) {}
    InputStream(T value) : current_(value) {}
    ~InputStream() {}

    char readChar() const {
        return *(char *)current_;
    }

    unsigned char readUChar() const {
        return *(unsigned char *)current_;
    }

    int8_t readInt8() const {
        return *(int8_t *)current_;
    }

    int16_t readInt16() const {
        return *(int16_t *)current_;
    }

    int32_t readInt32() const {
        return *(int32_t *)current_;
    }

    int64_t readInt64() const {
        return *(int64_t *)current_;
    }

    uint8_t readUInt8() const {
        return *(uint8_t *)current_;
    }

    uint16_t readUInt16() const {
        return *(uint16_t *)current_;
    }

    uint32_t readUInt32() const {
        return *(uint32_t *)current_;
    }

    uint64_t readUInt64() const {
        return *(uint64_t *)current_;
    }
};

struct ParseStatus {
    enum Type {
        Unknown,
        Success,
        TooSmall,
        Failed,
        Last
    };
};

struct request;

/// Parser for incoming requests.
class request_parser
{
public:
    /// Construct ready to parse the request method.
    request_parser();

    /// Reset to initial parser state.
    void reset();

    /// Parse some data. The tribool return value is true when a complete request
    /// has been parsed, false if the data is invalid, indeterminate when more
    /// data is required. The InputIterator return value indicates how much of the
    /// input has been consumed.
    template <typename InputIterator>
    int parse(request & req, InputIterator begin, InputIterator end) {
        InputStream<InputIterator> stream(begin);
        uint32_t command = stream.readUInt32();
        uint32_t total_size = stream.readUInt32();

        size_t kHeaderSize = sizeof(uint32_t) * 2;

        size_t length = (size_t)(end - begin);
        if (length >= (total_size + kHeaderSize)) {
            return ParseStatus::Success;
        }
        else {
            return ParseStatus::TooSmall;
        }
    }

private:
    /// Handle the next character of input.
    boost::tribool consume(request& req, char input);

    /// Check if a byte is an HTTP character.
    static bool is_char(int c);

    /// Check if a byte is an HTTP control character.
    static bool is_ctl(int c);

    /// Check if a byte is defined as an HTTP tspecial character.
    static bool is_tspecial(int c);

    /// Check if a byte is a digit.
    static bool is_digit(int c);

    /// The current state of the parser.
    enum state
    {
        method_start,
        method,
        uri,
        http_version_h,
        http_version_t_1,
        http_version_t_2,
        http_version_p,
        http_version_slash,
        http_version_major_start,
        http_version_major,
        http_version_minor_start,
        http_version_minor,
        expecting_newline_1,
        header_line_start,
        header_lws,
        header_name,
        space_before_header_value,
        header_value,
        expecting_newline_2,
        expecting_newline_3
    } state_;
};

} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_REQUEST_PARSER_HPP
