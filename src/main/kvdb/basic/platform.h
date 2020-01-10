
#ifndef KVDB_BASIC_PLATFORM_H
#define KVDB_BASIC_PLATFORM_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/compiler.h"

#ifndef KVDB_DEFINED
#define KVDB_DEFINED(X)                 ((##X) && (##X != 0))
#endif

/**
 Config of jimi project, per target platform.
 */

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which JIMI uses.
#define KVDB_PLATFORM_UNKNOWN           0
// Windows family
#define KVDB_PLATFORM_WIN32             1
#define KVDB_PLATFORM_WIN64             2
#define KVDB_PLATFORM_WINCE             3
#define KVDB_PLATFORM_WINRT             4
#define KVDB_PLATFORM_WP8               5
// Linux family
#define KVDB_PLATFORM_LINUX             20
// Apple family
#define KVDB_PLATFORM_MAC               40
#define KVDB_PLATFORM_IOS               41
// Mobile family
#define KVDB_PLATFORM_ANDROID           50
#define KVDB_PLATFORM_BLACKBERRY        60
// Other systems
#define KVDB_PLATFORM_MARMALADE         100
#define KVDB_PLATFORM_BADA              101
#define KVDB_PLATFORM_NACL              102
#define KVDB_PLATFORM_EMSCRIPTEN        103
#define KVDB_PLATFORM_TIZEN             104

// Determine target platform by compile environment macro.
#undef  KVDB_TARGET_PLATFORM
#define KVDB_TARGET_PLATFORM            KVDB_PLATFORM_UNKNOWN

#ifndef KVDB_PLATFORM_NAME
#define KVDB_PLATFORM_NAME(X)           KVDB_PLATFORM_##X
#endif

#ifndef KVDB_CHECK_PLATFORM
    #ifdef KVDB_TARGET_PLATFORM
        #define KVDB_CHECK_PLATFORM(X)  ((KVDB_PLATFORM_##X) && (KVDB_TARGET_PLATFORM == KVDB_PLATFORM_##X))
    #else
        #define KVDB_CHECK_PLATFORM(X)  (KVDB_PLATFORM_##X)
    #endif
#endif  // KVDB_CHECK_PLATFORM

#ifndef KVDB_CHECK_OS
#define KVDB_CHECK_OS                   KVDB_CHECK_PLATFORM
#endif

// marmalade
#if defined(MARMALADE)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_MARMALADE
    #ifndef KVDB_IS_MARMALADE
    #define KVDB_IS_MARMALADE           1
    #endif
#endif

// bada
#if defined(SHP)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_BADA
    #ifndef KVDB_IS_BADA
    #define KVDB_IS_BADA                1
    #endif
#endif

// native client
#if defined(__native_client__)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_NACL
    #ifndef KVDB_IS_NACL
    #define KVDB_IS_NACL                1
    #endif
#endif

// Emscripten
#if defined(EMSCRIPTEN)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_EMSCRIPTEN
    #ifndef KVDB_IS_EMSCRIPTEN
    #define KVDB_IS_EMSCRIPTEN          1
    #endif
#endif

// tizen
#if defined(TIZEN)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_TIZEN
    #ifndef KVDB_IS_WINRT
    #define KVDB_IS_TIZEN               1
    #endif
#endif

// qnx
#if defined(__QNX__)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_BLACKBERRY
    #ifndef KVDB_IS_BLACKBERRY
    #define KVDB_IS_BLACKBERRY          1
    #endif
#endif

// android
#if defined(ANDROID)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_ANDROID
    #ifndef KVDB_IS_ANDROID
    #define KVDB_IS_ANDROID             1
    #endif
#endif

// iphone
#if defined(KVDB_TARGET_OS_IPHONE)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_IOS
    #ifndef KVDB_IS_OS_IPHONE
    #define KVDB_IS_OS_IPHONE           1
    #endif
#endif

// ipad, perhaps this precompiled entry is invalid.
#if defined(KVDB_TARGET_OS_IPAD)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_IOS
    #ifndef KVDB_IS_OS_IPAD
    #define KVDB_IS_OS_IPAD             1
    #endif
#endif

// mac
#if defined(KVDB_TARGET_OS_MAC)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_MAC
    #ifndef KVDB_IS_OS_MAC
    #define KVDB_IS_OS_MAC              1
    #endif
#endif

// linux
#if defined(__linux__) || defined(__linux) || defined(LINUX)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_LINUX
    // KVDB_IS_LINUX has defined below
    #ifndef KVDB_IS_OS_LINUX
    #define KVDB_IS_OS_LINUX            1
    #endif
#endif

// WinCE (Windows CE)
#if defined(WCE) || defined(_WCE) || defined(WINCE) || defined(_WIN32_WCE)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_WINCE
    #ifndef KVDB_IS_WINCE
    #define KVDB_IS_WINCE               1
    #endif
#endif

// WinRT (Windows Store App)
#if defined(WINRT) || defined(_WINRT)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_WINRT
    #ifndef KVDB_IS_WINRT
    #define KVDB_IS_WINRT               1
    #endif
#endif

// WP8 (Windows Phone 8 App)
#if defined(WP8) || defined(_WP8)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_WP8
    #ifndef KVDB_IS_WP8
    #define KVDB_IS_WP8                 1
    #endif
#endif

// win32
#if defined(WIN32) || defined(_WIN32) || defined(_WINDOWS) || defined(__WIN32__) || defined(__NT__)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_WIN32
    #ifndef KVDB_IS_WIN32
    #define KVDB_IS_WIN32               1
    #endif
#endif

// win64
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
    #undef  KVDB_TARGET_PLATFORM
    #define KVDB_TARGET_PLATFORM        KVDB_PLATFORM_WIN64
    #ifndef KVDB_IS_WIN64
    #define KVDB_IS_WIN64               1
    #endif
#endif

#if (defined(KVDB_IS_WIN32) || defined(KVDB_IS_WIN64)) && !(defined(__GNUC__) || defined(__MINGW__) \
    || defined(__MINGW32__) || defined(__MINGW64__))
#ifndef KVDB_IS_WINDOWS
#define KVDB_IS_WINDOWS                 (KVDB_CHECK_PLATFORM(WIN32) || KVDB_CHECK_PLATFORM(WIN64))
#endif
#endif

#if defined(KVDB_IS_WIN32) || defined(KVDB_IS_WIN64) || defined(KVDB_IS_WINCE) \
 || defined(KVDB_IS_WINRT) || defined(KVDB_IS_WP8)
#ifndef KVDB_IS_WINFAMILY
#define KVDB_IS_WINFAMILY               (KVDB_CHECK_PLATFORM(WIN32) || KVDB_CHECK_PLATFORM(WIN64) \
                                      || KVDB_CHECK_PLATFORM(WINCE) || KVDB_CHECK_PLATFORM(WINRT) \
                                      || KVDB_CHECK_PLATFORM(WP8))
#endif
#endif

#if defined(KVDB_IS_OS_UNIX)
#ifndef KVDB_IS_UNIX
#define KVDB_IS_UNIX                    (KVDB_CHECK_PLATFORM(LINUX))
#endif
#endif

#if defined(KVDB_IS_OS_LINUX)
#ifndef KVDB_IS_LINUX
#define KVDB_IS_LINUX                   (KVDB_CHECK_PLATFORM(LINUX))
#endif
#endif

#if defined(KVDB_IS_OS_IPHONE) || defined(KVDB_IS_OS_IPAD)
#ifndef KVDB_IS_IOS
#define KVDB_IS_IOS                     (KVDB_CHECK_PLATFORM(OS_IPHONE) || KVDB_CHECK_PLATFORM(OS_IPAD))
#endif
#endif

#if defined(KVDB_IS_OS_MAC)
#ifndef KVDB_IS_MAC
#define KVDB_IS_MAC                     (KVDB_CHECK_PLATFORM(OS_MAC))
#endif
#endif

#if defined(KVDB_IS_IOS) || defined(KVDB_IS_MAC)
#ifndef KVDB_IS_APPLE
#define KVDB_IS_APPLE                   (KVDB_CHECK_PLATFORM(IOS) || KVDB_CHECK_PLATFORM(MAC))
#endif
#endif

// for DOXYGEN
#if defined(DOXYGEN)
  #ifndef KVDB_IS_DOXYGEN
    #define KVDB_IS_DOXYGEN             1
  #endif
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if (!defined(KVDB_TARGET_PLATFORM)) || (KVDB_TARGET_PLATFORM == KVDB_PLATFORM_UNKNOWN)
    #error "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif

#if (KVDB_TARGET_PLATFORM == KVDB_PLATFORM_WIN32 || KVDB_TARGET_PLATFORM == KVDB_PLATFORM_WIN64)
#pragma warning (disable:4127)
#endif // KVDB_PLATFORM_WIN32

#endif // KVDB_BASIC_PLATFORM_H
