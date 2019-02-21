#pragma once
#ifndef WYRD_LOGGER_H
#define WYRD_LOGGER_H

#include "core/core.h"

#define USE_LOGGING

typedef struct Core_Logger_t {

	// lowest level of logging, (file, line, message)
	void (*errorMsg)(char const* file, int line, const char* function, char const* msg);
	void (*warningMsg)(char const* file, int line, const char* function, char const* msg);
	void (*infoMsg)(char const* file, int line, const char* function, char const* msg);
	void (*debugMsg)(char const* file, int line, const char* function, char const* msg);

	void (*failedAssert)(char const* file, int line, char const* msg);

} Core_Logger;

EXTERN_C Core_Logger Core_Log;

#if PLATFORM != PLATFORM_WINDOWS
EXTERN_C void OutputDebugString(char const *msg);
#endif

EXTERN_C void _ErrorMsg(const char* file, int line, const char* function, const char* string, ...);
EXTERN_C void _WarningMsg(const char* file, int line, const char* function, const char* string, ...);
EXTERN_C void _InfoMsg(const char* file, int line, const char* function, const char* string, ...);
EXTERN_C void _DebugMsg(const char* file, int line, const char* function, const char* string, ...);

EXTERN_C void _FailedAssert(const char* file, int line, const char* statement);

#define ErrorMsg(str, ...) _ErrorMsg(__LINE__, __FILE__, __FUNCTION__, str, ##__VA_ARGS__)
#define WarningMsg(str, ...) _WarningMsg(__LINE__, __FILE__, __FUNCTION__, str, ##__VA_ARGS__)
#define InfoMsg(str, ...) _InfoMsg(__LINE__, __FILE__, __FUNCTION__, str, ##__VA_ARGS__)

#ifdef _DEBUG
#define DebugMsg(str, ...) _DebugMsg(__LINE__, __FILE__, __FUNCTION__, str, ##__VA_ARGS__)

#define IFASSERT(x) x

#if defined(ORBIS)
// there is a large amount of stuff included via header files ...
#define ASSERT(cond) SCE_GNM_ASSERT(cond)
#else

#define ASSERT(b) \
	if (b) {} else { _FailedAssert(__FILE__, __LINE__, #b); }
#endif

#else

#define DebugMsg(str, ...)
#define ASSERT(b)
#define IFASSERT(x)

#endif    // DEBUG

#ifdef USE_LOGGING
#define LOGDEBUG(message) _DebugMsg( __FILE__, __LINE__, __FUNCTION__, message)
#define LOGINFO(message) _InfoMsg( __FILE__, __LINE__, __FUNCTION__, message)
#define LOGWARNING(message) _WarningMsg( __FILE__, __LINE__, __FUNCTION__, message)
#define LOGERROR(message) _ErrorMsg( __FILE__, __LINE__, __FUNCTION__, message)

#define LOGDEBUGF(message, ...) _DebugMsg( __FILE__, __LINE__, __FUNCTION__, message, ##__VA_ARGS__)
#define LOGINFOF(message, ...) _InfoMsg( __FILE__, __LINE__, __FUNCTION__, message, ##__VA_ARGS__)
#define LOGWARNINGF(message, ...) _WarningMsg( __FILE__, __LINE__, __FUNCTION__, message, ##__VA_ARGS__)
#define LOGERRORF(message, ...) _ErrorMsg( __FILE__, __LINE__, __FUNCTION__, message, ##__VA_ARGS__)

#else
#define LOGDEBUG(message) ((void)0)
#define LOGINFO(message) ((void)0)
#define LOGWARNING(message) ((void)0)
#define LOGERROR(message) ((void)0)

#define LOGDEBUGF(...) ((void)0)
#define LOGINFOF(...) ((void)0)
#define LOGWARNINGF(...) ((void)0)
#define LOGERRORF(...) ((void)0)
#endif

#endif //WYRD_LOGGER_H
