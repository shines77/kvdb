
#include "server/connection_manager.h"

#include <algorithm>
#include <boost/bind.hpp>

namespace kvdb {
namespace server {

void connection_manager::start(connection_ptr conn)
{
    connections_.insert(conn);
    conn->start();
}

void connection_manager::stop(connection_ptr conn)
{
    connections_.erase(conn);
    conn->stop();
}

void connection_manager::stop_all()
{
    std::for_each(connections_.begin(), connections_.end(),
                  boost::bind(&kvdb_connection::stop, _1));
    connections_.clear();
}

} // namespace server
} // namespace kvdb
