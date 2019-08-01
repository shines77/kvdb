
#include "server/request_parser.h"
#include "server/request.h"

namespace kvdb {
namespace server {

request_parser::request_parser()
    : state_(method_start)
{
}

void request_parser::reset()
{
    state_ = method_start;
}

boost::tribool request_parser::consume(request & req, char input)
{
    switch (state_)
    {
    case expecting_newline_2:
        if (input == '\n')
        {
            state_ = header_line_start;
            return boost::indeterminate;
        }
        else
        {
            return false;
        }
    case expecting_newline_3:
        return (input == '\n');

    default:
        return false;
    }
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
