// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef WYRD_OS_THREAD_HPP
#define WYRD_OS_THREAD_HPP

#include "core/core.h"
#include "os/thread.h"

namespace Os {

struct Mutex {
  Mutex() { Os_MutexCreate(&handle); };
  ~Mutex() { Os_MutexDestroy(&handle); };

  // take ownership of a C level mutex
  explicit Mutex(Os_Mutex_t mutie) : handle(mutie) {};

  void Acquire() { Os_MutexAcquire(&handle); };
  void Release() { Os_MutexRelease(&handle); };

  Os_Mutex_t handle;
};

struct ConditionalVariable {
  ConditionalVariable() { Os_ConditionalVariableCreate(&handle); };
  ~ConditionalVariable() { Os_ConditionalVariableDestroy(&handle); };

  void Wait(Mutex& mutex, uint64_t waitms) {
    Os_ConditionalVariableWait(&handle, &mutex.handle, waitms);
  }
  void Set() { Os_ConditionalVariableSet(&handle); };

  Os_ConditionalVariable_t handle;
};

struct MutexLock {
  MutexLock(Mutex& mutex) : mMutex(&mutex.handle) { Os_MutexAcquire(mMutex); };
  MutexLock(Os_Mutex_t *mutex) : mMutex(mutex) { Os_MutexAcquire(mMutex); };
  ~MutexLock() { Os_MutexRelease(mMutex); };

  /// Prevent copy construction.
  MutexLock(const MutexLock& rhs) = delete;
  /// Prevent assignment.
  MutexLock& operator=(const MutexLock& rhs) = delete;

  Os_Mutex_t *mMutex;
};

struct Thread {
  Thread(Os_JobFunction_t function, void *data) { Os_ThreadCreate(&handle, function, data); }
  ~Thread() { Os_ThreadDestroy(&handle); }

  void Join() { Os_ThreadJoin(&handle); }

  static void SetMainThread() { Os_SetMainThread(); };
  static Os_ThreadID_t GetCurrentThreadID() { return Os_GetCurrentThreadID(); };
  static bool IsMainThread() { return Os_IsMainThread(); }
  static void Sleep(uint64_t waitms) { Os_Sleep(waitms); }
  static uint32_t GetNumCPUCores(void) { return Os_CPUCoreCount(); };

  Os_Thread_t handle;
};

}; // end Os namesapce
#endif
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
