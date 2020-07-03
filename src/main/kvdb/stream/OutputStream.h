
#ifndef KVDB_STREAM_OUTPUTSTREAM_H
#define KVDB_STREAM_OUTPUTSTREAM_H

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

#include "kvdb/stream/BasicStream.h"
#include "kvdb/stream/BasicPrepareStream.h"

namespace kvdb {

template < typename StorageTy, typename Base = BasicStream<StorageTy> >
class BasicOutputStream : public Base {
public:
    typedef Base                                base_type;
    typedef BasicOutputStream<StorageTy, Base>  this_type;
    typedef this_type                           downcast_type;

    typedef typename base_type::char_type       char_type;
    typedef typename base_type::size_type       size_type;
    typedef typename base_type::storage_type    storage_type;
    typedef typename base_type::allocator_type  allocator_type;

    typedef typename base_type::string_type     string_type;
    typedef typename base_type::stringref_type  stringref_type;

    BasicOutputStream(storage_type & storage) : base_type(storage) {}
    BasicOutputStream(storage_type && storage) : base_type(std::forward<storage_type>(storage)) {}
    ~BasicOutputStream() {}

protected:
    template <typename StringType>
    void internalWriteString(const StringType & value) {
        base_type::setString(value);
        base_type::skipChar(static_cast<int>(value.size()));
    }

public:
    downcast_type & downcastTo() {
        return (*static_cast<downcast_type *>(this));
    }

    const downcast_type & downcastTo() const {
        return (*const_cast<const downcast_type *>(static_cast<downcast_type *>(this)));
    }

    void writeType(uint8_t type) {
        this->writeUInt8(type);
    }

    void writeType32(uint32_t type) {
        this->writeUInt8((uint8_t)type);
    }

    void writeByte(uint8_t value) {
        base_type::setByte(value);
        base_type::skipByte();
    }

    void writeChar(char_type value) {
        base_type::setChar(value);
        base_type::skipChar();
    }

    void writeBool(bool value) {
        base_type::setBool(value);
        base_type::skipBool();
    }

    void writeInt8(int8_t value) {
        base_type::setInt8(value);
        base_type::skipInt8();
    }

    void writeInt16(int16_t value) {
        base_type::setInt16(value);
        base_type::skipInt16();
    }

    void writeInt32(int32_t value) {
        base_type::setInt32(value);
        base_type::skipInt32();
    }

    void writeInt64(int64_t value) {
        base_type::setInt64(value);
        base_type::skipInt64();
    }

    void writeUInt8(uint8_t value) {
        base_type::setUInt8(value);
        base_type::skipUInt8();
    }

    void writeUInt16(uint16_t value) {
        base_type::setUInt16(value);
        base_type::skipUInt16();
    }

    void writeUInt32(uint32_t value) {
        base_type::setUInt32(value);
        base_type::skipUInt32();
    }

    void writeUInt64(uint64_t value) {
        base_type::setUInt64(value);
        base_type::skipUInt64();
    }

    void writePointer(void * value) {
        base_type::setPointer(value);
        base_type::skipPointer();
    }

    void writeFloat(float value) {
        base_type::setFloat(value);
        base_type::skipFloat();
    }

    void writeDouble(double value) {
        base_type::setDouble(value);
        base_type::skipDouble();
    }

    template <typename StringType>
    void writeString(const StringType & value) {
        this->writeUInt32(static_cast<uint32_t>(value.size()));
        this->internalWriteString(value);
    }
};

typedef BasicOutputStream< BasicByteBuffer<char>, BasicStream<BasicByteBuffer<char>> >          OutputStream;
typedef BasicOutputStream< BasicByteBuffer<wchar_t>, BasicStream<BasicByteBuffer<wchar_t>> >    OutputStreamW;

typedef BasicOutputStream< BasicByteBuffer<char>, BasicPrepareStream<BasicByteBuffer<char>> >       PrepareOutputStream;
typedef BasicOutputStream< BasicByteBuffer<wchar_t>, BasicPrepareStream<BasicByteBuffer<wchar_t>> > PrepareOutputStreamW;

static BasicByteBuffer<char>    s_dummyStorage;
static BasicByteBuffer<wchar_t> s_dummyStorageW;

} // namespace kvdb

#endif // KVDB_STREAM_OUTPUTSTREAM_H
