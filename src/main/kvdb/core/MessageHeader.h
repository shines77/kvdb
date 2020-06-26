
#ifndef KVDB_CORE_MESSAGE_HEADER_H
#define KVDB_CORE_MESSAGE_HEADER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

#include "kvdb/core/MessageType.h"
#include "kvdb/stream/ReadWriteResult.h"
#include "kvdb/stream/InputStream.h"
#include "kvdb/stream/OutputStream.h"

#define USE_INGO_ARGS   0

#if defined(USE_INGO_ARGS) && (USE_INGO_ARGS != 0)
#define MAKE_MSG_VERINFO(ver, args)     (((ver << 16) & 0xFFFF0000U) | (args & 0x0000FFFFU))
#define GET_MSG_VERSION(info)           ((info & 0xFFFF0000U) >> 16)
#define GET_MSG_ARGS(info)              (info & 0x0000FFFFU)
#else
#define MAKE_MSG_VERINFO(ver, args)     (ver & 0x000000FFU)
#define GET_MSG_VERSION(info)           (info & 0x000000FFU)
#define GET_MSG_ARGS(info)              ((info & 0xFFFF0000U) >> 16)
#endif

namespace kvdb {

static const uint32_t kDefaultSign = 2019082500;
static const uint32_t kShortSign = 77;

struct VersionInfo {
    uint16_t version;
    uint16_t args;

    VersionInfo() : version(0), args(0) {}
    explicit VersionInfo(uint32_t info) : version(GET_MSG_VERSION(info)), args(GET_MSG_ARGS(info)) {}
    VersionInfo(uint16_t ver, uint16_t _args) : version(ver), args(_args) {}
    ~VersionInfo() {}

    uint32_t value() const { return *reinterpret_cast<const uint32_t *>(this); }

    void setValue(uint32_t value) {
        *reinterpret_cast<uint32_t *>(this) = value;
    }

    VersionInfo & operator = (uint32_t value) {
        this->setValue(value);
        return *this;
    }

    operator uint32_t() {
        return this->value();
    }
};

struct SizeField {
    uint32_t flags: 8;
    uint32_t size: 24;

    SizeField(uint32_t _flags = 0, uint32_t _size = 0)
        : flags(_flags), size(_size) {
    }

    uint32_t value() const { return *((uint32_t *)this); }

    void setValue(uint32_t value) {
        SizeField * newValue = reinterpret_cast<SizeField *>(&value);
        flags = newValue->flags;
        size = newValue->size;
    }

    uint32_t getFlags() const { return this->flags; }
    uint32_t getSize() const { return this->size; }

    void setFlags(uint32_t flags) { this->flags = flags; }
    void setSize(uint32_t size) { this->size = size; }
};

struct InfoField {
    uint8_t sign: 8;
    uint8_t version: 8;
    uint16_t opcode: 16;

    InfoField(uint8_t _sign = 0, uint8_t _version = 0, uint16_t _opcode = 0)
        : sign(_sign), version(_version), opcode(_opcode) {
    }

    uint32_t value() const { return *((uint32_t *)this); }

    void setValue(uint32_t value) {
        InfoField * newValue = reinterpret_cast<InfoField *>(&value);
        sign = newValue->sign;
        version = newValue->version;
        opcode = newValue->opcode;
    }

    uint8_t getSign() const { return this->sign; }
    uint8_t getVersion() const { return this->version; }
    uint16_t getOpcode() const { return this->opcode; }

    void getInfo(uint8_t & sign, uint8_t & version, uint16_t & opcode) {
        sign = this->sign;
        version = this->version;
        opcode = this->opcode;
    }

    void setSign(uint8_t sign) { this->sign = sign; }
    void setVersion(uint8_t version) { this->version = version; }
    void setOpcode(uint16_t opcode) { this->opcode = opcode; }

    void setInfo(uint8_t sign, uint8_t version, uint16_t opcode) {
        this->sign = sign;
        this->version = version;
        this->opcode = opcode;
    }
};

struct InfoType {
    union InfoUnion {
        struct info_t {
            uint8_t  sign;
            uint8_t  version;
            uint16_t opcode;

            info_t(uint8_t _sign = 0, uint8_t _version = 0, uint16_t _opcode = 0)
                : sign(_sign), version(_version), opcode(_opcode) {
            }
        } val;

        uint32_t value;

        InfoUnion(uint32_t _value = 0) : value(_value) {}
        InfoUnion(uint8_t _sign, uint8_t _version, uint16_t _opcode)
            : val(_sign, _version, _opcode) {
        }
    } u;

    InfoType(uint32_t _value = 0)
        : u(_value) {
    }

    InfoType(uint8_t _sign, uint8_t _version, uint16_t _opcode)
        : u(0, 0, 0) {
    }

    uint8_t getSign() const { return u.val.sign; }
    uint8_t getVersion() const { return u.val.version; }
    uint16_t getOpcode() const { return u.val.opcode; }

    uint32_t value() const { return u.value; }

    void getInfo(uint8_t & sign, uint8_t & version, uint16_t & opcode) {
        sign = u.val.sign;
        version = u.val.version;
        opcode = u.val.opcode;
    }

    void setValue(uint32_t value) { u.value = value; }
    void setSign(uint32_t sign) { u.val.sign = sign; }
    void setVersion(uint32_t version) { u.val.version = version; }
    void setOpcode(uint32_t opcode) { u.val.opcode = opcode; }

    void setInfo(uint8_t sign, uint8_t version, uint16_t opcode) {
        u.val.sign = sign;
        u.val.version = version;
        u.val.opcode = opcode;
    }
};

struct MessageHeader {
    SizeField   size;
    InfoField   info;

    MessageHeader() {
        info.opcode = 0;
    }
    ~MessageHeader() {}

    uint8_t flags() const { return this->size.flags; }
    uint32_t bodySize() const { return this->size.size; }
    uint32_t totalSize() const { return (this->size.size + sizeof(MessageHeader)); }

    uint32_t sizeValue() const { return this->size.value(); }

    void setFlags(uint8_t flags) { this->size.setFlags(flags); }
    void setBodySize(uint32_t size) { this->size.setSize(size); }
    void setTotalSize(uint32_t size) {
        assert(size >= sizeof(MessageHeader));
        this->size.setSize(size - sizeof(MessageHeader));
    }

    void setSizeValue(uint32_t value) { this->size.setValue(value); }

    uint8_t sign() const { return this->info.getSign(); }
    uint8_t version() const { return this->info.getVersion(); }
    uint16_t opcode() const { return this->info.getOpcode(); }
    uint16_t args() const { return 0; }

    uint32_t infoValue() const { return this->info.value(); }

    void setSign(uint32_t sign) { this->info.setSign(sign); }
    void setVersion(uint32_t version) { this->info.setVersion(version); }
    void setOpcode(uint32_t opcode) { this->info.setOpcode(opcode); }
    void setArgs(uint32_t args) { /* Not implemented yet. */ }

    void setInfoValue(uint32_t value) { this->info.setValue(value); }

    void getInfo(uint8_t & sign, uint8_t & version, uint16_t & opcode) {
        this->info.getInfo(sign, version, opcode);
    }

    void setInfo(uint8_t sign, uint8_t version, uint16_t opcode) {
        this->info.setInfo(sign, version, opcode);
    }

    bool verifySign() const {
        return (this->sign() == kShortSign);
    }

    int readHeader(const char * data, std::size_t size) {
        assert(size >= sizeof(MessageHeader));
        InputStream is(data, size);
        this->setSizeValue(is.readUInt32());
        this->setInfoValue(is.readUInt32());
        return ReadResult::Ok;
    }

    template <std::size_t N>
    int readHeader(const char(&data)[N]) {
        assert(N >= sizeof(MessageHeader));
        InputStream is(data);
        this->setSizeValue(is.readUInt32());
        this->setInfoValue(is.readUInt32());
        return ReadResult::Ok;
    }

    void writeHeader(OutputStream & os) {
        os.writeUInt32(this->sizeValue());
        os.writeUInt32(this->infoValue());
    }

    static
    void writeHeader(OutputStream & os,
                     uint32_t sign, uint32_t type,
                     uint32_t info, uint32_t length) {
        os.writeUInt32(sign);
        os.writeUInt32(type);
        os.writeUInt32(info);
        os.writeUInt32(length);
    }

    void writeHeaderAndRestore(OutputStream & os) {
        char * savePos = os.current();
        os.reset();
        os.writeUInt32(this->sizeValue());
        os.writeUInt32(this->infoValue());
        os.setCurrent(savePos);
    }

    static
    void writeHeaderAndRestore(OutputStream & os,
                               uint8_t sign, uint16_t opcode,
                               uint8_t version) {
        MessageHeader header;
        assert(os.length() >= sizeof(MessageHeader));
        uint32_t bodySize = static_cast<uint32_t>(os.length() - sizeof(MessageHeader));
        header.setBodySize(bodySize);
        header.setSign(sign);
        header.setOpcode(opcode);
        header.setVersion(version);

        char * savePos = os.current();
        os.reset();
        header.writeHeader(os);
        os.setCurrent(savePos);
    }

    static
    void writeHeaderAndRestore(OutputStream & os,
                               uint8_t sign, uint16_t opcode,
                               uint8_t version, uint32_t bodySize) {
        MessageHeader header;
        header.setBodySize(bodySize);
        header.setSign(sign);
        header.setOpcode(opcode);
        header.setVersion(version);

        char * savePos = os.current();
        os.reset();
        header.writeHeader(os);
        os.setCurrent(savePos);
    }
};

static const uint32_t kMsgHeaderSize = sizeof(MessageHeader);

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_HEADER_H
