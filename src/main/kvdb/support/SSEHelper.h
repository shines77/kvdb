
#ifndef KVDB_SSE_HELPER_H
#define KVDB_SSE_HELPER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <nmmintrin.h>  // For SSE 4.2

namespace kvdb {

template <typename CharTy>
struct SSEHelper {
    static const int _SIDD_CHAR_OPS = _SIDD_UBYTE_OPS;
    static const int kMaxSize = 16;
    static const int kWordSize = 1;
};

template <>
struct SSEHelper<short> {
    static const int _SIDD_CHAR_OPS = _SIDD_UWORD_OPS;
    static const int kMaxSize = 8;
    static const int kWordSize = 2;
};

template <>
struct SSEHelper<unsigned short> {
    static const int _SIDD_CHAR_OPS = _SIDD_UWORD_OPS;
    static const int kMaxSize = 8;
    static const int kWordSize = 2;
};

template <>
struct SSEHelper<wchar_t> {
    static const int _SIDD_CHAR_OPS = _SIDD_UWORD_OPS;
    static const int kMaxSize = 8;
    static const int kWordSize = 2;
};

} // namespace kvdb

#endif // KVDB_SSE_HELPER_H
