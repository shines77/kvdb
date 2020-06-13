
#ifndef KVDB_CORE_MESSAGE_HEADER_H
#define KVDB_CORE_MESSAGE_HEADER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

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

struct ReadResult {
    enum Type {
        Failed = -1,
        Ok = 0,
        Last
    };
};

struct WriteResult {
    enum Type {
        Failed = -1,
        Ok = 0,
        Last
    };
};

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

struct LenField {
    uint8_t flags: 8;
    uint32_t length: 24;

    LenField(uint8_t _flags = 0, uint32_t _length = 0)
        : flags(_flags), length(_length) {
    }

    uint32_t value() const { return *((uint32_t *)this); }

    void setValue(uint32_t value) {
        LenField * newValue = reinterpret_cast<LenField *>(&value);
        flags = newValue->flags;
        length = newValue->length;
    }

    uint8_t getFlags() const { return this->flags; }
    uint32_t getLength() const { return this->length; }

    void setFlags(uint8_t flags) { this->flags = flags; }
    void setLength(uint32_t length) { this->length = length; }
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
    LenField    len;
    InfoField   info;

    MessageHeader() {
        info.opcode = 0;
    }
    ~MessageHeader() {}

    uint8_t flags() const { return this->len.flags; }
    uint32_t length() const { return this->len.length; }

    uint32_t totalLength() const { return (this->len.length + sizeof(MessageHeader)); }

    uint32_t lenValue() const { return this->len.value(); }

    void setFlags(uint8_t flags) { this->len.setFlags(flags); }
    void setLength(uint32_t length) { this->len.setLength(length); }

    void setLenValue(uint32_t value) { this->len.setValue(value); }

    uint8_t sign() const { return this->info.getSign(); }
    uint8_t version() const { return this->info.getVersion(); }
    uint16_t opcode() const { return this->info.getOpcode(); }
    uint16_t args() const { return 0; }

    uint32_t infoValue() const { return this->info.value(); }

    void getInfo(uint8_t & sign, uint8_t & version, uint16_t & opcode) {
        this->info.getInfo(sign, version, opcode);
    }

    void setSign(uint32_t sign) { this->info.setSign(sign); }
    void setVersion(uint32_t version) { this->info.setVersion(version); }
    void setOpcode(uint32_t opcode) { this->info.setOpcode(opcode); }
    void setArgs(uint32_t args) { /* Not implemented yet. */ }

    void setInfoValue(uint32_t value) { this->info.setValue(value); }

    void setInfo(uint8_t sign, uint8_t version, uint16_t opcode) {
        this->info.setInfo(sign, version, opcode);
    }
};

static const uint32_t kMsgHeaderSize = sizeof(MessageHeader);

} // namespace kvdb

#endif // KVDB_CORE_MESSAGE_HEADER_H
