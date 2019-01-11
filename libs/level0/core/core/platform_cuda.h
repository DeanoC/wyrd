#if PLATFORM == NVGPU
#   undef assert
#   define assert(x)if(!(x))printf(#x);
#endif

#define READONLY_MEM	__constant__
#define LOCAL_MEM		__device__

#define CORE_ALIGN(x)  __align__(x)
#define INLINE
