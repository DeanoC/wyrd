#if COMPILER_VERSION < MS_VS2010
#	cppinclude "pstdint.h"
#	else
#	include <stdint.h>
#endif

//#	define _CRTDBG_MAP_ALLOC
//#	cppinclude <crtdbg.h>

#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
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

// C RunTime Header Files
#include <cstdlib>
#include <cassert>

#define READONLY_MEM
#define LOCAL_MEM
#define ALIGN(x)  __align__(x)

#ifdef _MSC_VER    //If on Visual Studio
#define UNREF_PARAM(x) (x)
#else
//Add more compilers and platforms as we need them
#define UNREF_PARAM(x)
#endif

#if __cplusplus >= 201103
#define DEFINE_ALIGNED(def, a) alignas(a) def
#else
#define DEFINE_ALIGNED(def, a) __declspec(align(a)) def
#endif

#if __cplusplus >= 201103
#define DEFINE_ALIGNED(def, a) alignas(a) def
#elif defined(__OSX__)
#define DEFINE_ALIGNED(def, a) def __attribute__((aligned(a)))
#else
//If we haven't specified the platform here, we fallback on the C++11 and C11 keyword for aligning
//Best case -> No platform specific align defined -> use this one that does the same thing
//Worst case -> No platform specific align defined -> this one also doesn't work and fails to compile -> add a platform specific one :)
#define DEFINE_ALIGNED(def, a) alignas(a) def
#endif
#endif