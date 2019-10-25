
#ifndef KVDB_CLIENT_COMMON_H
#define KVDB_CLIENT_COMMON_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>
#include <string>
#include <atomic>

#include "kvdb/common/boost_asio_msvc.h"
#include "kvdb/common/aligned_atomic.h"

#define _ToString(x)            #x

#define APP_NAME                "kvdb"

#define DEFAULT_SERVER_ADDRESS  "127.0.0.1"
#define DEFAULT_SERVER_PORT     8077
#define DEFAULT_SERVER_PORT_S   "8077"

#define DEFAULT_LOCAL_IP        "127.0.0.1"

#define DEFAULT_SERVER_HOST     DEFAULT_SERVER_ADDRESS ":" DEFAULT_SERVER_PORT_S

#define CLIENT_PREFIX           APP_NAME "-" DEFAULT_SERVER_HOST "> "

#endif // KVDB_CLIENT_COMMON_H
