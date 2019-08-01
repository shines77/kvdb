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

/// A request received from a client.
struct request
{
    uint8_t     major;
    uint8_t     minor;
    uint16_t    length;
    uint32_t    command;
    std::string uri;
    std::vector<header_field> fields;
};

} // namespace server
} // namespace kvdb

#endif // KVDB_REQUEST_H
