
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

// String compare mode
#define STRING_COMPARE_STDC     0
#define STRING_COMPARE_U64      1
#define STRING_COMPARE_SSE42    2

#define STRING_COMPARE_MODE     STRING_COMPARE_SSE42

#include "kvdb/common/boost_asio_msvc.h"
#include <boost/asio.hpp>

#include "client/common.h"
#include "client/KvdbClient.h"
#include "client/KvdbClientApp.h"

#include "kvdb/all.h"

using namespace kvdb;
using namespace kvdb::client;

std::string kvdb::g_server_address;
std::string kvdb::g_server_port;

int main(int argc, char * argv[])
{
    KvdbClientApp app(argc, argv);
    int result = app.run();
    return result;
}
