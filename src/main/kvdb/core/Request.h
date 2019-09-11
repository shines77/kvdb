
#ifndef KVDB_CORE_REQUEST_H
#define KVDB_CORE_REQUEST_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/core/Message.h"

namespace kvdb {

class IRequest : public Message {
public:
    //

protected:
    //

public:
    IRequest(uint32_t type = Message::Unknown, const char * data = nullptr)
        : Message(type, data) {
        //
    }

    virtual ~IRequest() {}
};

template <typename T>
class Request : public BasicMessage<T> {
public:
    //

protected:
    //

public:
    Request(uint32_t type = Message::Unknown, const char * data = nullptr)
        : BasicMessage(type, data) {
        //
    }

    virtual ~Request() {}
};

} // namespace kvdb

#endif // KVDB_CORE_REQUEST_H
