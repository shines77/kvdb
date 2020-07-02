
#ifndef KVDB_TEST_FLOAT_EPSINON_H
#define KVDB_TEST_FLOAT_EPSINON_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <float.h>

//
// Doug Gwyn: if (RelDif(a, b) <= TOLERANCE) ...
//
// See: https://www.jianshu.com/p/b4f0dc31fd4e
//

// FLT_EPSILON = 1.192092896e-07F
#define FLOAT_EPSINON               FLT_EPSILON

// DBL_EPSILON = 2.2204460492503131e-016
#define DOUBLE_EPSINON              DBL_EPSILON

#define FLOAT_POSITIVE_EPSINON      ( 1e-6)
#define FLOAT_NEGATIVE_EPSINON      (-1e-6)

#define DOUBLE_POSITIVE_EPSINON     ( 1e-12)
#define DOUBLE_NEGATIVE_EPSINON     (-1e-12)

namespace kvdb {

//
// See: https://my.oschina.net/2bit/blog/3065096
//

static inline int is_float32_eq(float x, float y) {
    return ((x == y) || (
        ((*(uint32_t *)&x ^ *(uint32_t *)&y) == 1) &&
        ((*(uint32_t *)&x & 0x7F800000UL) != 0x7F800000UL)));
}

static inline int is_float64_eq(double x, double y) {
    return ((x == y) || (
        ((*(uint64_t *)&x ^ *(uint64_t *)&y) == 1) &&
        ((*(uint64_t *)&x & 0x7FF0000000000000ULL) != 0x7FF0000000000000ULL)));
}

} // namespace kvdb

#endif // KVDB_TEST_FLOAT_EPSINON_H
