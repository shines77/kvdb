
#ifndef BOOST_ASIO_MSVC_H
#define BOOST_ASIO_MSVC_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_WIN32) || defined(WIN32) || defined(OS_WINDOWS) || defined(_WINDOWS)
#  if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0501)
#    define _WIN32_WINNT	0x0601
#  endif
#endif

#if defined(_MSC_VER)
#  ifndef BOOST_ASIO_MSVC
#    define BOOST_ASIO_MSVC   _MSC_VER
#  endif
#endif

#if defined(BOOST_ASIO_MSVC)
#  if (_MSC_VER >= 1900)
#    ifndef BOOST_ASIO_ERROR_CATEGORY_NOEXCEPT
#      define BOOST_ASIO_ERROR_CATEGORY_NOEXCEPT noexcept(true)
#    endif
#  endif // (_MSC_VER >= 1900)
#endif // defined(BOOST_ASIO_MSVC)

#endif // BOOST_ASIO_MSVC_H
