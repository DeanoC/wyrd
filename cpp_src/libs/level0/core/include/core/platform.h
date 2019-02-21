#pragma once

#ifndef CORE_PLATFORM_H
#define CORE_PLATFORM_H

// platform options
#define PLATFORM_WINDOWS    (0)
#define PLATFORM_APPLE_MAC  (1)
#define PLATFORM_IPHONE     (2)
#define PLATFORM_UNIX       (3)
#define PLATFORM_ANDROID    (4)

// OS options
#define OS_CUSTOM         (0)
#define OS_WINDOWS        (1)
#define OS_OSX            (2)
#define OS_GNULINUX       (3)
#define OS_FREEBSD        (4)
#define OS_ANDROID        (5)

// compiler family
#define COMPILER_MSVC     (0)
#define COMPILER_GCC      (1)
#define COMPILER_CLANG    (2)

// endianess
#define CPU_LITTLE_ENDIAN (0)
#define CPU_BIG_ENDIAN    (1)

#define CPU_X86           (0)
#define CPU_X64           (1)
#define CPU_ARM           (2)

//--------------------------------------------------------
// Identification and classification from Compiler Defines
// Most of this info is from http://predef.sourceforge.net
//--------------------------------------------------------

// Processor and endian-ness identification
#if defined( i386 ) || defined( __i386__ ) || defined( __i386 ) || \
    defined( _M_IX86 ) || defined( __X86__ ) || defined( _X86_ ) || \
    defined( __THW_INTEL__ ) || defined( __I86__ ) || defined( __INTEL__ )
#define CPU_FAMILY    CPU_X86
#define CPU_ENDIANESS CPU_LITTLE_ENDIAN
#define CPU_BIT_SIZE  32
#elif defined( _M_X64 ) || defined( __amd64__ ) || defined( __amd64 ) || \
    defined( __x86_64__ ) || defined( __x86_64 )
#define CPU_FAMILY    CPU_X64
#define CPU_ENDIANESS CPU_LITTLE_ENDIAN
#define CPU_BIT_SIZE  64

#elif defined(__aarch64__)

#define CPU_FAMILY		CPU_ARM
#if defined(__AARCH64EB__)
#define CPU_ENDIANESS	CPU_BIG_ENDIAN
#else
#define CPU_ENDIANESS	CPU_LITTLE_ENDIAN
#endif
#define CPU_BIT_SIZE	64

#elif defined( __arm__ ) || defined( __thumb__ ) || defined( __TARGET_ARCH_ARM ) || defined( __TARGET_ARCH_THUMB ) || defined( _ARM )

#define CPU_FAMILY      CPU_ARM
#if defined(__ARMEB__) || defined(__THUMBEB__)
#define CPU_ENDIANESS	CPU_BIG_ENDIAN
#else
#define CPU_ENDIANESS	CPU_LITTLE_ENDIAN
#endif
#define CPU_BIT_SIZE	32

#endif // end CPU Id


// compiler identifcation
#if defined( _MSC_VER ) && !defined(__clang__)

// compiler version used with above
#define MS_VS2012                 (12)
#define MS_VS2013                 (13)
#define MS_VS2015                 (15)
#define MS_VS2017                 (17)

// Minimum we support is VS 2012
#if _MSC_VER < 1800
#error Not supported
#endif // end _MSCVER < 1800

#define COMPILER				COMPILER_MSVC

#if _MSC_VER < 1800
#define COMPILER_VERSION		MS_VS2012
#elif _MSC_VER < 1900
#define COMPILER_VERSION		MS_VS2013
#elif _MSC_VER < 1910
#define COMPILER_VERSION		MS_VS2015
#else
#define COMPILER_VERSION		MS_VS2017
#endif

#elif defined( __GNUC__ ) && !defined(__clang__)
#define COMPILER				  COMPILER_GCC
#define GCC_V2                    (0)
#define GCC_V3                    (1)
#define GCC_V4                    (2)
#define GCC_V4_3                  (3)

#elif defined( __clang__ )
#define COMPILER                  COMPILER_CLANG
#else
#error Not supported
#endif

// OS
#if defined( WIN32 )

#	define PLATFORM 				PLATFORM_WINDOWS
#	define PLATFORM_OS				OS_WINDOWS

#include "core/platform_win.h"

#elif defined(__APPLE__) && defined( __MACH__ )

#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define PLATFORM    PLATFORM_IPHONE
#else
#define PLATFORM    PLATFORM_APPLE_MAC
#endif
#define PLATFORM_OS OS_OSX

// override endianness with the OS_OSX one, hopefully right...
#undef CPU_ENDIANESS
#define CPU_ENDIANESS (TARGET_RT_LITTLE_ENDIAN == 1)

#include "core/platform_osx.h"

#elif defined(__ANDROID__)

#define PLATFORM        PLATFORM_ANDROID
#define PLATFORM_OS		OS_ANDROID

#include "core/platform_android.h"

#elif    defined( __unix__ ) || defined( __unix ) || \
        defined( __sysv__ ) || defined( __SVR4 ) || defined( __svr4__ ) || defined( _SYSTYPE_SVR4 ) || \
        defined( __FreeBSD__ ) || defined( __NetBSD__ ) || defined( __OpenBSD__ ) || defined( __bsdi__ ) || defined ( __DragonFly__ ) || defined( _SYSTYPE_BSD ) || \
        defined( sco ) || defined( _UNIXWARE7 ) || defined( ultrix ) || defined( __ultrix ) || defined( __ultrix__ ) || \
        defined( __osf__ ) || defined( __osf ) || defined( sun ) || defined( __sun ) || \
        defined( M_XENIX ) || defined( _SCO_DS ) || defined( sinux ) || defined( __minix ) || \
        defined( linux ) || defined( __linux ) || \
        defined( sgi ) || defined( __sgi ) || defined( __BEOS__ ) || defined (_AIX )

#define PLATFORM PLATFORM_UNIX

#if defined( linux ) || defined( __linux )

#define PLATFORM_OS		OS_GNULINUX
#include "core/platform_linux.h"

#elif defined( __FreeBSD__ ) || defined( __NetBSD__ ) || defined( __OpenBSD__ ) || defined( __bsdi__ ) || defined ( __DragonFly__ ) || defined( _SYSTYPE_BSD )

#define PLATFORM_OS		OS_FREEBSD
#include "core/platform_posix.h"

#else // unknown unix
#error Not supported
#endif

#else // unknown PLATFORM

#error unknown platform

#endif // endif OS

#endif
