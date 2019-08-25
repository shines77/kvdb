
#ifndef KVDB_CORE_MESSAGETYPE_H
#define KVDB_CORE_MESSAGETYPE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

struct MessageType {
    enum Type {
        Unknown,
        Login,
        HandShake,
        Connect,
        Query,
        Logout,
        Last
    };
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGETYPE_H
