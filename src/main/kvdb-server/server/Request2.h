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

#include "server/HeaderField.h"

namespace kvdb {
namespace server {

struct RequestHeader
{
    uint32_t    command;
    uint32_t    size;
};

#pragma warning (push)
#pragma warning (disable: 4200)

/// A request received from a client.
struct RequestData
{
    RequestHeader  header;
    char           data[0];
};

#pragma warning (pop)

/// A request received from a client.
struct Request
{
    RequestHeader  header;
    const char *   data;
};

} // namespace server
} // namespace kvdb

#endif // KVDB_REQUEST_H