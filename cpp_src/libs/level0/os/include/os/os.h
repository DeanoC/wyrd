#pragma once
#ifndef WYRD_OS_OS_H
#define WYRD_OS_OS_H

#if defined(__ANDROID__)
#include "os/android/common.h"
#elif defined(__APPLE__)
#include "os/apple/common.h"
#elif defined(__linux__)
#include "os/linux/common.h"
#elif defined(_WIN32)
#include "os/windows/common.h
#endif

#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <stddef.h>

#ifndef _WIN32
#define stricmp(a, b) strcasecmp(a, b)
#define vsprintf_s vsnprintf
#define strncpy_s strncpy
#endif

#include "os/rectdesc.h"
#include "os/windowsdesc.h"
#include "os/monitordesc.h"


// Define some sized types
typedef uint8_t uint8;
typedef   int8_t int8;

typedef uint16_t uint16;
typedef   int16_t  int16;

typedef uint32_t uint32;
typedef   int32_t  int32;
typedef ptrdiff_t intptr;

#ifdef _WIN32
typedef   signed __int64  int64;
typedef unsigned __int64 uint64;
#elif defined(__APPLE__)
typedef unsigned long DWORD;
typedef unsigned int UINT;
//typedef bool BOOL;
#elif defined(__linux__)
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef int64_t  int64;
typedef uint64_t uint64;
#else
typedef   signed long long  int64;
typedef unsigned long long uint64;
#endif

typedef uint8 ubyte;
typedef uint16 ushort;
typedef unsigned int uint;
typedef const char *LPCSTR, *PCSTR;

// API functions

// Init Close App
bool isRunning();
void requestShutDown();

//
// failure research ...
//
#include "os/platformevents.hpp"

#endif
/*
 * Copyright (c) 2018 Confetti Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/