
#ifndef KVDB_CORE_MESSAGE_H
#define KVDB_CORE_MESSAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

namespace kvdb {

static const uint32_t kDefaultSignId = 2019082500;

struct MessageHeader {
    uint32_t sign;
    uint32_t type;
    uint32_t args;
    uint32_t length;

    MessageHeader() : sign(0), type(0), args(0), length(0) {}
    ~MessageHeader() {}
};

static const uint32_t kMsgHeaderSize = sizeof(MessageHeader);

#pragma warning (push)
#pragma warning (disable: 4200)

struct MessageData {
    MessageHeader header;
    char          data[0];
};

#pragma warning (pop)

struct MessageBody
{
    MessageHeader header;
    const char *  data;
};

class Message {
public:
    enum Type {
        Unknown = 0,

        LoginRequest,
        LoginResponse,

        HandShakeRequest,
        HandShakeResponse,

        ConnectRequest,
        ConnectResponse,

        QueryRequest,
        QueryResponse,

        DisconnectRequest,
        DisconnectResponse,

        LogoutRequest,
        LogoutResponse,

        Last
    };

    MessageHeader header;

protected:
    const char * data_;

public:
    Message(uint32_t type = Message::Unknown, const char * data = nullptr) : data_(data) {
        this->header.sign = kDefaultSignId;
        this->header.type = type;
    }

    virtual ~Message() {}

    uint32_t sign(uint32_t sign) const { return this->header.sign; }
    uint32_t messageType(uint32_t type) const { return this->header.type; }
    uint32_t bodyLength(uint32_t length) const { return this->header.length; }
    uint32_t args(uint32_t args) const { return this->header.args; }

    void setSign(uint32_t sign) { this->header.sign = sign; }
    void setMessageType(uint32_t type) { this->header.type = type; }
    void setBodyLength(uint32_t length) { this->header.length = length; }
    void setArgs(uint32_t args) { this->header.args = args; }

    char * data() { return (char *)this->data_; }
    const char * data() const { return this->data_; }

    void setData(const char * data) { this->data_ = data; }

    template <typename InputStreamTy>
    void readHeader(InputStreamTy & is) {
        this->header.sign   = is.readUInt32();
        this->header.type   = is.readUInt32();
        this->header.args   = is.readUInt32();
        this->header.length = is.readUInt32();
    }

    template <typename OutputStreamTy>
    void writeHeader(OutputStreamTy & os) {
        os.writeUInt32(this->header.sign);
        os.writeUInt32(this->header.type);
        os.writeUInt32(this->header.args);
        os.writeUInt32(this->header.length);
    }
};

template <typename T>
class BasicMessage : public Message {
public:
    BasicMessage(uint32_t type = Message::Unknown, const char * data = nullptr)
        : Message(type, data) {
    }

    virtual ~BasicMessage() {}

    uint32_t prepare() {
        // Reset the stream position and calculate the total require size.
        PrepareOutputPacketStream preOS;
        preOS.reset();
        preOS.next(kMsgHeaderSize);
        T * pThis = static_cast<T *>(this);
        if (pThis != nullptr) {
            pThis->writeBody(preOS);
        }
        return (uint32_t)preOS.position();
    }

    template <typename OutputStreamTy>
    void prepare(OutputStreamTy & os, bool needPrepare) {
        // Need prepare stream space ?
        if ((!needPrepare) && os.isMemoryStream()) {
            uint32_t totalSize = this->prepare();
            //os.reallocate(totalSize);
        }
    }

    template <typename InputStreamTy>
    int readFrom(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;

        Message::readHeader(is);

        T * pThis = static_cast<T *>(this);
        if (pThis != nullptr) {
            readStatus = pThis->readBody(is);
        }
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeTo(OutputStreamTy & os, bool needPrepare = true) {
        this->prepare(os, needPrepare);

        Message::writeHeader(os);

        T * pThis = static_cast<T *>(this);
        if (pThis != nullptr) {
            pThis->writeBody(os);
        }
    }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_H
