
#ifndef KVDB_CLIENT_LOGINSTATE_H
#define KVDB_CLIENT_LOGINSTATE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

struct LoginState {
    enum Type {
        Unknown,
        Success,
        Error,
        Failed,
        AccountError,
        Last
    };
};

} // namespace kvdb

#endif // KVDB_CLIENT_LOGINSTATE_H
