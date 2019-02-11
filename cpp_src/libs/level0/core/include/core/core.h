#pragma once

#ifndef CORE_CORE_H
#define CORE_CORE_H

#include "core/platform.h"
#if COMPILER == CUDA_COMPILER
#include "platform_cuda.h"
#elif PLATFORM_OS == MS_WINDOWS
#include "platform_win.h"
#elif PLATFORM_OS == OSX
#include "core/platform_osx.h"
#elif PLATFORM_OS == GNULINUX
#include "platform_linux.h"
#elif PLATFORM_OS == FREEBSD
#include "platform_posix.h"
#endif

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#if PLATFORM == WINDOWS && !defined(USING_STATIC_LIBS)
#define EXPORT EXTERN_C __declspec(dllexport)
#define IMPORT EXTERN_C __declspec(dllimport)
#define CAPI __declspec(cdecl)
#define EXPORT_CPP __declspec(dllexport)
#else
#define EXPORT EXTERN_C
#define IMPORT EXTERN_C
#define CAPI
#define EXPORT_CPP
#endif

#endif //CORE_CORE_H
