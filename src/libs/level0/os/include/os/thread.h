// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef WYRD_OS_THREAD_H
#define WYRD_OS_THREAD_H

#include "core/core.h"

#if PLATFORM == PLATFORM_WINDOWS

#if CPU_BIT_SIZE == 32
typedef struct { char dummy[24]; } Os_Mutex_t;
typedef struct { char dummy[4]; } Os_ConditionalVariable_t;
#elif CPU_BIT_SIZE == 64
typedef struct { char dummy[40]; } Os_Mutex_t;
typedef struct { char dummy[8]; } Os_ConditionalVariable_t;
#else
#error What bit size if this CPU?!
#endif

typedef unsigned int Os_ThreadID_t;
typedef void* Os_Thread_t;

#else

#include <pthread.h>

/// Operating system mutual exclusion primitive.
typedef pthread_mutex_t Os_Mutex_t;
typedef pthread_cond_t Os_ConditionalVariable_t;

typedef pthread_t Os_ThreadID_t;
typedef pthread_t Os_Thread_t;

#endif

typedef void (*Os_JobFunction_t)(void *);


EXTERN_C bool Os_MutexCreate(Os_Mutex_t *mutex);
EXTERN_C void Os_MutexDestroy(Os_Mutex_t *mutex);
EXTERN_C void Os_MutexAcquire(Os_Mutex_t *mutex);
EXTERN_C void Os_MutexRelease(Os_Mutex_t *mutex);
EXTERN_C bool Os_ConditionalVariableCreate(Os_ConditionalVariable_t *cd);
EXTERN_C void Os_ConditionalVariableDestroy(Os_ConditionalVariable_t *cd);
EXTERN_C void Os_ConditionalVariableWait(Os_ConditionalVariable_t *cd, Os_Mutex_t *mutex, uint64_t waitms);
EXTERN_C void Os_ConditionalVariableSet(Os_ConditionalVariable_t *cd);

EXTERN_C bool Os_ThreadCreate(Os_Thread_t *thread, Os_JobFunction_t func, void *data);
EXTERN_C void Os_ThreadDestroy(Os_Thread_t *thread);
EXTERN_C void Os_ThreadJoin(Os_Thread_t *thread);

EXTERN_C Os_ThreadID_t Os_GetCurrentThreadID(void);
EXTERN_C void Os_SetMainThread(void);
EXTERN_C bool Os_IsMainThread(void);

EXTERN_C void Os_Sleep(uint64_t waitms);
// Note in theory this can change at runtime on some platforms
EXTERN_C uint32_t Os_CPUCoreCount(void);

#endif //WYRD_OS_THREAD_H
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
*/
