
#include "server/RequestHandler.h"

#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

#include <kvdb/core/Message.h>
#include <kvdb/core/Messages.h>
#include <kvdb/jstd/StringRef.h>
#include <kvdb/server/ServerStatus.h>
#include <kvdb/stream/ParseResult.h>
#include <kvdb/stream/ParseStatus.h>
#include <kvdb/stream/InputStream.h>
#include <kvdb/stream/InputPacketStream.h>
#include <kvdb/stream/OutputStream.h>
#include <kvdb/stream/OutputPacketStream.h>

#include "server/Request.h"
#include "server/Response.h"

namespace kvdb {
namespace server {

RequestHandler::RequestHandler(const std::string & doc_root)
{
}

RequestHandler::~RequestHandler()
{
}

int RequestHandler::handleRequest(ConnectionContext & context,
                                  const Request & request,
                                  Response & response)
{
    InputPacketStream stream(request.data());
    PacketHeader header = request.header;

    if (header.msgLength > 0) {
        const char * first = stream.current();
        int result = 0;
        switch (header.msgType) {
        case Message::LoginRequest:
            result = handleLoginRequest(context, stream, response);
            break;

        case Message::HandShakeRequest:
            result = handleHandshakeRequest(context, stream, response);
            break;

        case Message::QueryRequest:
            result = handleQueryRequest(context, stream, response);
            break;

        default:
            // Unknown type message
            break;
        }

        const char * last = stream.current();
        if ((last - first) == (ptrdiff_t)header.msgLength) {
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
                                       InputPacketStream & stream,
                                       Response & response)
{
    std::string username, password, database;
    int result = stream.readString(username);
    if (result == ParseResult::OK) {
        int result = stream.readString(password);
        if (result == ParseResult::OK) {
            int result = stream.readString(database);
            if (result == ParseResult::OK) {
                OutputStream os;
                response.setSignId(kDefaultSignId);
                response.setMsgType(Message::LoginResponse);
                response.writeTo(os);

                LoginResponse loginResponse;
                OutputPacketStream os;
                loginResponse.writeTo(os);
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
                                           InputPacketStream & stream,
                                           Response & response)
{
    return ParseStatus::Success;
}

int RequestHandler::parseFirstQueryCommand(jstd::StringRef & cmd, const jstd::StringRef & qurey)
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
                                       InputPacketStream & stream,
                                       Response & response)
{
    jstd::StringRef qurey;
    int result = stream.readString(qurey);
    if (result == ParseResult::OK) {
        jstd::StringRef cmd;
        int result = parseFirstQueryCommand(cmd, qurey);
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
