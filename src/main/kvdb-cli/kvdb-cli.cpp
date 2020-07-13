
#ifndef __SSE4_2__
#define __SSE4_2__              1
#endif // __SSE4_2__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include <iostream>
#include <iomanip>  // std::setw(), std::setfill(), std::setprecision().
#include <sstream>
#include <string>
#include <utility>

#if __SSE4_2__

// Support SSE 4.2: _mm_crc32_u32(), _mm_crc32_u64().
#define SUPPORT_SSE42_CRC32C    1

// Support Intel SMID SHA module: sha1 & sha256, it's higher than SSE 4.2 .
// _mm_sha1msg1_epu32(), _mm_sha1msg2_epu32() and so on.
#define SUPPORT_SMID_SHA        0

#endif // __SSE4_2__

// String compare mode
#define STRING_COMPARE_STDC     0
#define STRING_COMPARE_U64      1
#define STRING_COMPARE_SSE42    2

#define STRING_COMPARE_MODE     STRING_COMPARE_SSE42

#include <kvdb/common/boost_asio_msvc.h>

#include <kvdb/basic/inttypes.h>
#include <kvdb/test/Test.h>
#include <kvdb/support/PowerOf2.h>
#include <kvdb/system/Console.h>
#include "client/KvdbClientApp.h"

using namespace kvdb;
using namespace kvdb::client;

//
// About marco # and ##
// See: https://www.cnblogs.com/wb-DarkHorse/p/3588787.html
//

#define ROUND_TO_POWER2_TEST(clsName, N) \
    printf("Test: [%s<%" PRIuSIZE ">] == %" PRIuSIZE ", %" PRIuSIZE ", ", \
           #clsName, size_t(N), \
           kvdb::compile_time::clsName<N>::value, \
           kvdb::run_time::round_up_to_pow2(size_t(N))); \
    KVTEST_EXPECT_EQ(kvdb::compile_time::clsName<N>::value, \
                     kvdb::run_time::round_up_to_pow2(size_t(N)))

void round_to_power2_test_1()
{
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 0);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 1);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 2);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 3);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 4);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 5);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 6);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 7);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 8);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 9);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 10);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 11);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 12);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 13);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 14);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 15);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 16);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 17);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 18);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 19);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 20);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 21);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 22);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 23);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 24);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 25);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 26);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 27);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 28);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 29);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 30);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 31);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 32);
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 33);
    // 0x7FFFFFFF
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 2147483647);
    // 0x80000000
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 2147483648);
    // 0x80000001
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 2147483649);
    // 0x7FFFFFFFFFFFFFFEFULL
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 9223372036854775806);
    // 0x7FFFFFFFFFFFFFFFULL
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 9223372036854775807);
    // 0x8000000000000000ULL
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 9223372036854775808);
    // 0x8000000000000001ULL
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 9223372036854775809);
    // 0xFFFFFFFFFFFFFFFEULL
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 18446744073709551614);
    // 0xFFFFFFFFFFFFFFFFULL
    ROUND_TO_POWER2_TEST(round_up_to_pow2, 18446744073709551615);

    printf("\n");
}

void round_to_power2_test_2()
{
    ROUND_TO_POWER2_TEST(round_up_to_power2, 0);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 1);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 2);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 3);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 4);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 5);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 6);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 7);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 8);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 9);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 10);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 11);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 12);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 13);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 14);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 15);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 16);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 17);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 18);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 19);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 20);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 21);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 22);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 23);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 24);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 25);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 26);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 27);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 28);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 29);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 30);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 31);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 32);
    ROUND_TO_POWER2_TEST(round_up_to_power2, 33);
    // 0x7FFFFFFF
    ROUND_TO_POWER2_TEST(round_up_to_power2, 2147483647);
    // 0x80000000
    ROUND_TO_POWER2_TEST(round_up_to_power2, 2147483648);
    // 0x80000001
    ROUND_TO_POWER2_TEST(round_up_to_power2, 2147483649);
    // 0x7FFFFFFFFFFFFFFEFULL
    ROUND_TO_POWER2_TEST(round_up_to_power2, 9223372036854775806);
    // 0x7FFFFFFFFFFFFFFFULL
    ROUND_TO_POWER2_TEST(round_up_to_power2, 9223372036854775807);
    // 0x8000000000000000ULL
    ROUND_TO_POWER2_TEST(round_up_to_power2, 9223372036854775808);
    // 0x8000000000000001ULL
    ROUND_TO_POWER2_TEST(round_up_to_power2, 9223372036854775809);
    // 0xFFFFFFFFFFFFFFFEULL
    ROUND_TO_POWER2_TEST(round_up_to_power2, 18446744073709551614);
    // 0xFFFFFFFFFFFFFFFFULL
    ROUND_TO_POWER2_TEST(round_up_to_power2, 18446744073709551615);

    printf("\n");
}

void integral_utils_test()
{
    printf("integral_utils<size_t>::bits       = %" PRIuSIZE "\n", detail::integral_utils<size_t>::bits);
    printf("integral_utils<size_t>::max_shift  = %" PRIuSIZE "\n", detail::integral_utils<size_t>::max_shift);
    printf("integral_utils<size_t>::max_power2 = %" PRIuSIZE "\n", detail::integral_utils<size_t>::max_power2);
    printf("integral_utils<size_t>::max_num    = %" PRIuSIZE "\n", detail::integral_utils<size_t>::max_num);
    printf("\n");
}

int main(int argc, char * argv[])
{
#if 0
    integral_utils_test();

    round_to_power2_test_1();
    round_to_power2_test_2();
    return 0;
#else
    KvdbClientApp app(argc, argv);
    int result = app.run();
    return result;
#endif
}
