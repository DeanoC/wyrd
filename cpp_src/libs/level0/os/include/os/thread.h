// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef WYRD_OS_THREAD_H
#define WYRD_OS_THREAD_H

#include "core/core.h"

#if PLATFORM == PLATFORM_WINDOWS

typedef void* Mutex_t;
typedef void* ConditionVariable_t;
typedef unsigned int ThreadID;

#else

#include <pthread.h>

/// Operating system mutual exclusion primitive.
typedef pthread_mutex_t Os_Mutex_t;
typedef pthread_cond_t Os_ConditionVariable_t;

typedef pthread_t ThreadID;

#endif

typedef void(*JobFunction)(void*);

/// Work queue item.
typedef struct WorkItem_t
{
	/// Work item description and thread index (Main thread => 0)
	JobFunction	 pFunc;
	void*		   pData;
	unsigned		mPriority;
	volatile bool   mCompleted;
} WorkItem_t;

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
