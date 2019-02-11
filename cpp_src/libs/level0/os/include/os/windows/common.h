#pragma once
#ifndef WYRD_OS_WINDOWS_COMMON_H
#define WYRD_OS_WINDOWS_COMMON_H

#if !defined(_DURANGO)

#include <sys/stat.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#endif
#include <stdlib.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1

#else

#define stricmp(a, b) _stricmp(a, b)

#endif // end !_DURANGO


#include <windows.h>
typedef HINSTANCE HINST;
#define CALLTYPE __cdecl
typedef unsigned int ThreadID;

#endif //WYRD_COMMON_H
