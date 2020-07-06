
#include "client/HandleClientResponse.h"
#include "client/ClientContext.h"
#include "client/KvdbClient.h"

using namespace kvdb;
using namespace kvdb::client;

namespace kvdb {
namespace client {

int stopKvdbClient(ClientContext & context)
{
    std::cout << "KvdbClient::stopKvdbClient()" << std::endl;
    std::cout << std::endl;

    if (context.client != nullptr) {
        context.client->stop();
        return ParseStatus::Success;
    }

    return ParseStatus::Failed;
}

int startReadSomeKvdbClient(ClientContext & context)
{
    std::cout << "KvdbClient::kvdbClientStartReadSome()" << std::endl;
    std::cout << std::endl;

    if (context.client != nullptr) {
        context.client->start_read();
        return ParseStatus::Success;
    }

    return ParseStatus::Failed;
}

} // namespace client
} // namespace kvdb
