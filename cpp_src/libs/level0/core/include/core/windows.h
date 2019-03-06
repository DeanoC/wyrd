#pragma once
#ifndef WYRD_CORE_WINDOWS_H
#define WYRD_CORE_WINDOWS_H

#define _CRT_SECURE_NO_WARNINGS
#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers
#undef NOMINMAX
#define NOMINMAX

#undef _WIN32_WINNT
#include <sdkddkver.h>
#undef _WIN32_WINNT
#undef _NTDDI_VERSION

#define _WIN32_WINNT _WIN32_WINNT_WINBLUE
#define _NTDDI_VERSION NTDDI_WINBLUE

#include <windows.h>
#undef ERROR
#undef EXTERN_C

#endif //WYRD_CORE_WINDOWS_H
