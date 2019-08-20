
#include "server/RequestParser.h"
#include "server/Request.h"

namespace kvdb {
namespace server {

RequestParser::RequestParser()
    : state_(State::Start)
{
}

void RequestParser::reset()
{
    state_ = State::Start;
}

int RequestParser::handleLoginCommand(ConnectionContext & context, InputStream & stream)
{
    std::string username, password, database;
    int result = stream.parseString(username);
    if (result == ParseResult::OK) {
        int result = stream.parseString(password);
        if (result == ParseResult::OK) {
            int result = stream.parseString(database);
            if (result == ParseResult::OK) {
                return ParseStatus::Success;
            }
        }
    }
    else {
        //
    }
    return ParseStatus::Failed;
}

int RequestParser::handleHandshakeCommand(InputStream & stream)
{
    return ParseStatus::Success;
}

int RequestParser::parseFirstQueryCommand(jstd::StringRef & cmd, const jstd::StringRef & qurey)
{
    size_t first = 0, last;
    size_t i;
    for (i = 0; i < qurey.size(); ++i) {
        if (qurey[i] != ' ') {
            break;
        }
        first++;
    }

    last = first;
    for ( ; i < qurey.size(); ++i) {
        if (qurey[i] != ' ') {
            last++;
        }
        else {
            break;
        }
    }

    cmd.set_data(qurey.data(), first, last);
    return (int)cmd.size();
}

int RequestParser::handleQueryCommand(InputStream & stream)
{
    jstd::StringRef qurey;
    int result = stream.parseString(qurey);
    if (result == ParseResult::OK) {
        jstd::StringRef cmd;
        int result = parseFirstQueryCommand(cmd, qurey);
        return (result > 0) ? ParseStatus::Success : ParseStatus::Error;
    }
    else {
        //
    }
    return ParseStatus::Failed;
}

int RequestParser::handleRequestData(const char * data)
{
    InputStream stream(data);
    bool isEndOf = false;
    while (!isEndOf) {
        uint8_t data_type = stream.getUInt8();
        switch (data_type) {
        case DataType::EndOf:
            {
                stream.next();
                uint8_t end_of = stream.getUInt8();
                if (end_of == '\0') {
                    isEndOf = true;
                }
                stream.next();
            }
            break;

        case DataType::Bool:
            break;

        default:
            // Unknown errors
            break;
        }
    }
    return 0;
}

bool RequestParser::is_char(int c)
{
    return (c >= 0 && c <= 127);
}

bool RequestParser::is_ctl(int c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

bool RequestParser::is_tspecial(int c)
{
    switch (c) {
    case '(': case ')': case '<': case '>':  case '@':
    case ',': case ';': case ':': case '\\': case '"':
    case '/': case '[': case ']': case '?':  case '=':
    case '{': case '}': case ' ': case '\t':
        return true;

    default:
        return false;
    }
}

bool RequestParser::is_digit(int c)
{
    return c >= '0' && c <= '9';
}

} // namespace server
} // namespace kvdb
