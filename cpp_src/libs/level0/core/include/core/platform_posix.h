#	include <stdint.h>
#	include <stdlib.h>
#	include <memory.h>

#if __cplusplus >= 201103
#define DEFINE_ALIGNED(def, a) alignas(a) def
#elif defined(__OSX__)
#define DEFINE_ALIGNED(def, a) def __attribute__((aligned(a)))
#else
#define DEFINE_ALIGNED(def, a) def __align__(x)
#endif
