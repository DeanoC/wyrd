#pragma once
#ifndef WYRD_LOGMANAGER_LOGMANAGER_H
#define WYRD_LOGMANAGER_LOGMANAGER_H

#include "core/core.h"

typedef struct{}* LogMan_Handle;

EXTERN_C LogMan_Handle LogMan_Alloc();
EXTERN_C void LogMan_Free(LogMan_Handle handle);
EXTERN_C void LogMan_Open(LogMan_Handle handle, char const* fileName);
EXTERN_C void LogMan_Close(LogMan_Handle handle);
EXTERN_C void LogMan_SetTimeStamp(LogMan_Handle handle, bool enable);
EXTERN_C bool LogMan_GetTimeStamp(LogMan_Handle handle);
EXTERN_C void LogMan_SetInfoQuiet(LogMan_Handle handle, bool quiet);
EXTERN_C void LogMan_SetWarningQuiet(LogMan_Handle handle, bool quiet);
EXTERN_C void LogMan_SetErrorQuiet(LogMan_Handle handle, bool quiet);
EXTERN_C bool LogMan_IsInfoQuiet(LogMan_Handle handle);
EXTERN_C bool LogMan_IsWarningQuiet(LogMan_Handle handle);
EXTERN_C bool LogMan_IsErrorQuiet(LogMan_Handle handle);

#endif //WYRD_LOGMANAGER_LOGMANAGER_H
