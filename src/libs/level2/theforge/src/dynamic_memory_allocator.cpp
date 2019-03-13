#include "core/core.h"
#include "dynamic_memory_allocator.hpp"
#include "theforge/renderer.hpp"
#include "os/thread.hpp"

namespace TheForge {


void AddDynamicMemoryAllocator(Renderer *pRenderer, uint64_t size, DynamicMemoryAllocator **ppAllocator) {
  ASSERT(pRenderer);

  DynamicMemoryAllocator *pAllocator = (DynamicMemoryAllocator *) calloc(1, sizeof(*pAllocator));
  pAllocator->mCurrentPos = 0;
  pAllocator->mSize = size;
  Os_MutexCreate(&pAllocator->mAllocationMutex);

  BufferDesc desc = {};
  desc.mDescriptors = DESCRIPTOR_TYPE_INDEX_BUFFER | DESCRIPTOR_TYPE_VERTEX_BUFFER | DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  desc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
  desc.mSize = pAllocator->mSize;
  desc.mFlags = BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
  AllocBuffer(pRenderer, &desc, &pAllocator->pBuffer);

  pAllocator->mAlignment = pRenderer->pActiveGpuSettings->mUniformBufferAlignment;

  *ppAllocator = pAllocator;
}

void RemoveDynamicMemoryAllocator(Renderer *pRenderer, DynamicMemoryAllocator *pAllocator) {
  ASSERT(pAllocator);

  FreeBuffer(pRenderer, pAllocator->pBuffer);

  Os_MutexDestroy(&pAllocator->mAllocationMutex);

  free(pAllocator);
}

void ResetDynamicMemoryAllocator(DynamicMemoryAllocator *pAllocator) {
  ASSERT(pAllocator);
  pAllocator->mCurrentPos = 0;
}

void ConsumeDynamicMemoryAllocator(
    DynamicMemoryAllocator *p_linear_allocator,
    uint64_t size,
    void **ppCpuAddress,
    uint64_t *pOffset) {

  Os::MutexLock lock(&p_linear_allocator->mAllocationMutex);

  if (p_linear_allocator->mCurrentPos + size > p_linear_allocator->mSize) {
    ResetDynamicMemoryAllocator(p_linear_allocator);
  }

  *ppCpuAddress = (uint8_t *) p_linear_allocator->pBuffer->pCpuMappedAddress + p_linear_allocator->mCurrentPos;
  *pOffset = p_linear_allocator->mCurrentPos;

  // Increment position by multiple of 256 to use CBVs in same heap as other buffers
  p_linear_allocator->mCurrentPos += round_up_64(size, p_linear_allocator->mAlignment);
}

void ConsumeDynamicMemoryAllocatorLockFree(
    DynamicMemoryAllocator *p_linear_allocator,
    uint64_t size,
    void **ppCpuAddress,
    uint64_t *pOffset) {
  if (p_linear_allocator->mCurrentPos + size > p_linear_allocator->mSize) {
    ResetDynamicMemoryAllocator(p_linear_allocator);
  }

  *ppCpuAddress = (uint8_t *) p_linear_allocator->pBuffer->pCpuMappedAddress + p_linear_allocator->mCurrentPos;
  *pOffset = p_linear_allocator->mCurrentPos;
  // Increment position by multiple of 256 to use CBVs in same heap as other buffers
  p_linear_allocator->mCurrentPos += round_up_64(size, p_linear_allocator->mAlignment);
}


}

