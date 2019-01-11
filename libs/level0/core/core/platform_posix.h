#	include <cstdint>
#	include <cstdlib>
#	include <memory.h>

#define READONLY_MEM
#define LOCAL_MEM

#ifdef ALIGN
#undef ALIGN
#endif

#define ALIGN(x)  __align__(x)
#define INLINE inline