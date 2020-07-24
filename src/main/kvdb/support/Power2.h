
#ifndef KVDB_SUPPORT_POWER2_H
#define KVDB_SUPPORT_POWER2_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stddef.h"
#include "kvdb/basic/stdint.h"
#include "kvdb/basic/stdsize.h"

#include <assert.h>

#include <cstdint>
#include <cstddef>

namespace kvdb {
namespace detail {

static inline
std::size_t aligned_to(std::size_t size, std::size_t alignment)
{
    assert(size >= 1);
    if (likely((size & (size - 1)) == 0)) return size;

    assert((alignment & (alignment - 1)) == 0);
    size = (size + alignment - 1) & ~(alignment - 1);
    assert((size / alignment * alignment) == size);
    return size;
}

} // namespace detail
} // namespace kvdb

#endif // KVDB_SUPPORT_POWER2_H
