
#ifndef KVDB_STREAM_READ_WRITE_RESULT_H
#define KVDB_STREAM_READ_WRITE_RESULT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

struct ReadResult {
    enum Type {
        Failed = -1,
        Ok = 0,
        Last
    };
};

struct WriteResult {
    enum Type {
        Failed = -1,
        Ok = 0,
        Last
    };
};

} // namespace kvdb

#endif // KVDB_STREAM_READ_WRITE_RESULT_H
