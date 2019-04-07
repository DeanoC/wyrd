#pragma once
#ifndef WYRD_OS_TIME_H
#define WYRD_OS_TIME_H

#include "core/core.h"

// Time related functions
uint64_t Os_GetSystemTime();
uint64_t Os_GetTimeSinceStart();

// High res timer functions
int64_t Os_GetUSec();

#endif //WYRD_OS_TIME_H
