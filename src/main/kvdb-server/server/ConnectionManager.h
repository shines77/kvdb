
#ifndef KVDB_CONNECTION_MANAGER_H
#define KVDB_CONNECTION_MANAGER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <set>
#include <boost/noncopyable.hpp>
#include "server/Connection.h"
#include "server/KvdbConnection.h"

namespace kvdb {
namespace server {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class ConnectionManager : private boost::noncopyable
{
private:
    /// The managed connections.
    std::set<connection_ptr> connections_;

public:
    /// Add the specified connection to the manager and start it.
    void start(connection_ptr conn);

    /// Stop the specified connection.
    void stop(connection_ptr conn);

    /// Stop all connections.
    void stop_all();
};

} // namespace server
} // namespace kvdb

#endif // KVDB_CONNECTION_MANAGER_H
