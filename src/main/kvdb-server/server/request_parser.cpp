
#include "server/request_parser.h"
#include "server/request.h"

namespace kvdb {
namespace server {

request_parser::request_parser()
    : state_(State::Start)
{
}

void request_parser::reset()
{
    state_ = State::Start;
}

int request_parser::handle_login_command(InputStream & stream)
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

int request_parser::handle_handshake_command(InputStream & stream)
{
    return ParseStatus::Success;
}

int request_parser::parse_first_query_command(jstd::StringRef & cmd, const jstd::StringRef & qurey)
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

int request_parser::handle_query_command(InputStream & stream)
{
    jstd::StringRef qurey;
    int result = stream.parseString(qurey);
    if (result == ParseResult::OK) {
        jstd::StringRef cmd;
        int result = parse_first_query_command(cmd, qurey);
        return (result > 0) ? ParseStatus::Success : ParseStatus::Error;
    }
    else {
        //
    }
    return ParseStatus::Failed;
}

int request_parser::handle_request_data(const char * data)
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

bool request_parser::is_char(int c)
{
    return (c >= 0 && c <= 127);
}

bool request_parser::is_ctl(int c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

bool request_parser::is_tspecial(int c)
{
    switch (c) {
    case '(': case ')': case '<': case '>': case '@':
    case ',': case ';': case ':': case '\\': case '"':
    case '/': case '[': case ']': case '?': case '=':
    case '{': case '}': case ' ': case '\t':
        return true;

    default:
        return false;
    }
}

bool request_parser::is_digit(int c)
{
    return c >= '0' && c <= '9';
}

} // namespace server
} // namespace http
