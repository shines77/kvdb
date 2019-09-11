
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
    const char * data_;

public:
    Response(const char * data = nullptr) : data_(data) {}
    ~Response() {}

    char * data() { return (char *)this->data_; }
    const char * data() const { return this->data_; }
};

} // namespace kvdb

#endif // KVDB_CORE_RESPONSE_H
