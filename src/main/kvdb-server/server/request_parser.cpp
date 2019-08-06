
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
    if (result == 0) {
        int result = stream.parseString(password);
        if (result == 0) {
            int result = stream.parseString(database);
            if (result == 0) {
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
    return 0;
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

} // namespace server2
} // namespace http
