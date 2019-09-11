
#include "server/RequestHandler.h"

#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

#include <kvdb/jstd/StringRef.h>
#include <kvdb/core/Message.h>
#include <kvdb/core/Request.h>
#include <kvdb/core/Response.h>
#include <kvdb/core/Messages.h>

#include <kvdb/server/ServerStatus.h>
#include <kvdb/stream/ParseResult.h>
#include <kvdb/stream/ParseStatus.h>
#include <kvdb/stream/InputStream.h>
#include <kvdb/stream/InputPacketStream.h>
#include <kvdb/stream/OutputStream.h>
#include <kvdb/stream/OutputPacketStream.h>

using namespace kvdb;

namespace kvdb {
namespace server {

RequestHandler::RequestHandler(const std::string & doc_root)
{
}

RequestHandler::~RequestHandler()
{
}

int RequestHandler::handleRequest(ConnectionContext & context,
                                  const Request & request)
{
    InputPacketStream stream(request.data());
    MessageHeader header = request.header;

    if (header.length > 0) {
        const char * first = stream.current();
        int result = 0;
        switch (header.type) {
        case Message::LoginRequest:
            result = handleLoginRequest(context, stream);
            break;

        case Message::HandShakeRequest:
            result = handleHandshakeRequest(context, stream);
            break;

        case Message::QueryRequest:
            result = handleQueryRequest(context, stream);
            break;

        default:
            // Unknown type message
            break;
        }

        const char * last = stream.current();
        if ((last - first) == (ptrdiff_t)header.length) {
            return ParseStatus::Success;
        }
        else {
            return ParseStatus::Error;
        }
    }
    else {
        return ParseStatus::TooSmall;
    }
}

int RequestHandler::handleLoginRequest(ConnectionContext & context,
                                       InputPacketStream & stream)
{
    std::string username, password, database;
    int result = stream.readString(username);
    if (result == ParseResult::OK) {
        int result = stream.readString(password);
        if (result == ParseResult::OK) {
            int result = stream.readString(database);
            if (result == ParseResult::OK) {
                LoginResponse response;
                PrepareOutputPacketStream preOS;
                uint32_t msgLength = response.prepare(preOS);
                OutputPacketStream os;
                os.writeHeader(kDefaultSignId, Message::LoginResponse, msgLength, 3);
                response.writeTo(os);

                return ParseStatus::Success;
            }
        }
    }
    else {
        //
    }
    return ParseStatus::Failed;
}

int RequestHandler::handleHandshakeRequest(ConnectionContext & context,
                                           InputPacketStream & stream)
{
    return ParseStatus::Success;
}

int RequestHandler::parseQueryRequestFirstCmd(jstd::StringRef & cmd, const jstd::StringRef & qurey)
{
    size_t first = 0, last;
    size_t i;
    for (i = 0; i < qurey.size(); ++i) {
        if (qurey[i] != ' ') {
            break;
        }
        first++;
    }

    last = first;
    for ( ; i < qurey.size(); ++i) {
        if (qurey[i] != ' ') {
            last++;
        }
        else {
            break;
        }
    }

    cmd.set_data(qurey.data(), first, last);
    return (int)cmd.size();
}

int RequestHandler::handleQueryRequest(ConnectionContext & context,
                                       InputPacketStream & stream)
{
    jstd::StringRef qurey;
    int result = stream.readString(qurey);
    if (result == ParseResult::OK) {
        jstd::StringRef cmd;
        int result = parseQueryRequestFirstCmd(cmd, qurey);
        return (result > 0) ? ParseStatus::Success : ParseStatus::Error;
    }
    else {
        //
    }
    return ParseStatus::Failed;
}

int RequestHandler::handleRequestData(const char * data)
{
    InputStream stream(data);
    bool isEndOf = false;
    while (!isEndOf) {
        uint8_t type = stream.getType();
        switch (type) {
        case DataType::EndOf:
            {
                stream.next();
                uint8_t endOfMark = stream.getUInt8();
                if (endOfMark == '\0') {
                    isEndOf = true;
                }
                stream.next();
            }
            break;

        case DataType::Bool:
            break;

        default:
            // Unknown errors
            break;
        }
    }
    return 0;
}

} // namespace server
} // namespace kvdb
