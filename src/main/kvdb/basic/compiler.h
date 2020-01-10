
#ifndef KVDB_BASIC_COMPILER_H
#define KVDB_BASIC_COMPILER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define KVDB_PLATFORM_WINDOWS   0
#define KVDB_PLATFORM_UNIX      1
#define KVDB_PLATFORM_APPLE     2
#define KVDB_PLATFORM_INTEL     3

// Must be first (win 64 also define _WIN32)
#if defined(_WIN64)
#  define KVDB_PLATFORM     KVDB_PLATFORM_WINDOWS
#elif defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
#  define KVDB_PLATFORM     KVDB_PLATFORM_WINDOWS
#elif defined(__APPLE_CC__)
#  define KVDB_PLATFORM     KVDB_PLATFORM_APPLE
#elif defined(__INTEL_COMPILER)
#  define KVDB_PLATFORM     KVDB_PLATFORM_INTEL
#else
#  define KVDB_PLATFORM     KVDB_PLATFORM_UNIX
#endif

#define KVDB_COMPILER_MSVC      0
#define KVDB_COMPILER_GNU       1
#define KVDB_COMPILER_BORLAND   2
#define KVDB_COMPILER_INTEL     3

#if defined(_MSC_VER)
#  define KVDB_COMPILER     KVDB_COMPILER_MSVC
#elif defined(__BORLANDC__)
#  define KVDB_COMPILER     KVDB_COMPILER_BORLAND
#elif defined(__INTEL_COMPILER)
#  define KVDB_COMPILER     KVDB_COMPILER_INTEL
#elif defined(__GNUC__)
#  define KVDB_COMPILER     KVDB_COMPILER_GNU
#  define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#  error "FATAL ERROR: Unknown compiler."
#endif

#endif // KVDB_BASIC_COMPILER_H
