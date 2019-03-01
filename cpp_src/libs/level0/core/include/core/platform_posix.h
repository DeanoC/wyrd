#ifndef _RUST_BINDGEN_
#include <stdlib.h>
#include <memory.h>
#endif

#include <stdbool.h>
#include <stdint.h>

#if defined(_RUST_BINDGEN_)
typedef unsigned long size_t;
#endif


#if __cplusplus >= 201103
#define DEFINE_ALIGNED(def, a) alignas(a) def
#elif defined(__OSX__)
#define DEFINE_ALIGNED(def, a) def __attribute__((aligned(a)))
#else
#define DEFINE_ALIGNED(def, a) def __align__(x)
#endif

#define EXPORT EXTERN_C
#define IMPORT EXTERN_C
#define CAPI
#define EXPORT_CPP

#define stricmp(a, b) strcasecmp(a, b)
#define vsprintf_s vsnprintf
#define strncpy_s strncpy
#define _strdup strdup
