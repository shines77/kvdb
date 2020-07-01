
#ifndef KVDB_TEST_H
#define KVDB_TEST_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"
#include "kvdb/basic/stdsize.h"
#include <kvdb/test/Expert.h>
#include <kvdb/test/Assert.h>
#include <kvdb/system/Console.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

namespace kvdb {

#if defined(_WIN32) || defined(__cygwin__)

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

#elif defined(__linux__) || defined(__GNUC__) || defined(__clang__)

//
// See: https://blog.csdn.net/hejinjing_tom_com/article/details/12162491
//
// \033[30m Black \033[0m
//

void print_passed()
{
    printf("\033[32mPassed\033[0m");
}

void print_failed()
{
    printf("\033[31mFailed\033[0m");
}

#else

void print_passed()
{
    printf("Passed");
}

void print_failed()
{
    printf("Failed");
}

#endif // _WIN32

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

} // namespace kvdb

#endif // KVDB_TEST_H
