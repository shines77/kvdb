
#ifndef KVDB_REQUEST_HANDLER_H
#define KVDB_REQUEST_HANDLER_H

#include <string>
#include <boost/noncopyable.hpp>

namespace kvdb {
namespace server {

struct response;
struct request;

/// The common handler for all incoming requests.
class request_handler : private boost::noncopyable
{
private:
    /// The directory containing the files to be served.
    std::string doc_root_;

    /// Perform URL-decoding on a string. Returns false if the encoding was
    /// invalid.
    static bool url_decode(const std::string & in, std::string & out);

public:
    /// Construct with a directory containing files to be served.
    explicit request_handler(const std::string & doc_root);

    /// Handle a request and produce a response.
    void handle_request(const request & req, response & res);
};

} // namespace server2
} // namespace http

#endif // KVDB_REQUEST_HANDLER_H
