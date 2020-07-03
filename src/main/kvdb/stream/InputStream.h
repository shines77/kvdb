
#ifndef KVDB_STREAM_INPUTSTREAM_H
#define KVDB_STREAM_INPUTSTREAM_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdint>
#include <string>
#include <type_traits>

#include "kvdb/core/DataType.h"
#include "kvdb/stream/ConstBuffer.h"
#include "kvdb/stream/BasicStream.h"
#include "kvdb/stream/BasicPrepareStream.h"

namespace kvdb {

template < typename StorageTy, typename Base = BasicStream<StorageTy> >
class BasicInputStream : public Base {
public:
    typedef Base                                base_type;
    typedef BasicInputStream<StorageTy, Base>   this_type;
    typedef this_type                           downcast_type;

    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;
    typedef typename base_type::storage_type    storage_type;
    typedef typename base_type::allocator_type  allocator_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

protected:
    StorageTy istorage_;

public:
    BasicInputStream(storage_type & storage)
        : base_type(storage), istorage_(nullptr, 0) {
    }
    BasicInputStream(storage_type && storage)
        : base_type(std::forward<storage_type>(storage)),
          istorage_(nullptr, 0) {
    }
    BasicInputStream(const char_type * data, size_type size)
        : base_type(istorage_), istorage_(data, size) {
        this->attach(data, size);
    }
    template <size_type N>
    BasicInputStream(const char_type(&data)[N])
        : base_type(istorage_), istorage_(data, N) {
        this->attach(data, N);
    }
    ~BasicInputStream() {}

    void attach(const char_type * data, size_type size) {
        if (this->isConstBuffer()) {
            this->storage_.attach(data, size);
            this->setCurrent(data);
        }
        else {
            this->storage_.copy(data, size);
            this->setCurrent(this->storage_.data());
        }
    }

    template <size_type N>
    void attach(const char_type(&data)[N]) {
        this->attach(data, N);
    }

protected:
    template <typename StringType>
    void internalReadString(StringType & value, size_t length) {
        base_type::getString(value, length);
        base_type::skipChar((int)length);
    }

public:
    uint8_t readType() {
        return this->readUInt8();
    }

    uint8_t readByte() {
        uint8_t value = base_type::getByte();
        base_type::skipByte();
        return value;
    }

    char_type readChar() {
        char_type value = base_type::getChar();
        base_type::skipChar();
        return value;
    }

    bool readBool() {
        bool value = base_type::getBool();
        base_type::skipBool();
        return value;
    }

    int8_t readInt8() {
        int8_t value = base_type::getInt8();
        base_type::skipInt8();
        return value;
    }

    int16_t readInt16() {
        int16_t value = base_type::getInt16();
        base_type::skipInt16();
        return value;
    }

    int32_t readInt32() {
        int32_t value = base_type::getInt32();
        base_type::skipInt32();
        return value;
    }

    int64_t readInt64() {
        int64_t value = base_type::getInt64();
        base_type::skipInt64();
        return value;
    }

    uint8_t readUInt8() {
        uint8_t value = base_type::getUInt8();
        base_type::skipUInt8();
        return value;
    }

    uint16_t readUInt16() {
        uint16_t value = base_type::getUInt16();
        base_type::skipUInt16();
        return value;
    }

    uint32_t readUInt32() {
        uint32_t value = base_type::getUInt32();
        base_type::skipUInt32();
        return value;
    }

    uint64_t readUInt64() {
        uint64_t value = base_type::getUInt64();
        base_type::skipUInt64();
        return value;
    }

    void * readPointer() {
        void * value = base_type::getPointer();
        base_type::skipPointer();
        return value;
    }

    float readFloat() {
        float value = base_type::getFloat();
        base_type::skipFloat();
        return value;
    }

    double readDouble() {
        double value = base_type::getDouble();
        base_type::skipDouble();
        return value;
    }

    template <typename StringType>
    int readString(StringType & value) {
        uint32_t length = this->readUInt32();
        this->internalReadString(value, length);
        return ReadResult::Ok;
    }

    template <typename StringType>
    int parseString(StringType & value) {
        int result = ReadResult::Ok;
        uint32_t length;

        uint8_t type = this->readType();
        switch (type) {
        case DataType::String:
            length = this->readUInt32();
            this->internalReadString(value, length);
            break;

        case DataType::String1B:
            length = this->readUInt8();
            this->internalReadString(value, length);
            break;

        case DataType::String2B:
            length = this->readUInt16();
            this->internalReadString(value, length);
            break;

        case DataType::String3B:
        {
            uint32_t value32 = this->readUInt32();
#if NOT_IS_LITTLE_ENDIAN
            length = (value32 & 0xFFFFFF00UL) >> 8;
#else
            length = value32 & 0x00FFFFFFUL;
#endif
            this->internalReadString(value, length);
            break;
        }

        default:
            result = ReadResult::Failed;
            break;
        }

        return result;
    }
};

typedef BasicInputStream< BasicByteBuffer<char>, BasicStream<BasicByteBuffer<char>> >           ByteInputStream;
typedef BasicInputStream< BasicByteBuffer<wchar_t>, BasicStream<BasicByteBuffer<wchar_t>> >     InputByteStreamW;

typedef BasicInputStream< BasicByteBuffer<char>, BasicPrepareStream<BasicByteBuffer<char>> >        PrepareByteInputStream;
typedef BasicInputStream< BasicByteBuffer<wchar_t>, BasicPrepareStream<BasicByteBuffer<wchar_t>> >  PrepareByteInputStreamW;

typedef BasicInputStream< BasicConstBuffer<char>, BasicStream<BasicConstBuffer<char>> >         ConstInputStream;
typedef BasicInputStream< BasicConstBuffer<wchar_t>, BasicStream<BasicConstBuffer<wchar_t>> >   ConstInputStreamW;

typedef BasicInputStream< BasicConstBuffer<char>, BasicPrepareStream<BasicConstBuffer<char>> >        PrepareConstInputStream;
typedef BasicInputStream< BasicConstBuffer<wchar_t>, BasicPrepareStream<BasicConstBuffer<wchar_t>> >  PrepareConstInputStreamW;

typedef ConstInputStream            InputStream;
typedef ConstInputStreamW           InputStreamW;
typedef PrepareConstInputStream     PrepareInputStream;
typedef PrepareConstInputStreamW    PrepareInputStreamW;

} // namespace kvdb

#endif // KVDB_STREAM_INPUTSTREAM_H
