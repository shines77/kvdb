
#ifndef KVDB_STREAM_VARIANT_H
#define KVDB_STREAM_VARIANT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>

#include "kvdb/core/DataType.h"

namespace kvdb {

class Variant {
public:
    union Value {
        bool        b;
        int8_t      i8;
        uint8_t     u8;
        int16_t     i16;
        uint16_t    u16;
        int32_t     i32;
        uint32_t    u32;
        int64_t     i64;
        uint64_t    u64;
        float       f;
        double      d;
        void *      ptr;
    };

    uint32_t type;
    Value value;

    explicit Variant(int type = DataType::Unknown) : type(type), value({int64_t(0)}) {}
    Variant(const Variant & src) : type(src.type), value(src.value) {}
    ~Variant() {}

    Variant & operator = (const Variant & src) {
        this->type = src.type;
        this->value = src.value;
        return *this;
    }

    Variant & operator = (bool value) {
        this->type = DataType::Bool;
        this->value.b = value;
        return *this;
    }

    Variant & operator = (int8_t value) {
        this->type = DataType::Int8;
        this->value.i8 = value;
        return *this;
    }

    Variant & operator = (uint8_t value) {
        this->type = DataType::UInt8;
        this->value.u8 = value;
        return *this;
    }

    Variant & operator = (int16_t value) {
        this->type = DataType::Int16;
        this->value.i16 = value;
        return *this;
    }

    Variant & operator = (uint16_t value) {
        this->type = DataType::UInt16;
        this->value.u16 = value;
        return *this;
    }

    Variant & operator = (int32_t value) {
        this->type = DataType::Int32;
        this->value.i32 = value;
        return *this;
    }

    Variant & operator = (uint32_t value) {
        this->type = DataType::UInt32;
        this->value.u32 = value;
        return *this;
    }

    Variant & operator = (int64_t value) {
        this->type = DataType::Int64;
        this->value.i64 = value;
        return *this;
    }

    Variant & operator = (uint64_t value) {
        this->type = DataType::UInt64;
        this->value.u64 = value;
        return *this;
    }

    Variant & operator = (void * value) {
        this->type = DataType::Pointer;
        this->value.ptr = value;
        return *this;
    }

    Variant & operator = (float value) {
        this->type = DataType::Float;
        this->value.f = value;
        return *this;
    }

    Variant & operator = (double value) {
        this->type = DataType::Double;
        this->value.d = value;
        return *this;
    }

    uint32_t getType() const { return this->type; }

    void setType(uint32_t type) {
        this->type = type;
    }

    Value getValue() const {
        return this->value;
    }

    bool      getBool()   const { return this->value.b;   }
    int8_t    getInt8()   const { return this->value.i8;  }
    uint8_t   getUInt8()  const { return this->value.u8;  }
    int16_t   getInt16()  const { return this->value.i16; }
    uint16_t  getUInt16() const { return this->value.u16; }
    int32_t   getInt32()  const { return this->value.i32; }
    uint32_t  getUInt32() const { return this->value.u32; }
    int64_t   getInt64()  const { return this->value.i64; }
    uint64_t  getUInt64() const { return this->value.u64; }

    void *    getPointer() const { return this->value.ptr; }

    float     getFloat()  const { return this->value.f; }
    double    getDouble() const { return this->value.d; }

    void setValue(const Variant & src) {
        this->type = src.getType();
        this->value = src.getValue();
    }

    void setBool(bool value) {
        this->type = DataType::Bool;
        this->value.b = value;
    }

    void setInt8(int8_t value) {
        this->type = DataType::Int8;
        this->value.i8 = value;
    }

    void setUInt8(uint8_t value) {
        this->type = DataType::UInt8;
        this->value.u8 = value;
    }

    void setInt16(int16_t value) {
        this->type = DataType::Int16;
        this->value.i16 = value;
    }

    void setUInt16(uint16_t value) {
        this->type = DataType::UInt16;
        this->value.u16 = value;
    }

    void setInt32(int32_t value) {
        this->type = DataType::Int32;
        this->value.i32 = value;
    }

    void setUInt32(uint32_t value) {
        this->type = DataType::UInt32;
        this->value.u32 = value;
    }

    void setInt64(int64_t value) {
        this->type = DataType::Int64;
        this->value.i64 = value;
    }

    void setUInt64(uint64_t value) {
        this->type = DataType::UInt64;
        this->value.u64 = value;
    }

    void setPointer(void * value) {
        this->type = DataType::Pointer;
        this->value.ptr = value;
    }

    void setFloat(float value) {
        this->type = DataType::Float;
        this->value.f = value;
    }

    void setDouble(double value) {
        this->type = DataType::Double;
        this->value.d = value;
    }
};

} // namespace kvdb

#endif // KVDB_STREAM_VARIANT_H
