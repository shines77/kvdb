
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include <iostream>
#include <iomanip>  // std::setw(), std::setfill(), std::setprecision().
#include <sstream>
#include <string>
#include <utility>

#include "kvdb/common/boost_asio_msvc.h"
#include <boost/asio.hpp>

#include "client/common.h"
#include "client/KvdbClient.h"

#include "kvdb/all.h"

#include <boost/program_options.hpp>

// String compare mode
#define STRING_COMPARE_STDC     0
#define STRING_COMPARE_U64      1
#define STRING_COMPARE_SSE42    2

#define STRING_COMPARE_MODE     STRING_COMPARE_SSE42

#define _ToString(x)            #x

#define APP_NAME                "kvdb"

#define DEFAULT_SERVER_ADDRESS  "127.0.0.1"
#define DEFAULT_SERVER_PORT     8077
#define DEFAULT_SERVER_PORT_S   "8077"

#define DEFAULT_LOCAL_IP        "127.0.0.1"

#define DEFAULT_SERVER_HOST     DEFAULT_SERVER_ADDRESS":"DEFAULT_SERVER_PORT_S

#define CLIENT_PREFIX           APP_NAME"-"DEFAULT_SERVER_HOST"> "

using namespace kvdb;
using namespace kvdb::client;

uint32_t g_test_mode    = kvdb::test_mode_echo_server;
uint32_t g_test_method  = kvdb::test_method_pingpong;
uint32_t g_nodelay      = 0;
uint32_t g_need_echo    = 1;
uint32_t g_packet_size  = 64;

std::string g_test_mode_str      = "echo";
std::string g_test_method_str    = "pingpong";
std::string g_test_mode_full_str = "echo server";
std::string g_nodelay_str        = "false";
std::string g_rpc_topic;

std::string g_server_ip;
std::string g_server_port;

kvdb::aligned_atomic<uint64_t> kvdb::g_query_count(0);
kvdb::aligned_atomic<uint32_t> kvdb::g_client_count(0);

kvdb::aligned_atomic<uint64_t> kvdb::g_recv_bytes(0);
kvdb::aligned_atomic<uint64_t> kvdb::g_send_bytes(0);

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

KvdbClientConfig clientConfig;

void net_packet_test()
{
    char packetBuf[4096];

    NetPacket packet;
    OutputStream ostream(packetBuf);
    ostream.setStream(packetBuf);
    packet.writeTo(ostream);

    PacketHeader & header = packet.getHeaderInfo();
    ptrdiff_t length = ostream.length();

    InputStream istream(packetBuf);
    int count = packet.readFrom(istream);
}

void run_kvdb_client(const std::string & address, uint16_t port)
{
    try {
        // Alias of size_t
        //std::string::size_type sz;
        //int i_port = std::stoi(port, &sz);

        boost::asio::io_service io_service;
        KvdbClient client(io_service, address, port);
        client.run();

        std::cout << "Client is connectting ..." << std::endl;
        std::cout << std::endl;

        uint64_t last_query_count = 0;
        while (true) {
            auto cur_succeed_count = (uint64_t)g_query_count;
            auto client_count = (uint32_t)g_client_count;
            auto qps = (cur_succeed_count - last_query_count);
            std::cout << address.c_str() << ":" << port << " - "
                      << "nodelay:" << g_nodelay << ", "
                      << "mode:" << g_test_mode_str.c_str() << ", "
                      << "test:" << g_test_method_str.c_str() << ", "
                      << "qps=" << std::right << std::setw(7) << qps << ", "
                      << "BW="
                      << std::right << std::setw(6)
                      << std::setiosflags(std::ios::fixed) << std::setprecision(3)
                      << (qps * 8 / (1024.0 * 1024.0))
                      << " Mb/s" << std::endl;
            std::cout << std::right;
            last_query_count = cur_succeed_count;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        client.join();
    }
    catch (const std::exception & ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
}

void make_spaces(std::string & spaces, std::size_t size)
{
    spaces = "";
    for (std::size_t i = 0; i < size; ++i)
        spaces += " ";
}

void print_usage(const std::string & app_name, const boost::program_options::options_description & options_desc)
{
    std::string leader_spaces;
    make_spaces(leader_spaces, app_name.size());

    std::cerr << std::endl;
    std::cerr << options_desc << std::endl;

    std::cerr << "Usage: " << std::endl << std::endl
              << "  " << app_name.c_str()      << " --host=<host> --local=<local> --username=<username> --password=<password>" << std::endl
              << "  " << leader_spaces.c_str() << " --database=<database> [--echo=[0,1]]" << std::endl
              << std::endl
              << "For example: " << std::endl << std::endl
              << "  " << app_name.c_str()      << " --host=" << DEFAULT_SERVER_HOST
                                               << " --local=" << DEFAULT_LOCAL_IP
                                               << " --username=root --password=user1234" << std::endl
              << "  " << leader_spaces.c_str() << " --database=table1 --echo=0" << std::endl
              << std::endl
              << "  " << app_name.c_str() << " -s " << DEFAULT_SERVER_HOST
                                          << " -l " << DEFAULT_LOCAL_IP
                                          << " -u root -p user1234 -d table1 -e 0" << std::endl;
    std::cerr << std::endl;
}

int main(int argc, char * argv[])
{
    printf("\n");
    printf("%s client 1.0\n", APP_NAME);
    printf("\n");

    std::string app_name;
    std::string server_host;
    std::string username, password, database;
    std::string cmd, cmd_value;
    int32_t need_echo = 1;

    namespace options = boost::program_options;
    options::options_description desc("Command list");
    desc.add_options()
        ("help,h",          "usage info")
        ("host,s",          options::value<std::string>(&server_host)->default_value(DEFAULT_SERVER_HOST),
                            "server url: ([hostname]|[ip-address]):[port]")
        ("local,l",         options::value<std::string>(&server_host)->default_value(DEFAULT_LOCAL_IP),
                            "local url: ([hostname]|[ip-address]):[port]")
        ("username,u",      options::value<std::string>(&username)->default_value("root"),
                            "username = [root]")
        ("password,p",      options::value<std::string>(&password)->default_value("user1234"),
                            "password = [user1234]")
        ("database,d",      options::value<std::string>(&database)->default_value("topics"),
                            "database = [topics]")
        ("echo,e",          options::value<int32_t>(&need_echo)->default_value(1),
                            "whether the server need echo")
        ;

    // Parse the command line.
    options::variables_map args_map;
    try {
        options::store(options::parse_command_line(argc, argv, desc), args_map);
    }
    catch (const std::exception & ex) {
        std::cout << "Exception is: " << ex.what() << std::endl;
    }
    options::notify(args_map);

    app_name = get_app_name(argv[0]);

    // Option: help
    size_t nn = args_map.count("help");
    if (args_map.count("help") > 0) {
        print_usage(app_name, desc);
        exit(EXIT_FAILURE);
    }

    // Option: host
    std::string server_address;
    uint16_t server_port;
    if (args_map.count("host") > 0) {
        server_host = args_map["host"].as<std::string>();
    }
    std::cout << "host: " << server_host.c_str() << std::endl;
    bool has_seperator = parse_host_url(server_host, server_address, server_port, DEFAULT_SERVER_PORT);
    boost::asio::ip::address ip_address;
    ip_address.from_string(server_address);
    boost::asio::ip::tcp::endpoint server_endpoint(ip_address, server_port);
    if (!is_valid_ip_v4(server_address)) {
        std::cerr << "Error: ip address \"" << server_address.c_str() << "\" format is wrong." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!is_socket_port(server_port)) {
        std::cerr << "Error: port [" << server_port << "] number must be range in (0, 65535]." << std::endl;
        exit(EXIT_FAILURE);
    }

    clientConfig.remote = server_host;
    clientConfig.address = server_address;
    clientConfig.port = server_port;

    std::stringstream prefix_s; 
    prefix_s << APP_NAME << "-" << server_address << ":" << server_port << "> ";
    clientConfig.prefix = prefix_s.str();

    // Option: username
    if (args_map.count("username") > 0) {
        username = args_map["username"].as<std::string>();
    }
    std::cout << "username: " << username.c_str() << std::endl;
    if (username.size() <= 0) {
        std::cerr << "Error: username \"" << username.c_str() << "\" is empty." << std::endl;
        exit(EXIT_FAILURE);
    }

    clientConfig.username = username;

    // Option: password
    if (args_map.count("password") > 0) {
        password = args_map["password"].as<std::string>();
    }
    std::cout << "password: " << password.c_str() << std::endl;
    if (password.size() <= 0) {
        std::cerr << "Error: password \"" << password.c_str() << "\" is empty." << std::endl;
        exit(EXIT_FAILURE);
    }

    clientConfig.password = password;

    // Option: database
    if (args_map.count("database") > 0) {
        database = args_map["database"].as<std::string>();
    }
    std::cout << "database: " << database.c_str() << std::endl;
    if (database.size() <= 0) {
        std::cerr << "Error: database \"" << database.c_str() << "\" is empty." << std::endl;
        exit(EXIT_FAILURE);
    }

    clientConfig.database = database;

    printf("\n");

    net_packet_test();

    run_kvdb_client(server_address, server_port);

    bool exit = false;

    while (!exit) {
        printf("%s", clientConfig.prefix.c_str());
        unsigned char commands[1024];
        do {
#if defined(_WIN32)
            int cmd_len = scanf_s("%s", &commands, (int)(sizeof(commands) - 1));
#else
            int cmd_len = scanf("%s", &commands);
#endif
            if (cmd_len <= 0) continue;

            int first = 0, last = cmd_len - 1;
            unsigned char ch;

            // Find the first non space char position.
            while (ch = commands[first] != '\0') {
                if (likely(ch != ' ')) {
                    first++;
                    if (first >= cmd_len)
                        continue;
                }
                else {
                    break;
                }
            }

            // Find the last non space char position.
            while (last > 0) {
                ch = commands[last];
                if (likely(ch == ' ')) {
                    last--;
                }
                else {
                    break;
                }
            }

            commands[last] = '\0';

            unsigned char firstChar = commands[first];
            char * cmd = (char *)&commands[first];
            switch (firstChar) {
            case '\0':
                exit = true;
                break;
            case ' ':
                break;
            case 'b':
                if (strcmp(cmd, "bye") == 0) {
                    exit = true;
                }
                break;
            case 'q':
                if (strcmp(cmd, "quit") == 0) {
                    exit = true;
                }
                break;
            default:
                break;
            }
        } while (0);

        // Exit now ??
        if (exit) {
            printf("\n");
            printf("byebye.\n");
            printf("\n");
            break;
        }
        
        // Print the response.
        printf("Ok.\n");
    }

    ::system("pause");
    return 0;
}
