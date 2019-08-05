//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef KVDB_REQUEST_H
#define KVDB_REQUEST_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>

#include <string>
#include <vector>

#include "server/header_field.h"

namespace kvdb {
namespace server {

struct DataType {
    enum Type {
        Unknown,
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

struct Status {
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

struct request_header
{
    uint32_t    command;
    uint32_t    size;
};

/// A request received from a client.
struct request
{
    request_header  header;
    char            data[8];
};

} // namespace server
} // namespace kvdb

#endif // KVDB_REQUEST_H
