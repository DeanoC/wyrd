
#include "core/core.h"
#include "os/thread.hpp"
//#include "../Interfaces/IMemoryManager.h"

#include <unistd.h>

void *ThreadFunctionStatic(void *data) {
  WorkItem *pItem = static_cast<WorkItem *>(data);
//  pItem->pFunc(pItem->pData);
  return 0;
}

Os_ThreadID_t Thread::mainThreadID;

void Thread::SetMainThread() {
  mainThreadID = GetCurrentThreadID();
}

Os_ThreadID_t Thread::GetCurrentThreadID() {
  return (Os_ThreadID_t) pthread_self();
}

bool Thread::IsMainThread() {
  return GetCurrentThreadID() == mainThreadID;
}
void Thread::Sleep(unsigned mSec) {
  usleep(mSec * 1000);
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
