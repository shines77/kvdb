
#ifndef SUPPORT_POWEROF2_H
#define SUPPORT_POWEROF2_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdsize.h"

#include <stdint.h>
#include <stddef.h>

#include <type_traits>

namespace kvdb {
namespace detail {

enum boolean_value_t {
    false_value = false,
    true_value = true
};

struct false_type {
    typedef false_type type;
    enum { value = false_value };
};

struct true_type {
    typedef true_type type;
    enum { value = true_value };
};

template <bool condition, class T, class U>
struct condition_if {
    typedef U type;
};

template <class T, class U>
struct condition_if<true, T, U> {
    typedef T type;
};

template <bool condition>
struct boolean_if {
    typedef typename condition_if<condition, true_type, false_type>::type type;
    enum { value = type::value };
};

template <typename T>
struct integral_utils {
    typedef typename std::make_unsigned<T>::type unsigned_type;

    static_assert(std::is_integral<T>::value,
        "Error: integral_utils<T> -- T must be a integral type.");

    // max bits
    static const size_t bits = sizeof(T) * 8;
    static const size_t max_shift = bits - 1;
    // 0x80000000UL;
    static const unsigned_type max_power2 = static_cast<unsigned_type>(1UL) << max_shift;
    // 0xFFFFFFFFUL;
    static const unsigned_type max_num = static_cast<unsigned_type>(-1);
};

template <>
struct integral_utils<size_t> {
    // max bits
    static const size_t bits = sizeof(size_t) * 8;
    static const size_t max_shift = bits - 1;
    // 0x80000000UL;
    static const size_t max_power2 = static_cast<size_t>(1UL) << max_shift;
    // 0xFFFFFFFFUL;
    static const size_t max_num = static_cast<size_t>(-1);
};

template <>
struct integral_utils<ssize_t> {
    // max bits
    static const size_t bits = sizeof(ssize_t) * 8;
    static const size_t max_shift = bits - 1;
    // 0x80000000UL;
    static const size_t max_power2 = static_cast<size_t>(1UL) << max_shift;
    // 0xFFFFFFFFUL;
    static const size_t max_num = static_cast<size_t>(-1);
};

} // namespace detail

namespace run_time {

template <typename SizeType>
inline bool is_pow2(SizeType n) {
    static_assert(std::is_integral<SizeType>::value,
        "Error: is_pow2(SizeType n) -- SizeType must be a integral type.");
    typedef typename std::make_unsigned<SizeType>::type UnsignedSizeType;
    UnsignedSizeType x = n;
    return ((x & (x - 1)) == 0);
}

template <typename SizeType>
inline SizeType verify_pow2(SizeType n) {
    static_assert(std::is_integral<SizeType>::value,
        "Error: verify_pow2(SizeType n) -- SizeType must be a integral type.");
    typedef typename std::make_unsigned<SizeType>::type UnsignedSizeType;
    UnsignedSizeType x = n;
    return (x & (x - 1));
}

template <typename SizeType>
inline SizeType round_up_to_pow2(SizeType n) {
    static_assert(std::is_integral<SizeType>::value,
        "Error: round_up_to_pow2(SizeType n) -- SizeType must be a integral type.");
    typedef typename std::make_unsigned<SizeType>::type UnsignedSizeType;
    UnsignedSizeType x = static_cast<UnsignedSizeType>(n);
    if (is_pow2(x)) {
        if (x == detail::integral_utils<UnsignedSizeType>::max_power2)
            return static_cast<SizeType>(detail::integral_utils<UnsignedSizeType>::max_num);
        else if (x == 0)
            return static_cast<SizeType>(1);
        else
            return static_cast<SizeType>(x * 2);
    }
    else {
        x = static_cast<UnsignedSizeType>(x - 1);
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        return (x != detail::integral_utils<UnsignedSizeType>::max_num) ?
            static_cast<SizeType>(x + 1) : static_cast<SizeType>(x);
    }
}

template <>
inline uint32_t round_up_to_pow2(uint32_t n) {
    if (is_pow2(n)) {
        if (n == detail::integral_utils<uint32_t>::max_power2)
            return detail::integral_utils<uint32_t>::max_num;
        else
            return (n * 2);
    }
    else {
        uint32_t x = n - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        return (x != detail::integral_utils<uint32_t>::max_num) ? (x + 1) : x;
    }
}

template <>
inline int32_t round_up_to_pow2(int32_t n) {
    uint32_t x = static_cast<uint32_t>(n);
    if (is_pow2(x)) {
        if (x == detail::integral_utils<uint32_t>::max_power2)
            return static_cast<int32_t>(detail::integral_utils<uint32_t>::max_num);
        else
            return static_cast<int32_t>(x * 2);
    }
    else {
        if (n < 0) {
            x = static_cast<uint64_t>(-n);
        }
        x = x - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        if (n >= 0)
            return static_cast<int32_t>(x + 1);
        else
            return -static_cast<int32_t>(x + 1);
    }
}

template <>
inline uint64_t round_up_to_pow2(uint64_t n) {
    if (is_pow2(n)) {
        if (n == detail::integral_utils<uint64_t>::max_power2)
            return detail::integral_utils<uint64_t>::max_num;
        else if (n == 0)
            return uint64_t(1);
        else
            return (n * 2);
    }
    else {
        uint64_t x = n - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        x = x | (x >> 32);
        return (x != detail::integral_utils<uint64_t>::max_num) ? (x + 1) : x;
    }
}

template <>
inline int64_t round_up_to_pow2(int64_t n) {
    uint64_t x = static_cast<uint64_t>(n);
    if (is_pow2(x)) {
        if (x == detail::integral_utils<uint64_t>::max_power2)
            return static_cast<int64_t>(detail::integral_utils<uint64_t>::max_num);
        else if (n == 0)
            return int64_t(1);
        else
            return static_cast<int64_t>(x * 2);
    }
    else {
        if (n < 0) {
            x = static_cast<uint64_t>(-n);
        }
        x = x - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        x = x | (x >> 32);
        if (n >= 0)
            return static_cast<int64_t>(x + 1);
        else
            return -static_cast<int64_t>(x + 1);
    }
}

} // namespace run_time

namespace compile_time {

//////////////////////////////////////////////////////////////////////////////////

//
// is_pow_of_2 = (N && ((N & (N - 1)) == 0);  // here, N must is unsigned number
//
template <size_t N>
struct is_power2 {
    enum { value = detail::boolean_if<((N & (N - 1)) == 0)>::value };
};

template <>
struct is_power2<0> {
    enum { value = detail::true_value };
};

//////////////////////////////////////////////////////////////////////////////////

//
// round_up_to_pow2<N>
//

template <size_t N>
struct round_up_to_pow2_impl {
    static const size_t value = (is_power2<N>::value ? N :
        round_up_to_pow2_impl<N + 1>::value);
};

template <size_t N>
struct round_up_to_pow2 {
    static const size_t value = round_up_to_pow2_impl<N>::value;
};

//////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) \
 || defined(__amd64__) || defined(__x86_64__)

template <>
struct round_up_to_pow2_impl<18446744073709551615ULL> {
    static const size_t value = 18446744073709551615ULL;
};

template <>
struct round_up_to_pow2<18446744073709551615ULL> {
    static const size_t value = 18446744073709551615ULL;
};

#else

template <>
struct round_up_to_pow2_impl<4294967295UL> {
    static const size_t value = 4294967295UL;
};

template <>
struct round_up_to_pow2<4294967295UL> {
    static const size_t value = 4294967295UL;
};

#endif // _WIN64 || _M_X64 || _M_AMD64

//////////////////////////////////////////////////////////////////////////////////

// Maxinum recursive depth is about 497 layer.

template <size_t N, size_t Power2>
struct round_up_to_power2_impl {
    static const size_t max_power2 = detail::integral_utils<size_t>::max_power2;
    static const size_t max_num = detail::integral_utils<size_t>::max_num;
    static const size_t nextPower2 = (Power2 < max_power2) ? (Power2 << 1) : 0;
    static const bool too_large = (N >= max_power2);
    static const bool reach_limit = (Power2 == max_power2);

    static const size_t value = ((N >= max_power2) ? max_num :
           (((Power2 == max_power2) || (Power2 > N)) ? Power2 :
            round_up_to_power2_impl<N, nextPower2>::value));

    //static const size_t value = ((N >= max_power2) ? max_num :
    //    ((Power2 > N) ? Power2 : round_up_to_power2_impl<N, nextPower2>::value));
};

template <size_t N>
struct round_up_to_power2_impl<N, 0> {
    static const size_t value = detail::integral_utils<size_t>::max_num;
};

template <size_t N>
struct round_up_to_power2 {
    static const size_t value = round_up_to_power2_impl<N, 1>::value;
};

template <>
struct round_up_to_power2<0> {
    static const size_t value = 1;
};

//////////////////////////////////////////////////////////////////////////////////

template <size_t N>
struct _round_up_to_power2_impl {
    static const size_t max_num = detail::integral_utils<size_t>::max_num;
    static const size_t N1 = N - 1;
    static const size_t N2 = N1 | (N1 >> 1);
    static const size_t N3 = N2 | (N2 >> 2);
    static const size_t N4 = N3 | (N3 >> 4);
    static const size_t N5 = N4 | (N4 >> 8);
    static const size_t N6 = N5 | (N5 >> 16);
    static const size_t N7 = N6 | (N6 >> 32);
    static const size_t value = (N7 < max_num) ? (N7 + 1) : max_num;
};

template <size_t N>
struct _round_up_to_power2 {
    static const size_t max_power2 = detail::integral_utils<size_t>::max_power2;
    static const size_t max_num = detail::integral_utils<size_t>::max_num;
    static const size_t value = (N < max_num) ? _round_up_to_power2_impl<N + 1>::value : max_num;
};

template <>
struct _round_up_to_power2<0> {
    static const size_t value = 1;
};

//////////////////////////////////////////////////////////////////////////////////

//
// round_down_to_pow2<N>
//

template <size_t N>
struct round_down_to_pow2_impl {
    static const size_t value = (is_power2<N>::value ? N :
        round_down_to_pow2_impl<N - 1>::value);
};

template <size_t N>
struct round_down_to_pow2 {
    static const size_t value = round_down_to_pow2_impl<N>::value;
};

//////////////////////////////////////////////////////////////////////////////////

template <size_t N, size_t Power2>
struct round_down_to_power2_impl {
    enum {
        nextPower2 = Power2 * 2
    };
    static const bool too_large = (N >= detail::integral_utils<size_t>::max_power2);
    static const bool reach_limit = (nextPower2 == detail::integral_utils<size_t>::max_power2);
    static const size_t value = (too_large ? detail::integral_utils<size_t>::max_power2 :
           ((N == Power2) ? N :
            ((reach_limit || nextPower2 > N) ? Power2 :
             round_down_to_power2_impl<N, nextPower2>::value)));
};

template <size_t N>
struct round_down_to_power2 {
    static const size_t value = (is_power2<N>::value ? N : round_down_to_power2_impl<N, 1>::value);
};

template <>
struct round_down_to_power2<0> {
    enum { value = 0 };
};

//////////////////////////////////////////////////////////////////////////////////

} // namespace compile_time
} // namespace kvdb

#endif // SUPPORT_POWEROF2_H
