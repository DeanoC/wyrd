#pragma once
#ifndef WYRD_OS_TIME_H
#define WYRD_OS_TIME_H

// For time related functions such as getting localtime
#include <time.h>

// Time related functions
unsigned getSystemTime();
unsigned getTimeSinceStart();

#ifdef _WIN32
void sleep(unsigned mSec);
#endif

// High res timer functions
int64_t getUSec();
int64_t getTimerFrequency();

#endif //WYRD_OS_TIME_H
