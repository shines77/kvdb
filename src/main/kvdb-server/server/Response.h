
#ifndef KVDB_RESPONSE_H
#define KVDB_RESPONSE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>
#include <vector>
#include <boost/asio.hpp>

#include "server/HeaderField.h"
#include "kvdb/stream/NetPacket.h"

namespace kvdb {
namespace server {

/// A response to be sent to a client.
class Response : public NetPacket
{
public:
    /// The status of the reply.
    enum StatusType
    {
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        multiple_choices = 300,
        moved_permanently = 301,
        moved_temporarily = 302,
        not_modified = 304,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
    } status;

private:
    const char * data_;

public:
    Response() : NetPacket(), data_(nullptr) {}
    virtual ~Response() {}

    char * data() { return (char *)(this->data_); }
    const char * data() const { return this->data_; }

    void setData(const char * data) { this->data_ = data; }
};

} // namespace server
} // namespace kvdb

#endif // KVDB_RESPONSE_H
