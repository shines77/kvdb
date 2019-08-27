
#ifndef KVDB_CORE_MESSAGE_H
#define KVDB_CORE_MESSAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace kvdb {

class Message {
public:
    enum Type {
        Unknown,

        LoginRequest,
        LoginResponse,

        HandShakeRequest,
        HandShakeResponse,

        ConnectRequest,
        ConnectResponse,

        QueryRequest,
        QueryResponse,

        LogoutRequest,
        LogoutResponse,

        Last
    };

private:
    //

public:
    Message() {}
    ~Message() {}
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_H
