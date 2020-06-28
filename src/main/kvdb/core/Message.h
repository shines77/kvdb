
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

#include <vector>
#include <type_traits>

namespace kvdb {

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
    Message(uint16_t opcode = MessageType::Unknown, const char * data = nullptr) : body_(data) {
        this->header.info.sign = kShortSign;
        this->header.info.opcode = opcode;
    }

    Message(uint16_t opcode, uint8_t version, const char * data = nullptr) : body_(data) {
        this->header.info.sign = kShortSign;
        this->header.info.version = version;
        this->header.info.opcode = opcode;
    }

    virtual ~Message() {}

    static int32_t calcBodySize(uint32_t totalSize) {
        assert(totalSize >= kMsgHeaderSize);
        return int32_t(totalSize - kMsgHeaderSize);
    }

    static int32_t calcTotalSize(uint32_t bodySize) {
        return int32_t(bodySize + kMsgHeaderSize);
    }

    uint8_t flags() const { return this->header.flags(); }
    uint32_t bodySize() const { return this->header.bodySize(); }
    uint32_t totalSize() const { return this->header.totalSize(); }

    uint8_t sign() const { return this->header.sign(); }
    uint8_t version() const { return this->header.version(); }
    uint16_t opcode() const { return this->header.opcode(); }
    uint32_t args() const { /* Not implemented yet. */ return 0; }

    void setFlags(uint8_t flags) { this->header.setFlags(flags); }
    void setBodySize(uint32_t size) { this->header.setBodySize(size); }
    void setTotalSize(uint32_t size) { this->header.setTotalSize(size); }

    void setSign(uint8_t sign) { this->header.setSign(sign); }
    void setVersion(uint8_t version) { this->header.info.version = version; }
    void setOpcode(uint16_t opcode) { this->header.setOpcode(opcode); }
    void setArgs(uint32_t args) { /* Not implemented yet. */ }

    uint32_t sizeValue() const { return this->header.sizeValue(); }
    uint32_t infoValue() const { return this->header.infoValue(); }

    void setSizeValue(uint32_t value) { this->header.setSizeValue(value); }
    void setInfoValue(uint32_t value) { this->header.setInfoValue(value); }

    char * body() {
        return ((this->body_ == nullptr) ? ((char *)(void *)this + kMsgHeaderSize) : (char *)this->body_);
    }
    const char * body() const {
        return ((this->body_ == nullptr) ? ((const char *)(void *)this + kMsgHeaderSize) : this->body_);
    }

    void setBody(const char * body) { this->body_ = body; }

    MessageHeader & getHeader() { return const_cast<Message *>(this)->header; }
    const MessageHeader & getHeader() const { return const_cast<const Message *>(this)->header; }

    void setHeader(uint32_t bodySize, uint8_t sign = kShortSign) {
        this->setBodySize(bodySize);
        this->setSign(sign);
    }

    void setHeaderTotal(uint32_t totalSize, uint8_t sign = kShortSign) {
        this->setTotalSize(totalSize);
        this->setSign(sign);
    }

    void setHeader(uint16_t opcode, uint8_t version, uint32_t bodySize, uint8_t sign = kShortSign) {
        this->setBodySize(bodySize);

        this->setSign(sign);
        this->setOpcode(opcode);
        this->setVersion(version);
    }

    template <typename InputStreamTy>
    void readHeader(InputStreamTy & is) {
        this->setSizeValue(is.readUInt32());
        this->setInfoValue(is.readUInt32());
    }

    template <typename OutputStreamTy>
    void writeHeader(OutputStreamTy & os) {
        os.writeUInt32(this->sizeValue());
        os.writeUInt32(this->infoValue());
    }

    template <typename OutputStreamTy>
    void writeHeader(OutputStreamTy & os, uint32_t bodySize, uint8_t sign = kShortSign) {
        this->setHeader(bodySize, kShortSign);
        this->writeHeader(os);
    }

    template <typename OutputStreamTy>
    void writeHeaderTotal(OutputStreamTy & os, uint32_t totalSize, uint8_t sign = kShortSign) {
        this->setHeaderTotal(totalSize, kShortSign);
        this->writeHeader(os);
    }
};

template <typename T, uint32_t MsgVer = 0>
class BasicMessage : public Message {
public:
    static const uint8_t kMsgVer = MsgVer;

    BasicMessage(uint16_t opcode = MessageType::Unknown, const char * data = nullptr)
        : Message(opcode, T::kMsgVer, data) {
    }

    BasicMessage(uint16_t opcode, uint8_t version, const char * data = nullptr)
        : Message(opcode, version, data) {
    }

    virtual ~BasicMessage() {}

    template <typename OutputStreamTy = OutputStream>
    uint32_t prepareAll() {
        constexpr bool isOutputStream = std::is_same<OutputStreamTy, kvdb::OutputStream>::value;
        constexpr bool isOutputPacketStream = std::is_same<OutputStreamTy, kvdb::OutputPacketStream>::value;
        if (isOutputStream || isOutputPacketStream) {
            if (isOutputStream) {
                // Calculate the total require size.
                PrepareOutputStream preOS;
                preOS.skip(kMsgHeaderSize);
                T * pThis = static_cast<T *>(this);
                assert(pThis != nullptr);
                pThis->write(preOS);
                return (uint32_t)preOS.position();
            }
            else {
                // Calculate the total require size.
                PrepareOutputPacketStream preOS;
                preOS.skip(kMsgHeaderSize);
                T * pThis = static_cast<T *>(this);
                assert(pThis != nullptr);
                pThis->write(preOS);
                return (uint32_t)preOS.position();
            }
        }
        else {
            static_assert((isOutputStream || isOutputPacketStream),
                          "Error OutputStreamTy type.");
            return 0;
        }
    }

    template <typename OutputStreamTy = OutputStream>
    uint32_t prepareBody() {
        constexpr bool isOutputStream = std::is_same<OutputStreamTy, kvdb::OutputStream>::value;
        constexpr bool isOutputPacketStream = std::is_same<OutputStreamTy, kvdb::OutputPacketStream>::value;
        if (isOutputStream || isOutputPacketStream) {
            if (isOutputStream) {
                // Calculate the body require size.
                PrepareOutputStream preOS;
                T * pThis = static_cast<T *>(this);
                assert(pThis != nullptr);
                pThis->write(preOS);
                return (uint32_t)preOS.position();
            }
            else {
                // Calculate the body require size.
                PrepareOutputPacketStream preOS;
                T * pThis = static_cast<T *>(this);
                assert(pThis != nullptr);
                pThis->write(preOS);
                return (uint32_t)preOS.position();
            }
        }
        else {
            static_assert((isOutputStream || isOutputPacketStream),
                          "Error OutputStreamTy type.");
            return 0;
        }
    }

    template <typename OutputStreamTy>
    void prepare(OutputStreamTy & os, bool needPrepare) {
        // Need prepare stream space ?
        if (needPrepare && os.isMemoryStream()) {
            uint32_t totalSize = this->prepareAll<OutputStreamTy>();
            // Setting the message's body length
            this->header.setBodySize(totalSize - kMsgHeaderSize);
            os.inflate(totalSize);
        }
    }

    template <typename OutputStreamTy>
    void prepareBody(OutputStreamTy & os, bool needPrepare) {
        // Need prepare stream space ?
        if (needPrepare && os.isMemoryStream()) {
            uint32_t bodySize = this->prepareBody<OutputStreamTy>();
            // Setting the message's body length
            this->header.setBodySize(bodySize);
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
        readStatus = pThis->read(is);
        return readStatus;
    }

    template <typename InputStreamTy>
    int readFromBody(InputStreamTy & is) {
        int readStatus = ReadResult::Ok;

        T * pThis = static_cast<T *>(this);
        assert(pThis != nullptr);
        readStatus = pThis->read(is);
        return readStatus;
    }

    template <typename OutputStreamTy>
    void writeTo(OutputStreamTy & os, bool needPrepare = true) {
        this->prepare(os, needPrepare);

        typename OutputStreamTy::downcast_type & osb = os.downcastTo();
        Message::writeHeader(osb);

        T * pThis = static_cast<T *>(this);
        assert(pThis != nullptr);
        pThis->write(os);
    }

    template <typename OutputStreamTy>
    void writeBody(OutputStreamTy & os, bool needPrepare) {
        this->prepareBody(os, needPrepare);

        T * pThis = static_cast<T *>(this);
        assert(pThis != nullptr);
        pThis->write(os);
    }

    template <typename OutputStreamTy = OutputStream>
    uint32_t writeTo(std::vector<char> & buffer) {
        uint32_t totalSize = this->prepareAll<OutputStreamTy>();
        buffer.reserve(totalSize);

        OutputStreamTy os(buffer.data(), totalSize);
        this->writeHeaderTotal(os, totalSize);
        this->writeBody(os, false);

        return totalSize;
    }
};

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_H
