
#ifndef KVDB_CORE_MESSAGE_H
#define KVDB_CORE_MESSAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/core/MessageType.h"
#include "kvdb/core/MessageHeader.h"
#include "kvdb/stream/BasicStream.h"
#include "kvdb/stream/InputPacketStream.h"
#include "kvdb/stream/OutputPacketStream.h"

namespace kvdb {

static const uint32_t kDefaultSign = 2019082500;
static const uint32_t kShortSign = 77;

#pragma warning (push)
#pragma warning (disable: 4200)

struct MessageData {
    MessageHeader header;
    char          data[0];
};

#pragma warning (pop)

class Message {
public:
    MessageHeader header;

protected:
    const char * body_;

public:
    Message(uint32_t opcode = MessageType::Unknown, const char * data = nullptr) : body_(data) {
        this->header.info.sign = kShortSign;
        this->header.info.opcode = opcode;
    }

    virtual ~Message() {}

    uint8_t flags() const { return this->header.flags(); }
    uint32_t length() const { return this->header.length(); }
    uint32_t bodyLength() const { return (this->header.length() - sizeof(MessageHeader)); }

    uint8_t sign() const { return this->header.sign(); }
    uint8_t version() const { return this->header.version(); }
    uint16_t opcode() const { return this->header.opcode(); }
    uint32_t args() const { /* Not implemented yet. */ return 0; }

    uint32_t lenValue() const { return this->header.lenValue(); }
    void setLenValue(uint32_t value) { this->header.setLenValue(value); }

    void setFlags(uint8_t flags) { this->header.setFlags(flags); }
    void setLength(uint32_t length) { this->header.setLength(length); }
    void setBodyLength(uint32_t length) { this->header.setLength(length + sizeof(MessageHeader)); }

    void setSign(uint8_t sign) { this->header.setSign(sign); }
    void setVersion(uint8_t version) { this->header.info.version = version; }
    void setOpcode(uint16_t opcode) { this->header.setOpcode(opcode); }
    void setArgs(uint32_t args) { /* Not implemented yet. */ }

    uint32_t infoValue() const { return this->header.infoValue(); }
    void setInfoValue(uint32_t value) { this->header.setInfoValue(value); }

    char * body() { return (char *)this->body_; }
    const char * body() const { return this->body_; }

    void setBody(const char * body) { this->body_ = body; }

    MessageHeader & getHeader() const { return const_cast<Message *>(this)->header; }

    void setHeader(uint8_t sign, uint32_t length) {
        this->setLength(length);
        this->setSign(sign);
    }

    void setHeader(uint8_t sign, uint16_t opcode, uint32_t length) {
        this->setLength(length);

        this->setSign(sign);
        this->setOpcode(opcode);
    }

    void setHeader(uint8_t sign, uint8_t version, uint32_t args, uint32_t length) {
        this->setLength(length);

        this->setSign(sign);
        this->setVersion(version);
    }

    void setHeader(uint8_t sign, uint16_t opcode, uint8_t version, uint32_t args, uint32_t length) {
        this->setLength(length);

        this->setSign(sign);
        this->setVersion(version);
        this->setOpcode(opcode);
    }

    template <typename InputStreamTy>
    void readHeader(InputStreamTy & is) {
        this->setLenValue(is.readUInt32());
        this->setInfoValue(is.readUInt32());
    }

    template <typename OutputStreamTy>
    void writeHeader(OutputStreamTy & os) {
        os.writeUInt32(this->lenValue());
        os.writeUInt32(this->infoValue());
    }
};

template <typename T>
class BasicMessage : public Message {
public:
    BasicMessage(uint32_t opcode = MessageType::Unknown, const char * data = nullptr)
        : Message(opcode, data) {
        Message::setVersion(T::kVerInfo);
    }

    virtual ~BasicMessage() {}

    uint32_t prepare() {
        // Calculate the total require size.
        PrepareOutputPacketStream preOS;
        preOS.next(kMsgHeaderSize);
        T * pThis = static_cast<T *>(this);
        assert(pThis != nullptr);
        pThis->writeBody(preOS);
        return (uint32_t)preOS.position();
    }

    template <typename OutputStreamTy>
    void prepare(OutputStreamTy & os, bool needPrepare) {
        // Need prepare stream space ?
        if (needPrepare && os.isMemoryStream()) {
            uint32_t totalSize = this->prepare();
            // Setting the message's body length
            this->header.setLength(totalSize - kMsgHeaderSize);
            os.inflate(totalSize);
        }
    }

    uint32_t prepareBody() {
        // Calculate the body require size.
        PrepareOutputPacketStream preOS;
        T * pThis = static_cast<T *>(this);
        assert(pThis != nullptr);
        pThis->writeBody(preOS);
        return (uint32_t)preOS.position();
    }

    template <typename OutputStreamTy>
    void prepareBody(OutputStreamTy & os, bool needPrepare) {
        // Need prepare stream space ?
        if (needPrepare && os.isMemoryStream()) {
            uint32_t bodySize = this->prepareBody();
            // Setting the message's body length
            this->header.setLength(bodySize);
            os.inflate(bodySize);
        }
    }

    template <typename InputStreamTy>
    int readFrom(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;

        typename InputStreamTy::base_type & isb = is.downcastTo();
        Message::readHeader(isb);

        T * pThis = static_cast<T *>(this);
        assert(pThis != nullptr);
        readStatus = pThis->readBody(is);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os, bool needPrepare) {
        this->prepareBody(os, needPrepare);

        T * pThis = static_cast<T *>(this);
        assert(pThis != nullptr);
        pThis->writeBody(os);
    }

    template <typename OutputStreamTy>
    void writeTo(OutputStreamTy & os, bool needPrepare = true) {
        this->prepare(os, needPrepare);

        typename OutputStreamTy::base_type & osb = os.downcastTo();
        Message::writeHeader(osb);

        T * pThis = static_cast<T *>(this);
        assert(pThis != nullptr);
        pThis->writeBody(os);
    }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_H
