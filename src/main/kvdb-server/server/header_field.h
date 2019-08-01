
#ifndef KVDB_HEADER_FILED_H
#define KVDB_HEADER_FILED_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>

namespace kvdb {
namespace server {

struct header_field
{
    std::string name;
    std::string value;
};

} // namespace server
} // namespace kvdb

#endif // KVDB_HEADER_FILED_H
