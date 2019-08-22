
#include "server/RequestHandler.h"

#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

#include "server/Request.h"
#include "server/Response.h"

namespace kvdb {
namespace server {

RequestHandler::RequestHandler(const std::string & doc_root)
    : doc_root_(doc_root)
{
}

void RequestHandler::handle_request(const Request & req, Response & res)
{
    // Decode url to path.
    std::string request_path;
    if (req.data != nullptr) {
        if (!url_decode(req.data, request_path)) {
            res = Response::stock_response(Response::bad_request);
            return;
        }
    }

    // Request path must be absolute and not contain "..".
    if (request_path.empty() || request_path[0] != '/'
        || request_path.find("..") != std::string::npos) {
        res = Response::stock_response(Response::bad_request);
        return;
    }

    // If path ends in slash (i.e. is a directory) then add "index.html".
    if (request_path[request_path.size() - 1] == '/') {
        request_path += "index.html";
    }

    // Determine the file extension.
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
        extension = request_path.substr(last_dot_pos + 1);
    }

    // Open the file to send back.
    std::string full_path = doc_root_ + request_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    if (!is) {
        res = Response::stock_response(Response::not_found);
        return;
    }

    // Fill out the reply to be sent to the client.
    res.status = Response::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0) {
        res.content.append(buf, is.gcount());
    }

    res.fields.resize(2);
    res.fields[0].name = "Content-Length";
    res.fields[0].value = boost::lexical_cast<std::string>(res.content.size());
    res.fields[1].name = "Content-Type";
    res.fields[1].value = "html/text";
}

bool RequestHandler::url_decode(const std::string & in, std::string & out)
{
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%') {
            if (i + 3 <= in.size()) {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    out += static_cast<char>(value);
                    i += 2;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else if (in[i] == '+') {
            out += ' ';
        }
        else {
            out += in[i];
        }
    }
    return true;
}

} // namespace server
} // namespace kvdb
