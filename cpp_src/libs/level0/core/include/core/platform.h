#pragma once

#ifndef CORE_PLATFORM_H
#define CORE_PLATFORM_H

// platform options
#define PLATFORM_WINDOWS                  (0x0)
#define PLATFORM_APPLE_MAC                (0x1)
#define PLATFORM_IPHONE                   (0x2)
#define PLATFORM_UNIX                     (0x3)

// OS options
#define OS_CUSTOM                         (0x0)
#define OS_WINDOWS                        (0x1)
#define OS_OSX                            (0x2)
#define OS_GNULINUX                       (0x3)

// compiler family
#define COMPILER_MSVC                     (0x0)
#define COMPILER_GCC                      (0x1)
#define COMPILER_CLANG                    (0x2)

// endianess
#define CPU_LITTLE_ENDIAN         (0x0)
#define CPU_BIG_ENDIAN            (0x1)

#define CPU_X86                   (0x0)
#define CPU_X64                   (0x1)
#define CPU_ARM                   (0x2)

//--------------------------------------------------------
// Identification and classification from Compiler Defines
// Most of this info is from http://predef.sourceforge.net
//--------------------------------------------------------

// Processor and endian-ness identification
#if defined( i386 ) || defined( __i386__ ) || defined( __i386 ) || \
    defined( _M_IX86 ) || defined( __X86__ ) || defined( _X86_ ) || \
    defined( __THW_INTEL__ ) || defined( __I86__ ) || defined( __INTEL__ )
#	define CPU_FAMILY						CPU_X86
#	define CPU_ENDIANESS		 			CPU_LITTLE_ENDIAN
#	define CPU_BIT_SIZE						32
#elif defined( _M_X64 ) || defined( __amd64__ ) || defined( __amd64 ) || \
    defined( __x86_64__ ) || defined( __x86_64 )
#	define CPU_FAMILY                        CPU_X64
#	define CPU_ENDIANESS                    CPU_LITTLE_ENDIAN
#	define CPU_BIT_SIZE                        64
#elif defined(__aarch64__)
#	define CPU_FAMILY		 				CPU_ARM
#	if defined(__AARCH64EB__)
#		define CPU_ENDIANESS		 			CPU_BIG_ENDIAN
#	else
#		define CPU_ENDIANESS		 			CPU_LITTLE_ENDIAN
#	endif

#	define CPU_BIT_SIZE						64
#elif defined( __arm__ ) || defined( __thumb__ ) || defined( __TARGET_ARCH_ARM ) || defined( __TARGET_ARCH_THUMB ) || defined( _ARM )
#	define CPU_FAMILY		 				CPU_ARM
#	if defined(__ARMEB__) || defined(__THUMBEB__)
#		define CPU_ENDIANESS		 			CPU_BIG_ENDIAN
#	else
#		define CPU_ENDIANESS		 			CPU_LITTLE_ENDIAN
#	endif
#	define CPU_BIT_SIZE						32
#endif


// compiler identifcation
#if defined( _MSC_VER ) && !defined(__clang__)

// compiler version used with above
#define MS_VS2005                 (0x0)
#define MS_VS2008                 (0x1)
#define MS_VS2010                 (0x2)
#define MS_VS2012                 (0x3)
#define MS_VS2013                 (0x4)
#define MS_VS2015                 (0x5)
#define MS_VS2017                 (0x6)

// Minimum we support is VS 2005 (VS8 AKA 1400)
#	if _MSC_VER < 1400
#		error Not supported
#	endif // end _MSCVER < 1400

#	define COMPILER							COMPILER_MSVC
#	if _MSC_VER < 1500
#error Unsupported MSVC Compiler
#	elif _MSC_VER < 1600
#		define COMPILER_VERSION				MS_VS2008
#	elif _MSC_VER < 1700
#		define COMPILER_VERSION				MS_VS2010
#	elif _MSC_VER < 1800
#		define COMPILER_VERSION				MS_VS2012
#	elif _MSC_VER < 1900
#		define COMPILER_VERSION				MS_VS2013
#	elif _MSC_VER < 1910
#		define COMPILER_VERSION				MS_VS2015
#	else
#		define COMPILER_VERSION				MS_VS2017
#	endif

#elif defined( __GNUC__ ) && !defined(__clang__)
#	define COMPILER							COMPILER_GCC
#define GCC_V2                    (0x0)
#define GCC_V3                    (0x1)
#define GCC_V4                    (0x2)
#define GCC_V4_3                  (0x3)

#elif defined( __clang__ )
#	define COMPILER                            COMPILER_CLANG
#else
#	error Not supported
#endif

// OS
#if defined( WIN32 )
#	define PLATFORM 					PLATFORM_WINDOWS
#	define PLATFORM_OS					OS_WINDOWS
#elif defined(__APPLE__) && defined( __MACH__ )

#	include "TargetConditionals.h"
#	if TARGET_OS_IPHONE
#		define PLATFORM 				PLATFORM_IPHONE
#	else
#		define PLATFORM                    PLATFORM_APPLE_MAC
#	endif
#	define PLATFORM_OS                    OS_OSX

// override endianness with the OS_OSX one, hopefully right...
#	undef CPU_ENDIANESS
#	define CPU_ENDIANESS                (TARGET_RT_LITTLE_ENDIAN == 1)

#elif    defined( __unix__ ) || defined( __unix ) || \
        defined( __sysv__ ) || defined( __SVR4 ) || defined( __svr4__ ) || defined( _SYSTYPE_SVR4 ) || \
        defined( __FreeBSD__ ) || defined( __NetBSD__ ) || defined( __OpenBSD__ ) || defined( __bsdi__ ) || defined ( __DragonFly__ ) || defined( _SYSTYPE_BSD ) || \
        defined( sco ) || defined( _UNIXWARE7 ) || defined( ultrix ) || defined( __ultrix ) || defined( __ultrix__ ) || \
        defined( __osf__ ) || defined( __osf ) || defined( sun ) || defined( __sun ) || \
        defined( M_XENIX ) || defined( _SCO_DS ) || defined( sinux ) || defined( __minix ) || \
        defined( linux ) || defined( __linux ) || \
        defined( sgi ) || defined( __sgi ) || defined( __BEOS__ ) || defined (_AIX )

#	define PLATFORM						PLATFORM_UNIX

#	if defined( linux ) || defined( __linux )
#		define PLATFORM_OS					OS_GNULINUX
#	elif defined( __FreeBSD__ ) || defined( __NetBSD__ ) || defined( __OpenBSD__ ) || defined( __bsdi__ ) || defined ( __DragonFly__ ) || defined( _SYSTYPE_BSD )
#		define PLATFORM_OS					FREEBSD
#	else
#		error Not supported
#	endif

#elif defined( __clang__ )
// TODO pretend we are linux for now, as use clang on multi-os sa code checker
#define PLATFORM					PLATFORM_UNIX
#define PLATFORM_OS					OS_GNULINUX
#undef _MSC_VER
#else
#	error Not supported
#endif // endif COMPILER
#endif
