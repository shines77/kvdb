
#ifndef KVDB_TEST_ASSERT_H
#define KVDB_TEST_ASSERT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//
// KVTEST_ASSERT_XX
//

#define KVTEST_ASSERT_TRUE(condition) \
    do { \
        if (!!(condition)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_ASSERT_FALSE(condition) \
    do { \
        if (!!!(condition)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_ASSERT_NE(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_ASSERT_GE(val1, val2) \
    do { \
        if ((val1) >= (val2)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_ASSERT_LE(val1, val2) \
    do { \
        if ((val1) <= (val2)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_ASSERT_GT(val1, val2) \
    do { \
        if ((val1) > (val2)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_ASSERT_LT(val1, val2) \
    do { \
        if ((val1) < (val2)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#endif // KVDB_TEST_ASSERT_H
