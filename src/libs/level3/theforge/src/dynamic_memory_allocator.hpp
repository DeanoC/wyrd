
#pragma once
#ifndef WYRD_THEFORGE_DYNAMICMEMORYALLOCATOR_HPP
#define WYRD_THEFORGE_DYNAMICMEMORYALLOCATOR_HPP

#include "core/core.h"
#include "os/thread.h"
#include "theforge/renderer.hpp"

namespace TheForge {
struct DynamicMemoryAllocator {
  /// Size of mapped resources to be created
  uint64_t mSize;
  /// Current offset in the used page
  uint64_t mCurrentPos;
  /// Buffer alignment
  uint64_t mAlignment;
  Buffer *pBuffer;

  Os_Mutex_t mAllocationMutex;
};

void AddDynamicMemoryAllocator(Renderer *pRenderer, uint64_t size, DynamicMemoryAllocator **ppAllocator);
void RemoveDynamicMemoryAllocator(Renderer *pRenderer, DynamicMemoryAllocator *pAllocator);
void ConsumeDynamicMemoryAllocator(DynamicMemoryAllocator *p_linear_allocator,uint64_t size,void **ppCpuAddress,uint64_t *pOffset);
void ConsumeDynamicMemoryAllocatorLockFree(DynamicMemoryAllocator *p_linear_allocator,uint64_t size,void **ppCpuAddress,uint64_t *pOffset);

}
#endif //WYRD_THEFORGE_DYNAMICMEMORYALLOCATOR_HPP
