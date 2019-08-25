
#ifndef KVDB_REQUEST_PARSER_H
#define KVDB_REQUEST_PARSER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <type_traits>

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

#include <kvdb/core/MessageType.h>
#include <kvdb/stream/InputStream.h>
#include <kvdb/stream/ParseResult.h>
#include <kvdb/stream/ParseStatus.h>
#include <kvdb/server/ServerStatus.h>

#include <kvdb/jstd/StringRef.h>

#include "server/Request.h"
#include "server/ConnectionContext.h"

namespace kvdb {
namespace server {

/// Parser for incoming requests.
class RequestParser
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
    RequestParser();

    /// Reset to initial parser state.
    void reset();

    /// Parse some data. The tribool return value is true when a complete request
    /// has been parsed, false if the data is invalid, indeterminate when more
    /// data is required. The InputIterator return value indicates how much of the
    /// input has been consumed.
    template <typename InputIterator>
    int parse(ConnectionContext & context, Request & req, InputIterator begin, InputIterator end);

    int handleLoginMessage(ConnectionContext & context, InputStream & stream);
    int handleHandshakeMessage(InputStream & stream);
    int handleQueryMessage(InputStream & stream);

    int parseFirstQueryMessage(jstd::StringRef & cmd, const jstd::StringRef & qurey);

    int handleRequestData(const char * data);

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
int RequestParser::parse(ConnectionContext & context, Request & req,
                         InputIterator begin, InputIterator end)
{
    InputStream stream(begin);

    uint32_t msgType = stream.readUInt32();
    uint32_t msgLength = stream.readUInt32();
    uint32_t varCount = stream.readUInt32();

    size_t length = (size_t)(end - begin);
    if (length >= ((size_t)msgLength)) {
        req.header.msgType = msgType;
        req.header.msgLength = msgLength;
        req.header.varCount = varCount;

        const char * first = stream.current();
        int result = 0;
        switch (msgType) {
        case MessageType::Login:
            {
                result = handleLoginMessage(context, stream);
            }
            break;

        case MessageType::HandShake:
            {
                result = handleHandshakeMessage(stream);
            }
            break;

        case MessageType::Query:
            {
                result = handleQueryMessage(stream);
            }
            break;

        default:
            // Unknown command
            break;
        }

        const char * last = stream.current();
        if ((last - first) == (size_t)msgLength) {
            return ParseStatus::Success;
        }
        else {
            return ParseStatus::Failed;
        }
    }
    else {
        return ParseStatus::Success;
        //return ParseStatus::TooSmall;
    }
}

} // namespace server
} // namespace kvdb

#endif // KVDB_REQUEST_PARSER_H
