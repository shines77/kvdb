
#ifndef JSTD_STRINGREFLIST_H
#define JSTD_STRINGREFLIST_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <cstddef>
#include <string>

#include "kvdb/basic/stddef.h"
#include "kvdb/jstd/StringRef.h"

namespace jstd {

template <typename CharTy, std::size_t InitCapacity>
class BasicStringRefList {
public:
    typedef CharTy char_type;
    typedef std::size_t size_type;
    typedef std::basic_string<char_type> string_type;
    typedef BasicStringRef<char_type> stringref_type;

    static const std::size_t kInitCapacity = InitCapacity;

private:
    struct Entry {
        uint32_t offset;
        uint32_t length;
    };

    struct EntryPair {
        Entry key;
        Entry value;
    };

    struct EntryChunk {
        EntryChunk * next;
        std::size_t  size;
        std::size_t  capacity;
        EntryPair *  entries;

        EntryChunk(std::size_t _capacity)
            : next(nullptr), size(0), capacity(0), entries(nullptr) {
            EntryPair * new_entries = new EntryPair[_capacity];
            if (new_entries != nullptr) {
                this->entries = new_entries;
                this->capacity = _capacity;
            }
        }
        ~EntryChunk() {
#if 0
            next = nullptr;
            size = 0;
            capacity = 0;
#endif
            if (entries) {
                delete[] entries;
                entries = nullptr;
            }
        }
    };

public:
    stringref_type ref;
private:
    size_type    size_;
    size_type    capacity_;
    EntryChunk * head_;
    EntryChunk * tail_;
public:
    EntryPair items[kInitCapacity];

public:
    BasicStringRefList(std::size_t capacity = kInitCapacity)
        : ref(), size_(0), capacity_(capacity), head_(nullptr), tail_(nullptr) {
        initList();
    }
    BasicStringRefList(const char_type * data)
        : ref(data), size_(0), capacity_(kInitCapacity), head_(nullptr), tail_(nullptr) {
        initList();
    }
    BasicStringRefList(const char_type * data, size_type size)
        : ref(data, size), size_(0), capacity_(kInitCapacity), head_(nullptr), tail_(nullptr) {
        initList();
    }
    template <size_type N>
    BasicStringRefList(const char_type (&src)[N])
        : ref(src, N - 1), size_(0), capacity_(kInitCapacity), head_(nullptr), tail_(nullptr) {
        initList();
    }
    BasicStringRefList(const string_type & src)
        : ref(src), size_(0), capacity_(kInitCapacity), head_(nullptr), tail_(nullptr) {
        initList();
    }
    BasicStringRefList(const stringref_type & src)
        : ref(src), size_(0), capacity_(kInitCapacity), head_(nullptr), tail_(nullptr) {
        initList();
    }

    ~BasicStringRefList() {
        destroyChunks();
    }

private:
    void appendItem(std::size_t index,
                    const char * key, std::size_t key_len,
                    const char * value, std::size_t value_len) {
        EntryPair & item = this->items[index];
        item.key.offset = static_cast<uint16_t>(key - ref.data());
        item.key.length = static_cast<uint16_t>(key_len);
        item.value.offset = static_cast<uint16_t>(value - ref.data());
        item.value.length = static_cast<uint16_t>(value_len);
    }

public:
    const char_type * data() const { return this->ref.data(); }
    size_type size() const { return this->size_; }
    size_type capacity() const { return this->capacity_; }

    const char_type * c_str() const { return this->data(); }
    size_type length() const { return this->size(); }

    bool is_empty() const { return (this->size() == 0); }

    void reset() {
        this->ref.clear();
        this->size_ = 0;
        this->capacity_ = kInitCapacity;
        this->destroyChunks();
    }

    void clear() {
        this->reset();
    }

    void setRef(const char_type * data) {
        this->ref.assign(data);
    }

    void setRef(const char_type * data, size_type size) {
        this->ref.assign(data, size);
    }

    void setRef(const char_type * first, const char_type * last) {
        this->ref.assign(first, last);
    }

    template <size_type N>
    void setRef(const char_type (&src)[N]) {
        this->ref.assign(src, N - 1);
    }

    void setRef(const string_type & src) {
        this->ref.assign(src);
    }

    void setRef(const stringref_type & src) {
        this->ref.assign(src);
    }

    void initList() {
#ifndef NDEBUG
        ::memset((void *)&items, 0, sizeof(items));
#endif // !NDEBUG
    }

    void destroyChunks() {
        EntryChunk * chunk = this->head_;
        while (unlikely(chunk != nullptr)) {
            EntryChunk * next = chunk->next;
            delete chunk;
            chunk = next;
        }
        this->head_ = nullptr;
        this->tail_ = nullptr;
    }

    EntryChunk * findLastChunk() {
        EntryChunk * chunk = this->head_;
        while (likely(chunk != nullptr)) {
            if (likely(chunk->next != nullptr))
                chunk = chunk->next;
            else
                break;
        }
        return chunk;
    }

    void append(const char * key, std::size_t key_len,
                const char * value, std::size_t value_len) {
        assert(key != nullptr);
        assert(value != nullptr);
        if (likely(this->size_ < kInitCapacity)) {
            assert(this->size_ < this->capacity_);
            this->appendItem(this->size_, key, key_len, value, value_len);
            ++(this->size_);
        }
        else {
            if (unlikely(this->size_ >= this->capacity_)) {
                // Add a new entries chunk
                static const std::size_t kChunkSize = 64;
                EntryChunk * newChunk = new EntryChunk(kChunkSize);
                if (newChunk != nullptr) {
                    if (this->head_ == nullptr)
                        this->head_ = newChunk;
                    if (this->tail_ == nullptr) {
                        this->tail_ = newChunk;
                    }
                    else {
                        this->tail_->next = newChunk;
                        this->tail_ = newChunk;
                    }
                    this->capacity_ += kChunkSize;
                }
            }
            assert(this->size_ < this->capacity_);
            //
            // TODO: If the item count more than fixed kInitCapacity size,
            //       append to the last position of last chunk.
            //
            appendItem(this->size_, key, key_len, value, value_len);
            ++(this->size_);
        }
    }
};

template <std::size_t InitCapacity>
using StringRefList = BasicStringRefList<char, InitCapacity>;

template <std::size_t InitCapacity>
using StringRefListW = BasicStringRefList<wchar_t, InitCapacity>;

} // namespace jstd

#endif // JSTD_STRINGREFLIST_H
