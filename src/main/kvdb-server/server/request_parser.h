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

#include <kvdb/stream/InputStream.h>
#include <kvdb/stream/ParseStatus.h>
#include <kvdb/server/CommandType.h>
#include <kvdb/server/ServerStatus.h>

namespace kvdb {
namespace server {

struct request;

/// Parser for incoming requests.
class request_parser
{
private:
    /// The current state of the parser.
    struct State {
        enum Type {
            Start,
            Login,
            HandShake,
            Connect,
            Disconnect,
            Logout,
            Last
        };
    };

    int state_;

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
    int parse(request & req, InputIterator begin, InputIterator end);

    int handle_login_command(InputStream & stream);
    int handle_handshake_command(InputStream & stream);

    int handle_request_data(const char * data);

private:
    /// Check if a byte is an HTTP character.
    static bool is_char(int c);

    /// Check if a byte is an HTTP control character.
    static bool is_ctl(int c);

    /// Check if a byte is defined as an HTTP tspecial character.
    static bool is_tspecial(int c);

    /// Check if a byte is a digit.
    static bool is_digit(int c);
};

template <typename InputIterator>
int request_parser::parse(request & req, InputIterator begin, InputIterator end)
{
    InputStream stream(begin);
    uint32_t command = stream.readUInt32();
    uint32_t total_size = stream.readUInt32();

    size_t kHeaderSize = sizeof(request_header);

    size_t length = (size_t)(end - begin);
    if (length >= ((size_t)total_size + kHeaderSize)) {
        req.header.command = command;
        req.header.size = total_size;

        const char * first = stream.current();
        int result = 0;
        switch (command) {
        case CommandType::Login:
            {
                result = handle_login_command(stream);
            }
            break;

        case CommandType::HandShake:
            {
                //handle_handshake_command(stream);
            }
            break;

        default:
            // Unknown command
            break;
        }

        const char * last = stream.current();
        if ((last - first) == (size_t)total_size) {
            return ParseStatus::Success;
        }
        else {
            return ParseStatus::Failed;
        }
    }
    else {
        return ParseStatus::TooSmall;
    }
}

} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_REQUEST_PARSER_HPP
