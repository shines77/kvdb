
#ifndef KVDB_SYSTEM_CONSOLE_H
#define KVDB_SYSTEM_CONSOLE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "kvdb/basic/stdint.h"

#if defined(_WIN32) || defined(__cygwin__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif // _WIN32

namespace kvdb {

#if defined(_WIN32) || defined(__cygwin__)

/********************************************************

  fgColor:

    0  - BLACK
    1  - BLUE
    2  - GREEN
    3  - CYAN
    4  - RED
    5  - MAGENTA
    6  - BROWN
    7  - LIGHTGRAY

    8  - DARKGRAY
    9  - LIGHTBLUE
    10 - LIGHTGREEN
    11 - LIGHTCYAN
    12 - LIGHTRED
    13 - LIGHTMAGENTA
    14 - YELLOW
    15 - WHITE

********************************************************/

struct FgColor {
    enum {
        Black           = 0x00,
        Blue            = 0x01,
        Green           = 0x02,
        Cyan            = Blue | Green,
        Red             = 0x04,
        Magenta         = Red | Blue,
        Brown           = Red | Green,
        LightGray       = Red | Green | Blue,

        Intensity       = 0x08,

        Darkgray        = Intensity | Black,
        LightBlue       = Intensity | Blue,
        LightGreen      = Intensity | Green,
        LightCyan       = Intensity | Blue | Green,
        LightRed        = Intensity | Red,
        LightMagenta    = Intensity | Red | Blue,
        Yellow          = Intensity | Red | Green,
        White           = Intensity | Red | Green | Blue,
        Last
    };
};

struct BgColor {
    enum {
        Black           = 0x00,
        Blue            = 0x10,
        Green           = 0x20,
        Cyan            = Blue | Green,
        Red             = 0x40,
        Magenta         = Red | Blue,
        Brown           = Red | Green,
        LightGray       = Red | Green | Blue,

        Intensity       = 0x80,

        Darkgray        = Intensity | Black,
        LightBlue       = Intensity | Blue,
        LightGreen      = Intensity | Green,
        LightCyan       = Intensity | Blue | Green,
        LightRed        = Intensity | Red,
        LightMagenta    = Intensity | Red | Blue,
        Yellow          = Intensity | Red | Green,
        White           = Intensity | Red | Green | Blue,
        Last
    };
};

static WORD s_wConsoleAttributes = 0;

//
// Set windows console text foreground color
//
BOOL SetConsoleTextFgColor(DWORD fgColor)
{
    BOOL result = FALSE;

    // We will need this handle to get the current background attribute
    HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        // We use csbi for the wAttributes word.
        if (::GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
            // Mask out all but the background attribute, and add in the forgournd color
            WORD wColor = (csbi.wAttributes & 0xF0) | (fgColor & 0x0F);
            result = ::SetConsoleTextAttribute(hStdOut, wColor);
            if (result) {
                s_wConsoleAttributes = csbi.wAttributes;
            }
        }
    }

    return result;
}

//
// Set windows console text foreground color
//
BOOL SetConsoleTextBgColor(DWORD bgColor)
{
    BOOL result = FALSE;

    // We will need this handle to get the current background attribute
    HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        // We use csbi for the wAttributes word.
        if (::GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
            // Mask out all but the background attribute, and add in the forgournd color
            WORD wColor = (bgColor & 0xF0) | (csbi.wAttributes & 0x0F);
            result = ::SetConsoleTextAttribute(hStdOut, wColor);
            if (result) {
                s_wConsoleAttributes = csbi.wAttributes;
            }
        }
    }

    return result;
}

//
// Set windows console text foreground and background color
//
BOOL SetConsoleTextColor(DWORD fgColor, DWORD bgColor)
{
    BOOL result = FALSE;

    // We will need this handle to get the current background attribute
    HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        // We use csbi for the wAttributes word.
        if (::GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
            // Mask out all but the background attribute, and add in the forgournd color
            WORD wColor = (bgColor & 0xF0) | (fgColor & 0x0F);
            result = ::SetConsoleTextAttribute(hStdOut, wColor);
            if (result) {
                s_wConsoleAttributes = csbi.wAttributes;
            }
        }
    }

    return result;
}

BOOL RecoverConsoleTextColor()
{
    BOOL result = FALSE;

    // We will need this handle to get the current background attribute
    HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut != INVALID_HANDLE_VALUE) {
        result = ::SetConsoleTextAttribute(hStdOut, s_wConsoleAttributes);
    }

    return result;
}

#endif // _WIN32

} // namespace kvdb

#endif // KVDB_SYSTEM_CONSOLE_H