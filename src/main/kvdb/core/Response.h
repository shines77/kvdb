
#ifndef KVDB_CORE_RESPONSE_H
#define KVDB_CORE_RESPONSE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/core/Message.h"

namespace kvdb {

class Response : public Message {
public:
    //

protected:
    //

public:
    Response(uint32_t type = Message::Unknown, const char * data = nullptr)
        : Message(type, data) {
        //
    }

    virtual ~Response() {}
};

} // namespace kvdb

#endif // KVDB_CORE_RESPONSE_H