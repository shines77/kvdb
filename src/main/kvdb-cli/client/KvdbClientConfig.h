
#ifndef KVDB_CLIENT_CONFIG_H
#define KVDB_CLIENT_CONFIG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <boost/noncopyable.hpp>

namespace kvdb {
namespace client {

struct KvdbClientConfig {
    std::string remote;
    std::string local;
    std::string address;
    uint16_t port;

    std::string username;
    std::string password;
    std::string database;

    std::string prefix;

    KvdbClientConfig() : port(0) {}

    KvdbClientConfig(const std::string & host)
        : remote(host), port(0) {
        //
    }

    KvdbClientConfig(const std::string & _address, uint16_t _port)
        : address(_address), port(_port) {
        //
    }
};

} // namespace client
} // namespace kvdb

#endif // KVDB_CLIENT_CONFIG_H
