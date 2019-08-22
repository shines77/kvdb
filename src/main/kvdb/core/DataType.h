
#ifndef KVDB_STREAM_DATATYPE_H
#define KVDB_STREAM_DATATYPE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

struct DataType {
    enum Type {
        EndOf = 0,
        Unknown = EndOf,
        Bit,
        Bool,
        Byte,
        Int8,
        Int16,
        Int32,
        Int64,
        UInt8,
        UInt16,
        UInt32,
        UInt64,
        Pointer,
        Float,
        Double,
        LongDouble,
        Decimal,
        String,
        String1B,
        String2B,
        String3B,
        String4B,
        Binary,
        Binary1B,
        Binary2B,
        Binary3B,
        Binary4B,
        Date,
        Time,
        Year,
        DateTime,
        TimeStamp,
        Bitmap,
        Last
    };
};

} // namespace kvdb

#endif // KVDB_STREAM_DATATYPE_H
