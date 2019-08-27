
#ifndef KVDB_CORE_MESSAGES_H
#define KVDB_CORE_MESSAGES_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>

#include "kvdb/stream/Packet.h"
#include "kvdb/stream/InputPacketStream.h"
#include "kvdb/stream/OutputPacketStream.h"

//#include "kvdb/stream/PrepareInputPacketStream.h"
#include "kvdb/stream/PrepareOutputPacketStream.h"

namespace kvdb {

struct LoginRequest {
    std::string username;
    std::string password;
    std::string database;

    LoginRequest() {}
    LoginRequest(std::string _username, std::string _password, std::string _database)
        : username(_username), password(_password), database(_database) {
    }
    ~LoginRequest() {}

    size_t prepare() {
        PrepareOutputPacketStream os;
        os.writeString(username);
        os.writeString(password);
        os.writeString(database);
        return os.requireSize();
    }

    void writeTo(OutputPacketStream & os) {
        os.writeString(username);
        os.writeString(password);
        os.writeString(database);
    }

    int readFrom(InputPacketStream & is) {
        int readStatus = ReadResult::Ok;
        readStatus |= is.readString(username);
        readStatus |= is.readString(password);
        readStatus |= is.readString(database);
        return readStatus;
    }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGES_H
