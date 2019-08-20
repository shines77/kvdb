
#ifndef KVDB_REQUEST_PARSER_H
#define KVDB_REQUEST_PARSER_H

#include <type_traits>

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

#include <kvdb/stream/InputStream.h>
#include <kvdb/stream/ParseResult.h>
#include <kvdb/stream/ParseStatus.h>
#include <kvdb/server/CommandType.h>
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

    int handleLoginCommand(ConnectionContext & context, InputStream & stream);
    int handleHandshakeCommand(InputStream & stream);
    int handleQueryCommand(InputStream & stream);

    int parseFirstQueryCommand(jstd::StringRef & cmd, const jstd::StringRef & qurey);

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
int RequestParser::parse(ConnectionContext & context, Request & req, InputIterator begin, InputIterator end)
{
    InputStream stream(begin);
    uint32_t command = stream.readUInt32();
    uint32_t count = stream.readUInt32();
    uint32_t total_size = stream.readUInt32();

    size_t length = (size_t)(end - begin);
    if (length >= ((size_t)total_size)) {
        req.header.command = command;
        req.header.count = count;
        req.header.total_size = total_size;

        const char * first = stream.current();
        int result = 0;
        switch (command) {
        case CommandType::Login:
            {
                result = handleLoginCommand(context, stream);
            }
            break;

        case CommandType::HandShake:
            {
                //result = handle_handshake_command(stream);
            }
            break;

        case CommandType::Query:
            {
                //result = handle_query_command(stream);
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

} // namespace server
} // namespace kvdb

#endif // KVDB_REQUEST_PARSER_H
