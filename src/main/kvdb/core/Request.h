
#ifndef KVDB_CORE_REQUEST_H
#define KVDB_CORE_REQUEST_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/core/Message.h"

namespace kvdb {

class Request : public Message {
public:
    //

protected:
    const char * data_;

public:
    Request(const char * data = nullptr) : data_(data) {}
    ~Request() {}

    char * data() { return (char *)this->data_; }
    const char * data() const { return this->data_; }
};

} // namespace kvdb

#endif // KVDB_CORE_REQUEST_H
