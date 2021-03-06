
#include "server/ConnectionManager.h"

#include <functional>
#include <algorithm>
#include <boost/bind.hpp>

namespace kvdb {
namespace server {

void ConnectionManager::start(connection_ptr conn)
{
    connections_.insert(conn);
    conn->start();
}

void ConnectionManager::stop(connection_ptr conn)
{
    connections_.erase(conn);
    conn->stop();
}

void ConnectionManager::stop_all()
{
    std::for_each(connections_.begin(), connections_.end(),
                  std::bind(&Connection::stop, std::placeholders::_1));
    connections_.clear();
}

} // namespace server
} // namespace kvdb
