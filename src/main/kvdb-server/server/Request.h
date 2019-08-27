
#ifndef KVDB_REQUEST_H
#define KVDB_REQUEST_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>

#include <string>
#include <vector>

#include "server/HeaderField.h"
#include "kvdb/stream/NetPacket.h"

namespace kvdb {
namespace server {

/// A request received from a client.
class Request : public NetPacket
{
private:
    const char * data_;

public:
    Request() : NetPacket(), data_(nullptr) {}
    virtual ~Request() {}

    char * data() { return (char *)(this->data_); }
    const char * data() const { return this->data_; }

    void setData(const char * data) { this->data_ = data; }
};

} // namespace server
} // namespace kvdb

#endif // KVDB_REQUEST_H
