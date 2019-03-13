// Copyright (c) 2013 Doug Binks
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

/*
* Copyright (c) 2018-2019 Confetti Interactive Inc.
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
*
* Added additional atomic primitived
*/
#pragma once

#ifndef WYRD_OS_ATOMICS_H
#define WYRD_OS_ATOMICS_H

#include "core/core.h"

#if PLATFORM == PLATFORM_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#undef GetObject
#include <intrin.h>

extern "C" void _ReadWriteBarrier();
#pragma intrinsic(_ReadWriteBarrier)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchangeAdd)

// Memory Barriers to prevent CPU and Compiler re-ordering
#define Os_MemoryBarrierAcquire() _ReadWriteBarrier()
#define Os_MemoryBarrierRelease() _ReadWriteBarrier()
#define OS_BASE_ALIGN(x) __declspec( align( x ) )

#else

#define Os_MemoryBarrierAcquire() __asm__ __volatile__("": : :"memory")
#define Os_MemoryBarrierRelease() __asm__ __volatile__("": : :"memory")
#define OS_BASE_ALIGN(x)  __attribute__ ((aligned( x )))

#endif

typedef volatile OS_BASE_ALIGN(4) uint32_t Os_atomic32_t;
typedef volatile OS_BASE_ALIGN(8) uint64_t Os_atomic64_t;

// Atomically performs: if( *pDest == compareWith ) { *pDest = swapTo; }
// returns old *pDest (so if successfull, returns compareWith)
inline uint32_t Os_AtomicCompareAndSwap32(volatile uint32_t *pDest, uint32_t swapTo, uint32_t compareWith) {
#ifdef _WIN32
  return _InterlockedCompareExchange( (volatile long*)pDest,swapTo, compareWith );
#else
  return __sync_val_compare_and_swap(pDest, compareWith, swapTo);
#endif
}

inline uint64_t Os_AtomicCompareAndSwap64(volatile uint64_t *pDest, uint64_t swapTo, uint64_t compareWith) {
#ifdef _WIN32
  return _InterlockedCompareExchange64( (__int64 volatile*)pDest, swapTo, compareWith );
#else
  return __sync_val_compare_and_swap(pDest, compareWith, swapTo);
#endif
}

inline void *Os_AtomicCompareAndSwapPtr(void *volatile *pDest, void *swapTo, void *compareWith) {
#ifdef _WIN32
  return _InterlockedCompareExchangePointer( pDest, swapTo, compareWith );
#else
  return __sync_val_compare_and_swap(pDest, compareWith, swapTo);
#endif
}

// exchange ptr and return previous value
inline void *Os_AtomicExchangePtr(void *volatile *pDest, void *swapTo) {
#ifdef _WIN32
  return _InterlockedExchangePointer( pDest, swapTo );
#else
  return __sync_lock_test_and_set(pDest, swapTo);
#endif
}

// Atomically performs: tmp = *pDest; *pDest += value; return tmp;
inline int32_t Os_AtomicAdd32(volatile uint32_t *pDest, uint32_t value) {
#ifdef _WIN32
  return _InterlockedExchangeAdd( (long*)pDest, value );
#else
  return __sync_fetch_and_add(pDest, value);
#endif
}

// Atomically performs: tmp = *pDest; *pDest += value; return tmp;
inline uint64_t Os_AtomicAdd64(volatile uint64_t *pDest, uint64_t value) {
#ifdef _WIN32
  return (uint64_t)_InterlockedExchangeAdd64((LONG64*)pDest, value);
#else
  return __sync_fetch_and_add(pDest, value);
#endif
}

inline uint64_t Os_AtomicUpdateMax(volatile uint64_t *pDest, uint64_t value) {
  uint64_t prev_value = value;
  do { prev_value = Os_AtomicCompareAndSwap64(pDest, value, prev_value); }
  while (prev_value < value);
  return prev_value;
}

EXTERN_C inline uint32_t Os_AtomicAdd32_relaxed(volatile uint32_t *pDest, uint32_t value) {
  return Os_AtomicAdd32(pDest, value);
}

inline uint64_t Os_AtomicAdd64_relaxed(volatile uint64_t *pDest, uint64_t value) {
  return Os_AtomicAdd64(pDest, value);
}

#define Os_AtomicLoad32_relaxed(pVar) (*pVar)
#define Os_AtomicStore32_relaxed(pVar, val) ((*pVar) = (val))
#define Os_AtomicLoad64_relaxed(pVar) (*pVar)
#define Os_AtomicStore64_relaxed(pVar, val) ((*pVar) = (val))

inline uint64_t Os_AtomicLoad64_acquire(Os_atomic64_t *pVar) {
  uint64_t value = Os_AtomicLoad64_relaxed(pVar);
  Os_MemoryBarrierAcquire();
  return value;
}

inline void Os_AtomicStore64_release(Os_atomic64_t *pVar, uint64_t val) {
  Os_MemoryBarrierRelease();
  Os_AtomicStore64_relaxed(pVar, val);
}

#endif //WYRD_ATOMICS_H
