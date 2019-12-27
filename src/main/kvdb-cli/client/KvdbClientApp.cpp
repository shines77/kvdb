
#include "client/KvdbClientApp.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <string>

#include "client/common.h"
#include "client/KvdbClient.h"

#include "kvdb/common/cmd_utils.h"
#include "kvdb/common/boost_asio_msvc.h"
#include <boost/asio.hpp>

#include "kvdb/all.h"

namespace options = boost::program_options;

namespace kvdb {
namespace client {

KvdbClientConfig KvdbClientApp::client_config;

static
void make_spaces(std::string & spaces, std::size_t size)
{
    spaces = "";
    for (std::size_t i = 0; i < size; ++i)
        spaces += " ";
}

void KvdbClientApp::initialize()
{
    // Initialize application data before main()
    app_name = get_app_name(argv_[0]);
}

void KvdbClientApp::finalize()
{
    // Release application data after main()
}

void KvdbClientApp::welcome()
{
    printf("\n");
    printf("%s client 1.0\n", APP_NAME);
    printf("\n");
}

void KvdbClientApp::init_options()
{
    options_desc.add_options()
        ("help,h",          "usage info")
        ("host,s",          options::value<std::string>(&server_host)->default_value(DEFAULT_SERVER_HOST),
                            "server url: ([hostname]|[ip-address]):[port]")
        ("local,l",         options::value<std::string>(&local_address)->default_value(DEFAULT_LOCAL_IP),
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
}

void KvdbClientApp::print_usage()
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

int KvdbClientApp::parse_args(int argc, char * argv[])
{
    // Parse the command line.
    options::variables_map args_map;
    try {
        options::store(options::parse_command_line(argc, argv, options_desc), args_map);
    }
    catch (const std::exception & ex) {
        std::cout << "Exception is: " << ex.what() << std::endl;
    }
    options::notify(args_map);

    // Option: help
    size_t nn = args_map.count("help");
    if (args_map.count("help") > 0) {
        this->print_usage();
        return EXIT_FAILURE;
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
        return EXIT_FAILURE;
    }
    if (!is_socket_port(server_port)) {
        std::cerr << "Error: port [" << server_port << "] number must be range in (0, 65535]." << std::endl;
        return EXIT_FAILURE;
    }

    client_config.remote = server_host;
    client_config.address = server_address;
    client_config.port = server_port;

    std::stringstream prefix_s; 
    prefix_s << APP_NAME << "-" << server_address << ":" << server_port << "> ";
    client_config.prefix = prefix_s.str();

    // Option: username
    if (args_map.count("username") > 0) {
        username = args_map["username"].as<std::string>();
    }
    std::cout << "username: " << username.c_str() << std::endl;
    if (username.size() <= 0) {
        std::cerr << "Error: username \"" << username.c_str() << "\" is empty." << std::endl;
        return EXIT_FAILURE;
    }

    client_config.username = username;

    // Option: password
    if (args_map.count("password") > 0) {
        password = args_map["password"].as<std::string>();
    }
    std::cout << "password: " << password.c_str() << std::endl;
    if (password.size() <= 0) {
        std::cerr << "Error: password \"" << password.c_str() << "\" is empty." << std::endl;
        return EXIT_FAILURE;
    }

    client_config.password = password;

    // Option: database
    if (args_map.count("database") > 0) {
        database = args_map["database"].as<std::string>();
    }
    std::cout << "database: " << database.c_str() << std::endl;
    if (database.size() <= 0) {
        std::cerr << "Error: database \"" << database.c_str() << "\" is empty." << std::endl;
        return EXIT_FAILURE;
    }

    client_config.database = database;

    return EXIT_SUCCESS;
}

int KvdbClientApp::parse_args()
{
    return this->parse_args(argc_, argv_);
}

void KvdbClientApp::net_packet_test()
{
    char packetBuf[4096];

    NetPacket packet;
    OutputStream ostream(packetBuf);
    ostream.setBuffer(packetBuf);
    packet.writeTo(ostream);

    ptrdiff_t length = ostream.length();

    InputStream istream(packetBuf);
    int count = packet.readFrom(istream);
}

void KvdbClientApp::run_client(const std::string & address, uint16_t port)
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

        client.start();

        /*
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
        //*/

        client.join();
    }
    catch (const std::exception & ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
}

int KvdbClientApp::run_shell(int argc, char * argv[])
{
    bool exit = false;

    while (!exit) {
        printf("%s", client_config.prefix.c_str());
        unsigned char commands[1024];
        do {
#if defined(_WIN32)
            int cmd_len = scanf_s("%s", &commands[0], (int)(sizeof(commands) - 1));
#else
            int cmd_len = scanf("%s", &commands[0]);
#endif
            if (cmd_len <= 0) continue;

            int first = 0, last = cmd_len - 1;
            unsigned char ch;

            // Find the first non space char position.
            while ((ch = commands[first]) != '\0') {
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

#ifdef _WIN32
    ::system("pause");
#endif
    return 0;
}

int KvdbClientApp::main(int argc, char * argv[])
{
    int result;
    this->init_options();
    result = this->parse_args(argc, argv);

    printf("\n");

    if (result == EXIT_SUCCESS) {
        this->net_packet_test();
        this->run_client(client_config.address, client_config.port);

        result = this->run_shell(argc, argv);
    }
    else {
        ::exit(EXIT_FAILURE);
    }

    return result;
}

int KvdbClientApp::main()
{
    return this->main(argc_, argv_);
}

} // namespace client
} // namespace kvdb
