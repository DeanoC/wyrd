#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdbool.h>

//#	define _CRTDBG_MAP_ALLOC
//#	cppinclude <crtdbg.h>


#define READONLY_MEM
#define LOCAL_MEM
#define ALIGN(x)  __align__(x)

#define EXPORT EXTERN_C __declspec(dllexport)
#define IMPORT EXTERN_C __declspec(dllimport)
#define CAPI __declspec(cdecl)
#define EXPORT_CPP __declspec(dllexport)

#define UNREF_PARAM(x) (x)

#if __cplusplus >= 201103
#define DEFINE_ALIGNED(def, a) alignas(a) def
#else
#define DEFINE_ALIGNED(def, a) __declspec(align(a)) def
#endif

