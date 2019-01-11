//
// Created by Computer on 23/07/2018.
//

#pragma once

#ifndef CORE_CORE_H
#define CORE_CORE_H


#include "platform.h"
#if COMPILER == CUDA_COMPILER
#include "platform_cuda.h"
#elif PLATFORM_OS == MS_WINDOWS
#include "platform_win.h"
#elif PLATFORM_OS == OSX
#include "platform_osx.h"
#elif PLATFORM_OS == GNULINUX
#include "platform_linux.h"
#elif PLATFORM_OS == FREEBSD
#include "platform_posix.h"
#endif

#if PLATFORM == WINDOWS && !defined(USING_STATIC_LIBS)
#define EXPORT extern "C" __declspec(dllexport)
#define IMPORT extern "C" __declspec(dllimport)
#define CAPI __declspec(cdecl)
#define EXPORT_CPP __declspec(dllexport)
#else
#define EXPORT extern "C"
#define IMPORT extern "C"
#define CAPI
#define EXPORT_CPP
#endif

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

#endif //CORE_CORE_H
