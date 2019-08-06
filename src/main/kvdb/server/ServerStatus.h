
#ifndef KVDB_SERVER_SERVERSTATUS_H
#define KVDB_SERVER_SERVERSTATUS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

struct ServerStatus {
    enum Type {
        Unknown,

        LoginFailed,
        LoginOK,

        HandShakeFailed,
        HandShakeOK,

        ConnectFailed,
        Connected,

        QueryError,
        QueryOK,

        LogoutFailed,
        LogoutOK,

        Last
    };
};

} // namespace kvdb

#endif // KVDB_SERVER_SERVERSTATUS_H
