
#ifndef KVDB_TEST_EXPERT_H
#define KVDB_TEST_EXPERT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//
// KVTEST_EXPECT_XX
//

#define KVTEST_EXPECT_TRUE(condition) \
    do { \
        if (!!(condition)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_EXPECT_FALSE(condition) \
    do { \
        if (!!!(condition)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_EXPECT_EQ(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_EXPECT_NE(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_EXPECT_GE(val1, val2) \
    do { \
        if ((val1) >= (val2)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_EXPECT_LE(val1, val2) \
    do { \
        if ((val1) <= (val2)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_EXPECT_GT(val1, val2) \
    do { \
        if ((val1) > (val2)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#define KVTEST_EXPECT_LT(val1, val2) \
    do { \
        if ((val1) < (val2)) { \
            print_passed_ln(); \
        } \
        else { \
            print_failed_ln(); \
        } \
    } while (0)

#endif // KVDB_TEST_EXPERT_H
