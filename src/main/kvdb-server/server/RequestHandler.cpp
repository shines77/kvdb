
#include "server/RequestHandler.h"

#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

#include <kvdb/jstd/StringRef.h>
#include <kvdb/core/Message.h>
#include <kvdb/core/Request.h>
#include <kvdb/core/Response.h>
#include <kvdb/core/MessageDefine.h>

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

int RequestHandler::handleLoginRequest(ConnectionContext & context,
                                       InputPacketStream & is,
                                       OutputPacketStream & os)
{
    std::string username, password, database;
    int result = is.readString(username);
    if (result == ParseResult::OK) {
        int result = is.readString(password);
        if (result == ParseResult::OK) {
            int result = is.readString(database);
            if (result == ParseResult::OK) {
                LoginResponse_v0 response;
                response.setSign(kShortSign);
                response.setBodySize(0);
                response.iStatusCode = 0;
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
                                           InputPacketStream & is,
                                           OutputPacketStream & os)
{
    uint32_t iVersion;
    bool result = is.readUInt32(iVersion);
    if (result) {
        HandShakeResponse_v0 response;
        response.setSign(kShortSign);
        response.setBodySize(0);
        response.iStatusCode = 0;
        response.writeTo(os);

        return ParseStatus::Success;
    }
    
    return ParseStatus::Failed;
}

int RequestHandler::handleConnectRequest(ConnectionContext & context,
                                         InputPacketStream & is,
                                         OutputPacketStream & os)
{
    uint32_t iVersion;
    bool result = is.readUInt32(iVersion);
    if (result) {
        ConnectResponse_v0 response;
        response.setSign(kShortSign);
        response.setBodySize(0);
        response.iStatusCode = 0;
        response.writeTo(os);

        return ParseStatus::Success;
    }

    return ParseStatus::Failed;
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
                                       InputPacketStream & is,
                                       OutputPacketStream & os)
{
    jstd::StringRef qurey;
    int result = is.readString(qurey);
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

int RequestHandler::handleRequestData(const char * data, size_t size)
{
    InputStream stream(data, size);
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

int RequestHandler::handleRequest(ConnectionContext & context,
                                  const IRequest & request,
                                  OutputPacketStream & os)
{
    InputPacketStream stream(request.body(), request.bodySize());
    MessageHeader header = request.header;

    if (header.bodySize() > 0) {
        const char * first = stream.current();
        int result = 0;
        switch (header.opcode()) {
        case MessageType::LoginRequest:
            result = handleLoginRequest(context, stream, os);
            break;

        case MessageType::HandShakeRequest:
            result = handleHandshakeRequest(context, stream, os);
            break;

        case MessageType::ConnectRequest:
            result = handleConnectRequest(context, stream, os);
            break;

        case MessageType::QueryRequest:
            result = handleQueryRequest(context, stream, os);
            break;

        default:
            // Unknown opcode message
            break;
        }

        const char * last = stream.current();
        if ((last - first) == (ptrdiff_t)header.bodySize()) {
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

} // namespace server
} // namespace kvdb
