#pragma once
#ifndef WYRD_OS_APPLE_COMMON_H
#define WYRD_OS_APPLE_COMMON_H

#if !defined(TARGET_IOS)
#import <Carbon/Carbon.h>
#else
#include <stdint.h>
typedef uint64_t uint64;
#endif // end !IOS

#define CALLTYPE

#endif //WYRD_COMMON_H
