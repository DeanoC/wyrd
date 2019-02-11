// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef WYRD_OS_THREAD_H
#define WYRD_OS_THREAD_H

#include "os/os.h"

#ifndef _WIN32
#include <pthread.h>
#endif

/// Operating system mutual exclusion primitive.
typedef struct Mutex_t
{
#ifdef _WIN32
	void* pHandle;
#else
	pthread_mutex_t pHandle;
#endif
} Mutex_t;

typedef struct ConditionVariable_t
{
#ifdef _WIN32
	void* pHandle;
#else
	pthread_cond_t pHandle;
#endif
} ConditionVariable_t;

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

#ifdef __cplusplus
#include "os/thread.hpp"
#endif

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
