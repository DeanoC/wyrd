
#include "core/core.h"
#include "core/logger.h"

#include <stdio.h> // for sprintf family
#include <stdarg.h> // for va_xxx functions

#if PLATFORM != PLATFORM_WINDOWS
void OutputDebugString(char const* msg)
{
	printf("%s", msg);
}
#endif

#define MSG_MAX 1024

static void errorMsg(char const* file, int line, const char* function, char const* msg)
{
	char out[MSG_MAX];
	sprintf(out, "%s(%i): ERR (%s) - %s", file, line, function, msg);
	OutputDebugString(out);
}

static void warningMsg(char const* file, int line, const char* function, char const* msg)
{
	char out[MSG_MAX];
	sprintf(out, "%s(%i): WARN (%s) - %s", file, line, function, msg);
	OutputDebugString(out);
}

static void infoMsg(char const* file, int line, const char* function, char const* msg)
{
	char out[MSG_MAX];
	sprintf(out, "%s(%i): INFO (%s) - %s", file, line, function, msg);
	OutputDebugString(out);
}

static void debugMsg(char const* file, int line, const char* function, char const* msg)
{
	char out[MSG_MAX];
	sprintf(out, "%s(%i): DBG (%s) - %s", file, line, function, msg);
	OutputDebugString(out);
}

static void failedAssert(char const* file, int line, char const* msg)
{
	char out[MSG_MAX];
	sprintf(out, "%s(%i): Assert Failed - %s", file, line, msg);
	OutputDebugString(out);
}

EXTERN_C void _ErrorMsg(const char* file, int line, const char* function, const char* string, ...)
{
	va_list arglist;

	char out[MSG_MAX];
	va_start(arglist, string);
	vsprintf(out, string, arglist);
	va_end(arglist);

	Log.errorMsg(file, line, function, out);
}

EXTERN_C void _WarningMsg(const char* file, int line, const char* function, const char* string, ...)
{
	va_list arglist;

	char out[MSG_MAX];
	va_start(arglist, string);
	vsprintf(out, string, arglist);
	va_end(arglist);

	Log.warningMsg(file, line, function, out);

}

EXTERN_C void _InfoMsg(const char* file, int line, const char* function, const char* string, ...)
{
	va_list arglist;

	char out[MSG_MAX];
	va_start(arglist, string);
	vsprintf(out, string, arglist);
	va_end(arglist);

	Log.infoMsg(file, line, function, out);

}

EXTERN_C void _DebugMsg(const char* file, int line, const char* function, const char* string, ...)
{
#ifdef _DEBUG
	Log.warningMsg(file, line, function, "DebugMsg called in a non-debug version")
#else
	va_list arglist;

	char out[MSG_MAX];
	va_start(arglist, string);
	vsprintf(out, string, arglist);
	va_end(arglist);

	Log.debugMsg(file, line, function, out);
#endif
}

EXTERN_C void _FailedAssert(const char* file, int line, const char* statement)
{
	Log.failedAssert(file, line, statement);
}

EXTERN_C Logger Log = {
	&errorMsg,
	&warningMsg,
	&infoMsg,
	&debugMsg,
	&failedAssert
};

