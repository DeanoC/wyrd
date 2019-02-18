#ifndef __STDC_LIMIT_MACROS
#	define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
#	define __STDC_CONSTANT_MACROS
#endif

#include <inttypes.h>
#include <stdbool.h>
#include "platform_posix.h"

inline char* _strdup(char const* str)
{
	return strdup(str);
}

