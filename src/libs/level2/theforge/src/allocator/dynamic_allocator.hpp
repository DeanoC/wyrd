#pragma once
#ifndef WYRD_THEFORGE_DYNAMIC_ALLOCATOR_HPP
#define WYRD_THEFORGE_DYNAMIC_ALLOCATOR_HPP

#include "core/core.h"
#include "os/thread.h"
#include "theforge/renderer.hpp"
#include "config.hpp"

namespace TheForge {

// Minimum size of a free suballocation to register it in the free suballocation collection.
static const uint64_t RESOURCE_MIN_FREE_SUBALLOCATION_SIZE_TO_REGISTER = 16;

struct DynamicMemoryAllocator {
  /// Size of mapped resources to be created
  uint64_t mSize;
  /// Current offset in the used page
  uint64_t mCurrentPos;
  /// Buffer alignment
  uint64_t mAlignment;
  Buffer *pBuffer;

  Renderer *pRenderer;
  Os_Mutex_t *pMutex;

  void Reset() { mCurrentPos = 0; };
  void Consume(uint64_t size, void **ppCpuAddress, uint64_t *pOffset);
  void ConsumeLockFree(uint64_t size, void **ppCpuAddress, uint64_t *pOffset);

  static DynamicMemoryAllocator *Create(Renderer *renderer, uint64_t size);
  static void Destroy(DynamicMemoryAllocator *allocator);
};
}

#endif //WYRD_THEFORGE_DYNAMIC_ALLOCATOR_HPP
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
