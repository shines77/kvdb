
#ifndef KVDB_STREAM_PARSERESULT_H
#define KVDB_STREAM_PARSERESULT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

struct ParseResult {
    enum Type {
        Failed = -1,
        OK = 0,
        Success = OK,
        Error,
        Last
    };
};

} // namespace kvdb

#endif // KVDB_STREAM_PARSERESULT_H
