
#ifndef KVDB_CORE_OPCODE_H
#define KVDB_CORE_OPCODE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

namespace kvdb {

struct Opcode {
    enum Type {
        Unknown = 0,

        HandShakeRequest    = 0x0001,
        HandShakeResponse   = HandShakeRequest,

        LoginRequest        = 0x0002,
        LoginResponse       = LoginRequest,

        LogoutRequest       = 0x0003,
        LogoutResponse      = LogoutRequest,

        QueryRequest        = 0x0004,
        QueryResponse       = QueryRequest,

        Last
    };
};

} // namespace kvdb

#endif // KVDB_CORE_OPCODE_H
