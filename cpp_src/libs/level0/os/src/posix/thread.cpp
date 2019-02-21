
#include "core/core.h"
#include "core/logger.h"
#include "os/thread.hpp"
//#include "../Interfaces/IMemoryManager.h"

#include <unistd.h>
#include <sys/sysctl.h>

Mutex::Mutex() {
  handle = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&handle, NULL);
}

Mutex::~Mutex() { pthread_mutex_destroy(&handle); }

void Mutex::Acquire() { pthread_mutex_lock(&handle); }

void Mutex::Release() { pthread_mutex_unlock(&handle); }

void *ThreadFunctionStatic(void *data) {
  WorkItem *pItem = static_cast<WorkItem *>(data);
  pItem->pFunc(pItem->pData);
  return 0;
}

ConditionVariable::ConditionVariable() {
  handle = PTHREAD_COND_INITIALIZER;
  int res = pthread_cond_init(&handle, NULL);
  ASSERT(res == 0);
}

ConditionVariable::~ConditionVariable() { pthread_cond_destroy(&handle); }

void ConditionVariable::Wait(const Mutex& mutex, unsigned int ms) {
  timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = ms * 1000;

  pthread_mutex_t *mutexHandle = (pthread_mutex_t *) &mutex.handle;
  pthread_cond_timedwait(&handle, mutexHandle, &ts);
}

void ConditionVariable::Set() { pthread_cond_signal(&handle); }

ThreadID Thread::mainThreadID;

/*  void Thread::SetPriority(int priority)
{
	  sched_param param;
	  param.sched_priority = priority;
	  pthread_setschedparam(pHandle, SCHED_OTHER, &param);
}*/

void Thread::SetMainThread() { mainThreadID = GetCurrentThreadID(); }

ThreadID Thread::GetCurrentThreadID() { return (ThreadID) pthread_self(); }

bool Thread::IsMainThread() { return GetCurrentThreadID() == mainThreadID; }

ThreadHandle create_thread(WorkItem *pData) {
  pthread_t handle;
  int res = pthread_create(&handle, NULL, ThreadFunctionStatic, pData);
  ASSERT(res == 0);
  return (ThreadHandle) handle;
}

void destroy_thread(ThreadHandle handle) {
  pthread_join(handle, NULL);
  handle = NULL;
}

void join_thread(ThreadHandle handle) { pthread_join(handle, NULL); }

void Thread::Sleep(unsigned mSec) { usleep(mSec * 1000); }

// threading class (Static functions)
unsigned int Thread::GetNumCPUCores(void) {
  size_t len;
  unsigned int ncpu;
  len = sizeof(ncpu);
  sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0);
  return ncpu;
}


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