
#ifndef KVDB_CORE_STATUSCODE_H
#define KVDB_CORE_STATUSCODE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

struct StatusCode {
    enum Type {
        // Default Status
        Failed = -1,
        Unknown = 0,
        Ok = 1,
        Success = Ok,

        // Response Status
        LoginSuccess,
        LoginFailed,
        TooSmall,
        Last
    };
};

} // namespace kvdb

#endif // KVDB_CORE_STATUSCODE_H
