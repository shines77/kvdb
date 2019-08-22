
#ifndef KVDB_SERVER_COMMANDTYPE_H
#define KVDB_SERVER_COMMANDTYPE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

struct CommandType {
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

#endif // KVDB_SERVER_COMMANDTYPE_H
