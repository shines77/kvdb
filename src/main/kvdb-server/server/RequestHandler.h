
#ifndef KVDB_REQUEST_HANDLER_H
#define KVDB_REQUEST_HANDLER_H

#include <string>
#include <boost/noncopyable.hpp>

namespace kvdb {
namespace server {

struct Response;
struct Request;

/// The common handler for all incoming requests.
class RequestHandler : private boost::noncopyable
{
private:
    /// The directory containing the files to be served.
    std::string doc_root_;

    /// Perform URL-decoding on a string. Returns false if the encoding was
    /// invalid.
    static bool url_decode(const std::string & in, std::string & out);

public:
    /// Construct with a directory containing files to be served.
    explicit RequestHandler(const std::string & doc_root);

    /// Handle a request and produce a response.
    void handle_request(const Request & req, Response & res);
};

} // namespace server
} // namespace http

#endif // KVDB_REQUEST_HANDLER_H
