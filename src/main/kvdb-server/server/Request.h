
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
public:
    char * data;

public:
    Request() : NetPacket(), data(nullptr) {}
    virtual ~Request() {}
};

} // namespace server
} // namespace kvdb

#endif // KVDB_REQUEST_H
