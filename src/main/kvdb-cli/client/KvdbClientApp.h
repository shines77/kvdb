
#ifndef KVDB_CLIENT_APP_H
#define KVDB_CLIENT_APP_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <kvdb/core/Application.h>

#include <stdint.h>

#include <string>
#include <boost/program_options.hpp>

#include "client/KvdbClientConfig.h"

namespace options = boost::program_options;

namespace kvdb {
namespace client {

class KvdbClientApp : public kvdb::Application
{
public:
    std::string                     app_name;
    options::options_description    options_desc;

    std::string server_host, local_address;
    std::string username, password, database;
    int32_t need_echo;

    KvdbClientConfig client_config;

public:
    KvdbClientApp(int argc = 0, char * argv[] = nullptr)
        : kvdb::Application(argc, argv),
          options_desc("Command list"), need_echo(1) {
    }
    virtual ~KvdbClientApp() {}

    virtual void initialize() override;
    virtual void finalize() override;

    virtual void welcome() override;
    virtual void print_usage() override;
    virtual int  main(int argc, char * argv[]) override;

    void init_options();
    void parse_command_line();

    void net_packet_test();
    void run_kvdb_client(const std::string & address, uint16_t port);
    int  run_kvdb_shell(int argc, char * argv[]);
};

} // namespace client
} // namespace kvdb

#endif // KVDB_CLIENT_APP_H
