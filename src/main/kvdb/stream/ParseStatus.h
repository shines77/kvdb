
#ifndef KVDB_STREAM_PARSESTATUS_H
#define KVDB_STREAM_PARSESTATUS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

struct ParseStatus {
    enum Type {
        Failed = -1,
        Success = 0,
        Error,
        TooSmall,
        Last
    };
};

} // namespace kvdb

#endif // KVDB_STREAM_PARSESTATUS_H
