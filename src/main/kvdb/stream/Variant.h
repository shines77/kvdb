
#ifndef KVDB_STREAM_VARIANT_H
#define KVDB_STREAM_VARIANT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>

#include "kvdb/stream/DataType.h"

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

private:
    uint32_t type_;
    Value value_;

public:
    explicit Variant(int type = DataType::Unknown) : type_(type), value_({int64_t(0)}) {}
    Variant(const Variant & src) : type_(src.type_), value_(src.value_) {}
    ~Variant() {}

    Variant & operator = (const Variant & src) {
        type_ = src.type_;
        value_ = src.value_;
        return *this;
    }

    Variant & operator = (bool value) {
        type_ = DataType::Bool;
        value_.b = value;
        return *this;
    }

    Variant & operator = (int8_t value) {
        type_ = DataType::Int8;
        value_.i8 = value;
        return *this;
    }

    Variant & operator = (uint8_t value) {
        type_ = DataType::UInt8;
        value_.u8 = value;
        return *this;
    }

    Variant & operator = (int16_t value) {
        type_ = DataType::Int16;
        value_.i16 = value;
        return *this;
    }

    Variant & operator = (uint16_t value) {
        type_ = DataType::UInt16;
        value_.u16 = value;
        return *this;
    }

    Variant & operator = (int32_t value) {
        type_ = DataType::Int32;
        value_.i32 = value;
        return *this;
    }

    Variant & operator = (uint32_t value) {
        type_ = DataType::UInt32;
        value_.u32 = value;
        return *this;
    }

    Variant & operator = (int64_t value) {
        type_ = DataType::Int64;
        value_.i64 = value;
        return *this;
    }

    Variant & operator = (uint64_t value) {
        type_ = DataType::UInt64;
        value_.u64 = value;
        return *this;
    }

    Variant & operator = (void * value) {
        type_ = DataType::Pointer;
        value_.ptr = value;
        return *this;
    }

    Variant & operator = (float value) {
        type_ = DataType::Float;
        value_.f = value;
        return *this;
    }

    Variant & operator = (double value) {
        type_ = DataType::Double;
        value_.d = value;
        return *this;
    }

    uint32_t getType() const { return type_; }

    void setType(uint32_t type) {
        type_ =  type;
    }

    Value getValue() const {
        return value_;
    }

    bool      getBool()   const { return value_.b;   }
    int8_t    getInt8()   const { return value_.i8;  }
    uint8_t   getUInt8()  const { return value_.u8;  }
    int16_t   getInt16()  const { return value_.i16; }
    uint16_t  getUInt16() const { return value_.u16; }
    int32_t   getInt32()  const { return value_.i32; }
    uint32_t  getUInt32() const { return value_.u32; }
    int64_t   getInt64()  const { return value_.i64; }
    uint64_t  getUInt64() const { return value_.u64; }

    void *    getPointer() const { return value_.ptr; }

    float     getFloat()  const { return value_.f; }
    double    getDouble() const { return value_.d; }

    void setValue(const Variant & src) {
        type_ = src.getType();
        value_ = src.getValue();
    }

    void setBool(bool value) {
        type_ = DataType::Bool;
        value_.b = value;
    }

    void setInt8(int8_t value) {
        type_ = DataType::Int8;
        value_.i8 = value;
    }

    void setUInt8(uint8_t value) {
        type_ = DataType::UInt8;
        value_.u8 = value;
    }

    void setInt16(int16_t value) {
        type_ = DataType::Int16;
        value_.i16 = value;
    }

    void setUInt16(uint16_t value) {
        type_ = DataType::UInt16;
        value_.u16 = value;
    }

    void setInt32(int32_t value) {
        type_ = DataType::Int32;
        value_.i32 = value;
    }

    void setUInt32(uint32_t value) {
        type_ = DataType::UInt32;
        value_.u32 = value;
    }

    void setInt64(int64_t value) {
        type_ = DataType::Int64;
        value_.i64 = value;
    }

    void setUInt64(uint64_t value) {
        type_ = DataType::UInt64;
        value_.u64 = value;
    }

    void setPointer(void * value) {
        type_ = DataType::Pointer;
        value_.ptr = value;
    }

    void setFloat(float value) {
        type_ = DataType::Float;
        value_.f = value;
    }

    void setDouble(double value) {
        type_ = DataType::Double;
        value_.d = value;
    }
};

} // namespace kvdb

#endif // KVDB_STREAM_VARIANT_H
