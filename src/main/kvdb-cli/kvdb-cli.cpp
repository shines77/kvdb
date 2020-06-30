
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

// String compare mode
#define STRING_COMPARE_STDC     0
#define STRING_COMPARE_U64      1
#define STRING_COMPARE_SSE42    2

#define STRING_COMPARE_MODE     STRING_COMPARE_SSE42

#include <kvdb/common/boost_asio_msvc.h>

#include <kvdb/support/PowerOf2.h>
#include <kvdb/system/Console.h>
#include "client/KvdbClientApp.h"

#include <inttypes.h>   // PRIu64

using namespace kvdb;
using namespace kvdb::client;

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#ifndef PRIuSIZE
#define PRIuSIZE  "zu"
#endif
#ifndef PRIsSIZE
#define PRIsSIZE  "z"
#endif
#else
#ifndef PRIuSIZE
#define PRIuSIZE  PRIuPTR
#endif
#ifndef PRIsSIZE
#define PRIsSIZE  PRIdPTR
#endif
#endif

#define KV_TEST_EQUAL(L, R) \
    do { \
        if ((L) == (R)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define ROUND_TO_POWER2_TEST(name, N) \
    printf("Test: [%s<%" PRIuSIZE ">] == %" PRIuSIZE ", %" PRIuSIZE ", ", #name, size_t(N), \
           kvdb::compile_time::##name##<##N##>::value, \
           kvdb::run_time::round_up_to_pow2(size_t(##N##))); \
    KV_TEST_EQUAL(kvdb::compile_time::##name##<##N##>::value, \
                  kvdb::run_time::round_up_to_pow2(size_t(##N##)))

#define ROUND_TO_POWER2_TEST_1(name, N) \
    printf("Test: [%s<%" PRIuSIZE ">] == %" PRIuSIZE ", %" PRIuSIZE ", ", name, size_t(N), \
           kvdb::compile_time::round_up_to_power2<##N##>::value, \
           kvdb::run_time::round_up_to_pow2(size_t(##N##))); \
    KV_TEST_EQUAL(kvdb::compile_time::round_up_to_power2<##N##>::value, \
                  kvdb::run_time::round_up_to_pow2(size_t(##N##)))

void print_passed()
{
    SetConsoleTextFgColor(FgColor::Green);
    printf("Passed");
    RecoverConsoleTextColor();
}

void print_failed()
{
    SetConsoleTextFgColor(FgColor::Red);
    printf("Failed");
    RecoverConsoleTextColor();
}

void print_passed_ln()
{
    print_passed();
    printf(".\n");
}

void print_failed_ln()
{
    print_failed();
    printf(".\n");
}

void round_to_power2_test_1()
{
    ROUND_TO_POWER2_TEST(round_up_to_power2, 0);
    //ROUND_TO_POWER2_TEST(round_up_to_power2, 1);
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

void round_to_power2_test_2()
{
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 0);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 1);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 2);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 3);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 4);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 5);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 6);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 7);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 8);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 9);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 10);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 11);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 12);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 13);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 14);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 15);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 16);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 17);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 18);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 19);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 20);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 21);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 22);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 23);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 24);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 25);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 26);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 27);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 28);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 29);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 30);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 31);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 32);
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 33);
    // 0x7FFFFFFF
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 2147483647);
    // 0x80000000
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 2147483648);
    // 0x80000001
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 2147483649);
    // 0x7FFFFFFFFFFFFFFEFULL
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 9223372036854775806);
    // 0x7FFFFFFFFFFFFFFFULL
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 9223372036854775807);
    // 0x8000000000000000ULL
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 9223372036854775808);
    // 0x8000000000000001ULL
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 9223372036854775809);
    // 0xFFFFFFFFFFFFFFFEULL
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 18446744073709551614);
    // 0xFFFFFFFFFFFFFFFFULL
    ROUND_TO_POWER2_TEST(_round_up_to_power2, 18446744073709551615);

    printf("\n");
}

int main(int argc, char * argv[])
{
    printf("integral_utils<size_t>::bits       = %" PRIuPTR "\n", detail::integral_utils<size_t>::bits);
    printf("integral_utils<size_t>::max_shift  = %" PRIuPTR "\n", detail::integral_utils<size_t>::max_shift);
    printf("integral_utils<size_t>::max_power2 = %" PRIuPTR "\n", detail::integral_utils<size_t>::max_power2);
    printf("integral_utils<size_t>::max_num    = %" PRIuPTR "\n", detail::integral_utils<size_t>::max_num);
    printf("\n");

    round_to_power2_test_1();
    round_to_power2_test_2();
    return 0;

    //KvdbClientApp app(argc, argv);
    //int result = app.run();
    //return result;
}
