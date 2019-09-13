
#ifndef KVDB_CORE_MESSAGE_H
#define KVDB_CORE_MESSAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/core/MessageHeader.h"
#include "kvdb/stream/BasicStream.h"
#include "kvdb/stream/InputPacketStream.h"
#include "kvdb/stream/OutputPacketStream.h"

namespace kvdb {

static const uint32_t kDefaultSignId = 2019082500;

#pragma warning (push)
#pragma warning (disable: 4200)

struct MessageData {
    MessageHeader header;
    char          data[0];
};

#pragma warning (pop)

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
    const char * body_;

public:
    Message(uint32_t type = Message::Unknown, const char * data = nullptr) : body_(data) {
        this->header.sign = kDefaultSignId;
        this->header.type = type;
    }

    virtual ~Message() {}

    uint32_t sign() const { return this->header.sign; }
    uint32_t messageType() const { return this->header.type; }
    uint32_t bodyLength() const { return this->header.length; }
    uint32_t args() const { return this->header.args; }

    void setSign(uint32_t sign) { this->header.sign = sign; }
    void setMessageType(uint32_t type) { this->header.type = type; }
    void setBodyLength(uint32_t length) { this->header.length = length; }
    void setArgs(uint32_t args) { this->header.args = args; }

    char * body() { return (char *)this->body_; }
    const char * body() const { return this->body_; }

    void setBody(const char * body) { this->body_ = body; }

    MessageHeader & getHeader() const { return const_cast<Message *>(this)->header; }

    void setHeader(uint32_t sign, uint32_t length) {
        this->header.sign   = sign;
        this->header.length = length;
    }

    void setHeader(uint32_t sign, uint32_t args, uint32_t length) {
        this->header.sign   = sign;
        this->header.args   = args;
        this->header.length = length;
    }

    void setHeader(uint32_t sign, uint32_t type, uint32_t args, uint32_t length) {
        this->header.sign   = sign;
        this->header.type   = type;
        this->header.args   = args;
        this->header.length = length;
    }

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
        // Calculate the total require size.
        PrepareOutputPacketStream preOS;
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
        if (needPrepare && os.isMemoryStream()) {
            uint32_t totalSize = this->prepare();
            // Setting the message's body length
            this->header.length = totalSize - kMsgHeaderSize;
            os.inflate(totalSize);
        }
    }

    uint32_t prepareBody() {
        // Calculate the body require size.
        PrepareOutputPacketStream preOS;
        T * pThis = static_cast<T *>(this);
        if (pThis != nullptr) {
            pThis->writeBody(preOS);
        }
        return (uint32_t)preOS.position();
    }

    template <typename OutputStreamTy>
    void prepareBody(OutputStreamTy & os, bool needPrepare) {
        // Need prepare stream space ?
        if (needPrepare && os.isMemoryStream()) {
            uint32_t bodySize = this->prepareBody();
            // Setting the message's body length
            this->header.length = bodySize;
            os.inflate(bodySize);
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
    void writeBody(OutputStreamTy & os, bool needPrepare) {
        this->prepareBody(os, needPrepare);

        T * pThis = static_cast<T *>(this);
        if (pThis != nullptr) {
            pThis->writeBody(os);
        }
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
