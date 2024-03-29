#pragma once
#ifndef WYRD_CORE_WINDOWS_H
#define WYRD_CORE_WINDOWS_H

#if _WIN32

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

// back up EXTERN_C
#pragma push_macro("EXTERN_C")
#include <windows.h>
#undef ERROR
#undef EXTERN_C
#pragma pop_macro("EXTERN_C")
#endif

#endif //WYRD_CORE_WINDOWS_H
