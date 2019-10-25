
#ifndef KVDB_CORE_MESSAGE_TYPE_H
#define KVDB_CORE_MESSAGE_TYPE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

namespace kvdb {

struct MessageType {
    enum Type {
        Unknown = 0,

        LoginRequest,
        LoginResponse,

        HandShakeRequest,
        HandShakeResponse,

        ConnectRequest,
        ConnectResponse,

        QueryRequest,
        QueryResponse,

        DisconnectRequest,
        DisconnectResponse,

        LogoutRequest,
        LogoutResponse,

        Last
    };
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_TYPE_H
