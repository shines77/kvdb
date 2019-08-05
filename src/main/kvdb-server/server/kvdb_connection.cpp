
#include "server/kvdb_connection.h"

using namespace boost::system;
using namespace boost::asio;

namespace kvdb {
namespace server {

void kvdb_connection::start()
{
    set_socket_options();

    g_client_count++;

    do_read_some();
}

void kvdb_connection::stop()
{
    if (socket_.is_open()) {
        shutdown_both();

#if !defined(_WIN32_WINNT) || (_WIN32_WINNT >= 0x0600)
        //socket_.cancel();
#endif
        socket_.close();

        if (g_client_count.load() != 0)
            g_client_count--;
    }
}

void kvdb_connection::stop_connection(const boost::system::error_code & ec)
{
    if (ec != boost::asio::error::operation_aborted)
    {
        //if (connection_manager_)
        //    connection_manager_->stop(shared_from_this());
        stop();
    }
}

} // namespace server
} // namespace kvdb
