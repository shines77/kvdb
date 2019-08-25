
#ifndef KVDB_CORE_MESSAGES_H
#define KVDB_CORE_MESSAGES_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>

#include "kvdb/stream/InputPacketStream.h"
#include "kvdb/stream/OutputPacketStream.h"
#include "kvdb/stream/NetPacket.h"

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

    void skipToHeader(InputStream & os) {
        os.next(sizeof(PacketHeader));
    }

    void skipToHeader(InputPacketStream & os) {
        os.stream.next(sizeof(PacketHeader));
    }

    void skipToHeader(OutputStream & os) {
        os.next(sizeof(PacketHeader));
    }

    void skipToHeader(OutputPacketStream & os) {
        os.stream.next(sizeof(PacketHeader));
    }

    void writeTo(OutputPacketStream & os) {
        os.writeString(username);
        os.writeString(password);
        os.writeString(database);
    }

    int readFrom(InputPacketStream & is) {
        int readOk = ReadResult::Ok;
        readOk |= is.readString(username);
        readOk |= is.readString(password);
        readOk |= is.readString(database);
        return readOk;
    }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGES_H
