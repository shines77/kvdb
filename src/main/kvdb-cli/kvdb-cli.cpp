
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

#include "client/common/boost_asio_msvc.h"
#include <boost/asio.hpp>

#include "client/common.h"
#include "client/common/cmd_utils.h"

#include "kvdb/all.h"

#include <boost/program_options.hpp>

using namespace kvdb;

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

#define DEFAULT_SERVER_HOST     DEFAULT_SERVER_ADDRESS":"DEFAULT_SERVER_PORT_S

#define CLIENT_PREFIX           APP_NAME"-"DEFAULT_SERVER_HOST"> "

struct KvdbClientConfig {    
    std::string host;
    std::string address;
    uint16_t port;

    std::string username;
    std::string password;
    std::string database;

    std::string prefix;

    KvdbClientConfig() : port(0) {}

    KvdbClientConfig(const std::string & _host)
        : host(_host), port(0) {
        //
    }

    KvdbClientConfig(const std::string & _address, uint16_t _port)
        : address(_address), port(_port) {
        //
    }
};

KvdbClientConfig clientConfig;

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
              << "  " << app_name.c_str()      << " --host=<host> --username=<username> --password=<password>" << std::endl
              << "  " << leader_spaces.c_str() << " --database=<database> [--echo=[0,1]]" << std::endl
              << std::endl
              << "For example: " << std::endl << std::endl
              << "  " << app_name.c_str()      << " --host=" << DEFAULT_SERVER_HOST << " --username=root --password=user1234" << std::endl
              << "  " << leader_spaces.c_str() << " --database=table1 --echo=0" << std::endl
              << std::endl
              << "  " << app_name.c_str() << " -s " << DEFAULT_SERVER_HOST << " -u root -p user1234 -d table1 -e 0" << std::endl;
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

    clientConfig.host = server_host;
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
