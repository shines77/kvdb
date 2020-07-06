
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

    static KvdbClientConfig client_config;

public:
    KvdbClientApp(int argc = 0, char * argv[] = nullptr)
        : kvdb::Application(argc, argv),
          options_desc("Command list"), need_echo(1) {
    }
    virtual ~KvdbClientApp() {}

    static KvdbClientConfig & get_client_config() {
        return KvdbClientApp::client_config;
    }

    virtual void initialize() override;
    virtual void finalize() override;

    virtual void welcome() override;
    virtual void print_usage() override;
    virtual int  main(int argc, char * argv[]) override;
    virtual int  main() override;

    void init_options();
    int  parse_args(int argc, char * argv[]);
    int  parse_args();

    void net_packet_test();
    void run_client(const std::string & address, uint16_t port);
    int  run_shell(int argc, char * argv[]);
};

} // namespace client
} // namespace kvdb

#endif // KVDB_CLIENT_APP_H
