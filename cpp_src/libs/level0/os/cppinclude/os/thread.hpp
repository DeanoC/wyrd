// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef WYRD_OS_THREAD_HPP
#define WYRD_OS_THREAD_HPP

#include "core/core.h"
#include "os/thread.h"
#include "tinystl/vector.h"

struct Mutex {
  Mutex() { Os_MutexCreate(&handle); };
  ~Mutex() { Os_MutexDestroy(&handle); };

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
  MutexLock(Mutex& mutex);
  ~MutexLock();

  /// Prevent copy construction.
  MutexLock(const MutexLock& rhs) = delete;
  /// Prevent assignment.
  MutexLock& operator=(const MutexLock& rhs) = delete;

  Mutex& mMutex;
};

/// Work queue item.
struct WorkItem : public WorkItem_t {
  // Construct
  WorkItem() : WorkItem_t{nullptr, 0, false} {}
};

#ifndef _WIN32
/// Forward declaration
struct Thread;
#endif

/// Work queue subsystem for multithreading.
class ThreadPool {
 public:
  /// Construct.
  ThreadPool();
  /// Destruct.
  ~ThreadPool();

  /// Can only be called once during lifetime of program
  void CreateThreads(unsigned numThreads);
  void AddWorkItem(WorkItem *item);
  bool RemoveWorkItem(WorkItem *& item);
  unsigned RemoveWorkItems(const tinystl::vector<WorkItem *>& items);
  void Pause();
  void Resume();

  void Shutdown() { mShutDown = true; }

  void Complete(unsigned priority);

  unsigned GetNumThreads() const { return (uint32_t) mThreads.size(); }

  bool IsCompleted(unsigned priority) const;

  bool IsCompleting() const { return mCompleting; }

  static void ProcessItems(void *pThreadSystem);

 private:
  void Cleanup(unsigned priority);

  tinystl::vector<struct Thread *> mThreads;
  tinystl::vector<WorkItem *> mWorkItems;
  tinystl::vector<WorkItem *> mWorkQueue;
  Mutex mQueueMutex;
  ConditionalVariable mWaitConditionVar;
  Mutex mWaitMutex;
  volatile bool mShutDown;
  volatile bool mPausing;
  bool mPaused;
  bool mCompleting;
};

struct Thread {
  Thread(ThreadPool *threadSystem);
  ~Thread();

/*	Os_ThreadHandle_t pHandle;
	WorkItem* pItem;
*/
  static Os_ThreadID_t mainThreadID;

  static void SetMainThread();
  static Os_ThreadID_t GetCurrentThreadID();
  static bool IsMainThread();
  static void Sleep(unsigned mSec);
  static unsigned int GetNumCPUCores(void);
};

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
