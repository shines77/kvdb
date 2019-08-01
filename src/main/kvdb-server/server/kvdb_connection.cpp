
#include "server/kvdb_connection.h"

using namespace boost::system;
using namespace boost::asio;

namespace kvdb {
namespace server {

void kvdb_connection::start()
{      
    set_socket_send_bufsize(MAX_PACKET_SIZE);
    set_socket_recv_bufsize(MAX_PACKET_SIZE);

    static const int kNetSendTimeout = 45 * 1000;    // Send timeout is 45 seconds.
    static const int kNetRecvTimeout = 45 * 1000;    // Recieve timeout is 45 seconds.
    ::setsockopt(socket_.native_handle(), SOL_SOCKET, SO_SNDTIMEO, (const char *)&kNetSendTimeout, sizeof(kNetSendTimeout));
    ::setsockopt(socket_.native_handle(), SOL_SOCKET, SO_RCVTIMEO, (const char *)&kNetRecvTimeout, sizeof(kNetRecvTimeout));

    linger sLinger;
    sLinger.l_onoff = 1;    // Enable linger
    sLinger.l_linger = 5;   // After shutdown(), socket send/recv 5 second data yet.
    ::setsockopt(socket_.native_handle(), SOL_SOCKET, SO_LINGER, (const char *)&sLinger, sizeof(sLinger));

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


typedef boost::shared_ptr<kvdb_connection> connection_ptr;

} // namespace server
} // namespace kvdb

#undef USE_ATOMIC_REALTIME_UPDATE
#undef QUERY_COUNTER_INTERVAL

#undef MAX_UPDATE_CNT
#undef MAX_UPDATE_BYTES
