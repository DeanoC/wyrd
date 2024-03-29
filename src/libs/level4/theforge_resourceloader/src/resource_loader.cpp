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
7 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/

#include "core/core.h"
#include "core/logger.h"
#include "os/atomics.hpp"
#include "os/file.hpp"
#include "os/filesystem.hpp"
#include "os/thread.hpp"
#include "os/time.h"
#include "tinystl/vector.h"
#include "theforge/renderer.hpp"
#include "theforge_resourceloader/theforge_resourceloader.hpp"
#include "theforge_shaderreflection/theforge_shaderreflection.hpp"
#include "vfile/vfile.hpp"
#include "image/image.hpp"
#include "image/io.h"

#if defined(__ANDROID__)
#include <shaderc/shaderc.h>
#endif

namespace TheForge { namespace ResourceLoader {
namespace {
//////////////////////////////////////////////////////////////////////////
// Resource CopyEngine Structures
//////////////////////////////////////////////////////////////////////////
struct MappedMemoryRange {
  void *pData;
  Buffer *pBuffer;
  uint64_t mOffset;
  uint64_t mSize;
};

struct ResourceSet {
  Fence *pFence;
  Cmd *pCmd;
  tinystl::vector<Buffer *> mBuffers;
};

#define NUM_RESOURCE_SETS 2

//Synchronization?
struct CopyEngine {
  Queue *pQueue;
  CmdPool *pCmdPool;
  ResourceSet resourceSets[NUM_RESOURCE_SETS];
  uint64_t bufferSize;
  uint64_t allocatedSpace;
  bool isRecording;
};

//////////////////////////////////////////////////////////////////////////
// Resource Loader Internal Functions
//////////////////////////////////////////////////////////////////////////
void setupCopyEngine(Renderer *pRenderer, CopyEngine *pCopyEngine, uint32_t nodeIndex, uint64_t size) {
  QueueDesc desc = {QUEUE_FLAG_NONE, TheForge_QUEUE_PRIORITY_NORMAL, CMD_POOL_COPY, nodeIndex};
  AddQueue(pRenderer, &desc, &pCopyEngine->pQueue);

  AddCmdPool(pRenderer, pCopyEngine->pQueue, false, &pCopyEngine->pCmdPool);

  for (auto& resourceSet : pCopyEngine->resourceSets) {
    AddFence(pRenderer, &resourceSet.pFence);

    AddCmd(pCopyEngine->pCmdPool, false, &resourceSet.pCmd);

    resourceSet.mBuffers.resize(1);
    BufferDesc bufferDesc = {};
    bufferDesc.mSize = size;
    bufferDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_ONLY;
    bufferDesc.mFlags = BUFFER_CREATION_FLAG_OWN_MEMORY_BIT | BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
    bufferDesc.mNodeIndex = nodeIndex;
    TheForge::AddBuffer(pRenderer, &bufferDesc, &resourceSet.mBuffers.back());
  }

  pCopyEngine->bufferSize = size;
  pCopyEngine->allocatedSpace = 0;
  pCopyEngine->isRecording = false;
}

void cleanupCopyEngine(Renderer *pRenderer, CopyEngine *pCopyEngine) {
  for (auto& resourceSet : pCopyEngine->resourceSets) {
    for (size_t i = 0; i < resourceSet.mBuffers.size(); ++i) {
      RemoveBuffer(pRenderer, resourceSet.mBuffers[i]);
    }

    RemoveCmd(pCopyEngine->pCmdPool, resourceSet.pCmd);

    RemoveFence(pRenderer, resourceSet.pFence);
  }

  RemoveCmdPool(pRenderer, pCopyEngine->pCmdPool);

  RemoveQueue(pRenderer, pCopyEngine->pQueue);
}

void waitCopyEngineSet(Renderer *pRenderer, CopyEngine *pCopyEngine, size_t activeSet) {
  ASSERT(!pCopyEngine->isRecording);
  ResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
  WaitForFences(pRenderer, 1, &resourceSet.pFence);
}

void resetCopyEngineSet(Renderer *pRenderer, CopyEngine *pCopyEngine, size_t activeSet) {
  ASSERT(!pCopyEngine->isRecording);
  ResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
  for (size_t i = 1; i < resourceSet.mBuffers.size(); ++i) {
    RemoveBuffer(pRenderer, resourceSet.mBuffers[i]);
  }
  resourceSet.mBuffers.resize(1);
  pCopyEngine->allocatedSpace = 0;
  pCopyEngine->isRecording = false;
}

Cmd *aquireCmd(CopyEngine *pCopyEngine, size_t activeSet) {
  ResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
  if (!pCopyEngine->isRecording) {
    BeginCmd(resourceSet.pCmd);
    pCopyEngine->isRecording = true;
  }
  return resourceSet.pCmd;
}

void streamerFlush(CopyEngine *pCopyEngine, size_t activeSet) {
  if (pCopyEngine->isRecording) {
    ResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
    EndCmd(resourceSet.pCmd);
    QueueSubmit(pCopyEngine->pQueue, 1, &resourceSet.pCmd, resourceSet.pFence, 0, 0, 0, 0);
    pCopyEngine->isRecording = false;
  }
}

void finish(CopyEngine *pCopyEngine, size_t activeSet) {
  streamerFlush(pCopyEngine, activeSet);
  WaitQueueIdle(pCopyEngine->pQueue);
}

/// Return memory from pre-allocated staging buffer or create a temporary buffer if the streamer ran out of memory
MappedMemoryRange allocateStagingMemory(Renderer *pRenderer,
                                        CopyEngine *pCopyEngine,
                                        size_t activeSet,
                                        uint64_t memoryRequirement,
                                        uint32_t alignment) {
  uint64_t offset = pCopyEngine->allocatedSpace;
  if (alignment != 0) {
    offset = round_up_64(offset, alignment);
  }

  ResourceSet *pResourceSet = &pCopyEngine->resourceSets[activeSet];
  uint64_t size = pResourceSet->mBuffers.back()->mDesc.mSize;
  bool memoryAvailable = (offset < size) && (memoryRequirement <= size - offset);
  if (!memoryAvailable) {
    // Try creating a temporary staging buffer which we will clean up after resource is uploaded
    Buffer *tempStagingBuffer = NULL;
    BufferDesc desc = {};
    desc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_ONLY;
    desc.mFlags = BUFFER_CREATION_FLAG_OWN_MEMORY_BIT | BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
    desc.mSize = round_up_64(memoryRequirement, pCopyEngine->bufferSize);
    desc.mNodeIndex = pResourceSet->mBuffers.back()->mDesc.mNodeIndex;
    TheForge::AddBuffer(pRenderer, &desc, &tempStagingBuffer);

    if (tempStagingBuffer) {
      pResourceSet->mBuffers.emplace_back(tempStagingBuffer);
      offset = 0;
    } else {
      LOGERRORF("Failed to allocate memory (%llu) for resource", memoryRequirement);
      return {NULL};
    }
  }

  Buffer *buffer = pResourceSet->mBuffers.back();
#if defined(DIRECT3D11)
  // TODO: do done once, unmap before queue submit
  mapBuffer(pRenderer, buffer, nullptr);
#endif
  void *pDstData = (uint8_t *) buffer->pCpuMappedAddress + offset;
  pCopyEngine->allocatedSpace = offset + memoryRequirement;
  return {pDstData, buffer, offset, memoryRequirement};
}

ResourceStateFlags util_determine_resource_start_state(DescriptorTypeFlags usage) {
  ResourceStateFlags state = RESOURCE_STATE_UNDEFINED;
  if (usage & DESCRIPTOR_TYPE_RW_TEXTURE) {
    return RESOURCE_STATE_UNORDERED_ACCESS;
  } else if (usage & DESCRIPTOR_TYPE_TEXTURE) {
    return RESOURCE_STATE_SHADER_RESOURCE;
  }
  return state;
}

ResourceStateFlagBits util_determine_resource_start_state(const BufferDesc *pBuffer) {
  // Host visible (Upload Heap)
  if (pBuffer->mMemoryUsage == RESOURCE_MEMORY_USAGE_CPU_ONLY
      || pBuffer->mMemoryUsage == RESOURCE_MEMORY_USAGE_CPU_TO_GPU) {
    return RESOURCE_STATE_GENERIC_READ;
  }
    // Device Local (Default Heap)
  else if (pBuffer->mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY) {
    DescriptorTypeFlags usage = pBuffer->mDescriptors;

    // Try to limit number of states used overall to avoid sync complexities
    if (usage & DESCRIPTOR_TYPE_RW_BUFFER) {
      return RESOURCE_STATE_UNORDERED_ACCESS;
    }
    if ((usage & DESCRIPTOR_TYPE_VERTEX_BUFFER) || (usage & DESCRIPTOR_TYPE_UNIFORM_BUFFER)) {
      return RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    }
    if (usage & DESCRIPTOR_TYPE_INDEX_BUFFER) {
      return RESOURCE_STATE_INDEX_BUFFER;
    }
    if ((usage & DESCRIPTOR_TYPE_BUFFER)) {
      return RESOURCE_STATE_SHADER_RESOURCE;
    }

    return RESOURCE_STATE_COMMON;
  }
    // Host Cached (Readback Heap)
  else {
    return RESOURCE_STATE_COPY_DEST;
  }
}

struct UploadTextureParams {
  uint32_t w, h;
  uint32_t stridePitch;
  uint32_t numStrides;
};

#ifndef METAL
static void calculate_upload_texture_params(Enum fmt,
                                            uint32_t w,
                                            uint32_t h,
                                            UploadTextureParams& params) {
  switch (GetBlockSize(fmt)) {
    case BLOCK_SIZE_4x4: params.w = (w + 3) & 0xFFFFFFFC;
      params.stridePitch = (params.w >> 2) * GetBytesPerBlock(fmt);
      params.h = (h + 3) & 0xFFFFFFFC;
      params.numStrides = params.h >> 2;
      break;
    case BLOCK_SIZE_4x8: params.w = (w + 7) & 0xFFFFFFF8;
      params.stridePitch = (params.w >> 3) * GetBytesPerBlock(fmt);
      params.h = (h + 3) & 0xFFFFFFFC;
      params.numStrides = params.h >> 2;
      break;
    default: params.w = w;
      params.h = h;
      params.stridePitch = params.w * GetBytesPerPixel(fmt);
      params.numStrides = h;
      break;
  };
}
#endif

void updateTexture(Renderer *pRenderer,
                   CopyEngine *pCopyEngine,
                   size_t activeSet,
                   TextureUpdateDesc *pTextureUpdate) {
#if defined(METAL)
  Image::Image const *img = Image::Image::From(pTextureUpdate->pImage);
  Texture *pTexture = pTextureUpdate->pTexture;
  Cmd *pCmd = aquireCmd(pCopyEngine, activeSet);

  ASSERT(pTexture);

  uint32_t textureAlignment = pRenderer->pActiveGpuSettings->mUploadBufferTextureAlignment;
  MappedMemoryRange
      range = allocateStagingMemory(pRenderer, pCopyEngine, activeSet, pTexture->mTextureSize, textureAlignment);

  ASSERT(pTexture);

  // create source subres data structs
  SubresourceDataDesc texData[1024];
  SubresourceDataDesc *dest = texData;

  size_t const mipMapCount = img->LinkedCount();
  for (uint32_t n = 0; n < img->slices; ++n) {
    for (uint32_t i = 0; i < mipMapCount; ++i) {

      Image::Image const *mipImage = img->LinkedImageAt(i);
      dest->pData = (void *) (mipImage->Data<uint8_t>() + img->Index(0, 0, 0, n));
      dest->mRowPitch = img->ByteCountPerRow();
      dest->mSlicePitch = img->ByteCountPerSlice();
      ++dest;
    }
  }

  // calculate number of subresources
  int numSubresources = (int) (dest - texData);
  CmdUpdateSubresources(pCmd, 0, numSubresources, texData, range.pBuffer, range.mOffset, pTexture);
#else
  ASSERT(pCopyEngine->pQueue->mQueueDesc.mNodeIndex == pTextureUpdate->pTexture->mDesc.mNodeIndex);
  bool applyBarrieers =
      pRenderer->mSettings.mApi == RENDERER_API_VULKAN || pRenderer->mSettings.mApi == RENDERER_API_XBOX_D3D12;
  const Image& img = *pTextureUpdate->pImage;
  Texture *pTexture = pTextureUpdate->pTexture;
  Cmd *pCmd = aquireCmd(pCopyEngine, activeSet);

  ASSERT(pTexture);

  uint32_t textureAlignment = pRenderer->pActiveGpuSettings->mUploadBufferTextureAlignment;
  uint32_t textureRowAlignment = pRenderer->pActiveGpuSettings->mUploadBufferTextureRowAlignment;

  uint32_t nSlices = img.IsCube() ? 6 : 1;
  uint32_t arrayCount = img.GetArrayCount() * nSlices;
  uint32_t numSubresources = arrayCount * img.GetMipMapCount();
  MappedMemoryRange range = allocateStagingMemory(
      pRenderer, pCopyEngine, activeSet, pTexture->mTextureSize + textureAlignment * numSubresources, textureAlignment);
  // create source subres data structs
  SubresourceDataDesc texData[1024];
  SubresourceDataDesc *dest = texData;

  uint32_t offset = 0;
  for (uint32_t i = 0; i < img.GetMipMapCount(); ++i) {
    UploadTextureParams uploadParams;
    calculate_upload_texture_params(img.getFormat(), img.GetWidth(i), img.GetHeight(i), uploadParams);
    uint32_t bufferStridePitch = round_up(uploadParams.stridePitch, textureRowAlignment);
    uint32_t slicePitch = uploadParams.stridePitch * uploadParams.numStrides;

    for (uint32_t j = 0; j < arrayCount; ++j) {
      dest->mArrayLayer = j /*n * nSlices + k*/;
      dest->mMipLevel = i;
      dest->mBufferOffset = range.mOffset + offset;
      dest->mWidth = uploadParams.w;
      dest->mHeight = uploadParams.h;
      dest->mDepth = img.GetDepth(i);
      dest->mRowPitch = bufferStridePitch;
      dest->mSlicePitch = bufferStridePitch * uploadParams.numStrides;
      ++dest;

      uint32_t n = j / nSlices;
      uint32_t k = j - n * nSlices;
      uint8_t *pSrcData = (uint8_t *) img.GetPixels(i, n) + k * slicePitch;
      uint8_t *pSrcDataEnd = pSrcData + slicePitch * img.GetDepth(i);
      while (pSrcData < pSrcDataEnd) {
        memcpy((uint8_t *) range.pData + offset, pSrcData, uploadParams.stridePitch);
        pSrcData += uploadParams.stridePitch;
        offset += bufferStridePitch;
      }
      offset = round_up(offset, textureAlignment);
    }
  }

#if defined(DIRECT3D11)
  unmapBuffer(pRenderer, range.pBuffer);
#endif

  // Only need transition for vulkan and durango since resource will auto promote to copy dest on copy queue in PC dx12
  if (applyBarrieers) {
    TextureBarrier preCopyBarrier = {pTexture, RESOURCE_STATE_COPY_DEST};
    cmdResourceBarrier(pCmd, 0, NULL, 1, &preCopyBarrier, false);
  }

  cmdUpdateSubresources(pCmd, 0, numSubresources, texData, range.pBuffer, 0, pTexture);

  // Only need transition for vulkan and durango since resource will decay to srv on graphics queue in PC dx12
  if (applyBarrieers) {
    TextureBarrier postCopyBarrier = {pTexture, util_determine_resource_start_state(pTexture->mDesc.mDescriptors)};
    cmdResourceBarrier(pCmd, 0, NULL, 1, &postCopyBarrier, true);
  }
#endif

  if (pTextureUpdate->freeImage) {
    ((Image::Image *) img)->Destroy();
  }
}

void updateBuffer(Renderer *pRenderer,
                  CopyEngine *pCopyEngine,
                  size_t activeSet,
                  BufferUpdateDesc *pBufferUpdate) {
  ASSERT(pCopyEngine->pQueue->mQueueDesc.mNodeIndex == pBufferUpdate->pBuffer->mDesc.mNodeIndex);
  Buffer *pBuffer = pBufferUpdate->pBuffer;
  // TODO: remove uniform buffer alignment?
  const uint64_t bufferSize = (pBufferUpdate->mSize > 0) ? pBufferUpdate->mSize : pBuffer->mDesc.mSize;
  const uint64_t alignment = pBuffer->mDesc.mDescriptors & DESCRIPTOR_TYPE_UNIFORM_BUFFER
                             ? pRenderer->pActiveGpuSettings->mUniformBufferAlignment : 1;
  const uint64_t offset = round_up_64(pBufferUpdate->mDstOffset, alignment);

  void *pSrcBufferAddress = NULL;
  if (pBufferUpdate->pData) {
    //calculate address based on progress
    pSrcBufferAddress = (uint8_t *) (pBufferUpdate->pData) + pBufferUpdate->mSrcOffset;
  }

  ASSERT(pBuffer->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY
             || pBuffer->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_TO_CPU);
  Cmd *pCmd = aquireCmd(pCopyEngine, activeSet);
  //calculate remaining size
  MappedMemoryRange
      range = allocateStagingMemory(pRenderer, pCopyEngine, activeSet, bufferSize, RESOURCE_BUFFER_ALIGNMENT);
  //calculate upload size
  //calculate partial offset
  //update progress
  ASSERT(range.pData);

  if (pSrcBufferAddress) {
    memcpy(range.pData, pSrcBufferAddress, range.mSize);
  } else {
    memset(range.pData, 0, range.mSize);
  }

  CmdUpdateBuffer(pCmd, range.mOffset, pBuffer->mPositionInHeap + offset,
                  bufferSize, range.pBuffer, pBuffer);
#if defined(DIRECT3D11)
  unmapBuffer(pRenderer, range.pBuffer);
#endif

  ResourceStateFlags state = util_determine_resource_start_state(&pBuffer->mDesc);
#ifdef _DURANGO
  // XBox One needs explicit resource transitions
  BufferBarrier bufferBarriers[] = { { pBuffer, state } };
  cmdResourceBarrier(pCmd, 1, bufferBarriers, 0, NULL, false);
#else
  // Resource will automatically transition so just set the next state without a barrier
  pBuffer->mCurrentState = state;
#endif
}

//////////////////////////////////////////////////////////////////////////
// Resource Loader Globals
//////////////////////////////////////////////////////////////////////////
enum StreamerRequestType {
  STREAMER_REQUEST_UPDATE_BUFFER,
  STREAMER_REQUEST_UPDATE_TEXTURE,
  STREAMER_REQUEST_INVALID,
};

struct StreamerRequest {
  StreamerRequest() : mType(STREAMER_REQUEST_INVALID) {}
  StreamerRequest(BufferUpdateDesc& buffer) : mType(STREAMER_REQUEST_UPDATE_BUFFER), bufUpdateDesc(buffer) {}
  StreamerRequest(TextureUpdateDesc& texture) : mType(STREAMER_REQUEST_UPDATE_TEXTURE), texUpdateDesc(texture) {}

  StreamerRequestType mType;
  SyncToken mToken = 0;
  union {
    BufferUpdateDesc bufUpdateDesc;
    TextureUpdateDesc texUpdateDesc;
  };
};

//////////////////////////////////////////////////////////////////////////
// Resource Loader Implementation
//////////////////////////////////////////////////////////////////////////
struct ResourceLoader {
  Renderer *pRenderer;

  uint64_t mSize;
  CopyEngine *pCopyEngines[MAX_GPUS];

  volatile int mRun;
  Os_Thread_t mThread;

  Os::Mutex mQueueMutex;
  Os::ConditionalVariable mQueueCond;
  Os::Mutex mTokenMutex;
  Os::ConditionalVariable mTokenCond;
  tinystl::vector<StreamerRequest> mRequestQueue;

  Os_atomic64_t mTokenCompleted;
  Os_atomic64_t mTokenCounter;
};

CopyEngine *getCopyEngine(ResourceLoader *pLoader, uint32_t nodeIndex) {
  CopyEngine *& pCopyEngine = pLoader->pCopyEngines[nodeIndex];
  if (!pCopyEngine) {
    pCopyEngine = new CopyEngine();
    setupCopyEngine(pLoader->pRenderer, pCopyEngine, nodeIndex, pLoader->mSize);
  }
  return pCopyEngine;
}

void streamerThreadFunc(void *pThreadData) {
#define TIME_SLICE_DURATION_MS 4

  ResourceLoader *pLoader = (ResourceLoader *) pThreadData;
  ASSERT(pLoader);

  unsigned nextTimeslot = Os_GetSystemTime() + TIME_SLICE_DURATION_MS;
  SyncToken maxToken[NUM_RESOURCE_SETS] = {0};
  size_t activeSet = 0;
  StreamerRequest request;
  while (pLoader->mRun) {
    pLoader->mQueueMutex.Acquire();
    while (pLoader->mRun && pLoader->mRequestQueue.empty() && Os_GetSystemTime() < nextTimeslot) {
      unsigned time = Os_GetSystemTime();
      pLoader->mQueueCond.Wait(pLoader->mQueueMutex, nextTimeslot - time);
    }
    pLoader->mQueueMutex.Release();

    pLoader->mQueueMutex.Acquire();
    if (!pLoader->mRequestQueue.empty()) {
      request = pLoader->mRequestQueue.front();
      pLoader->mRequestQueue.erase(pLoader->mRequestQueue.begin());
    } else {
      request = StreamerRequest();
    }
    pLoader->mQueueMutex.Release();

    if (request.mToken) {
      ASSERT(maxToken[activeSet] < request.mToken);
      maxToken[activeSet] = request.mToken;
    }
    switch (request.mType) {
      case STREAMER_REQUEST_UPDATE_BUFFER:
        updateBuffer(
            pLoader->pRenderer, getCopyEngine(pLoader, request.bufUpdateDesc.pBuffer->mDesc.mNodeIndex), activeSet,
            &request.bufUpdateDesc);
        break;
      case STREAMER_REQUEST_UPDATE_TEXTURE:
        updateTexture(
            pLoader->pRenderer, getCopyEngine(pLoader, request.texUpdateDesc.pTexture->mDesc.mNodeIndex), activeSet,
            &request.texUpdateDesc);
        break;
      default:break;
    }

    if (Os_GetSystemTime() > nextTimeslot) {
      for (size_t i = 0; i < MAX_GPUS; ++i) {
        if (pLoader->pCopyEngines[i]) {
          streamerFlush(pLoader->pCopyEngines[i], activeSet);
        }
      }
      activeSet = (activeSet + 1) % NUM_RESOURCE_SETS;
      for (size_t i = 0; i < MAX_GPUS; ++i) {
        if (pLoader->pCopyEngines[i]) {
          waitCopyEngineSet(pLoader->pRenderer, pLoader->pCopyEngines[i], activeSet);
          resetCopyEngineSet(pLoader->pRenderer, pLoader->pCopyEngines[i], activeSet);
        }
      }
      SyncToken nextToken = maxToken[activeSet];
      SyncToken prevToken = Os_AtomicLoad64_relaxed(&pLoader->mTokenCompleted);
      // As the only writer atomicity is preserved
      Os_AtomicStore64_release(&pLoader->mTokenCompleted, nextToken > prevToken ? nextToken : prevToken);
      pLoader->mTokenCond.Set();
      nextTimeslot = Os_GetSystemTime() + TIME_SLICE_DURATION_MS;
    }

  }
  for (size_t i = 0; i < MAX_GPUS; ++i) {
    if (pLoader->pCopyEngines[i]) {
      finish(pLoader->pCopyEngines[i], activeSet);
    }
  }
}

void addResourceLoader(Renderer *pRenderer, uint64_t size, ResourceLoader **ppLoader) {
  ResourceLoader *pLoader = new ResourceLoader();
  pLoader->pRenderer = pRenderer;

  pLoader->mRun = true;
  pLoader->mSize = size;

//  pLoader->mWorkItem.pFunc = streamerThreadFunc;
//  pLoader->mWorkItem.pData = pLoader;
//  pLoader->mWorkItem.mPriority = 0;
//  pLoader->mWorkItem.mCompleted = false;

  Os_ThreadCreate(&pLoader->mThread, streamerThreadFunc, pLoader);

  *ppLoader = pLoader;
}

void removeResourceLoader(ResourceLoader *pLoader) {
  pLoader->mRun = false;
  pLoader->mQueueCond.Set();
  Os_ThreadDestroy(&pLoader->mThread);

  for (size_t i = 0; i < MAX_GPUS; ++i) {
    if (pLoader->pCopyEngines[i]) {
      cleanupCopyEngine(pLoader->pRenderer, pLoader->pCopyEngines[i]);
      delete (pLoader->pCopyEngines[i]);
    }
  }

  delete (pLoader);
}

void updateCPUbuffer(Renderer *pRenderer, BufferUpdateDesc *pBufferUpdate) {
  Buffer *pBuffer = pBufferUpdate->pBuffer;

  ASSERT(
      pBuffer->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_CPU_ONLY
          || pBuffer->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_CPU_TO_GPU);

  bool map = !pBuffer->pCpuMappedAddress;
  if (map) {
    MapBuffer(pRenderer, pBuffer, NULL);
  }

  const uint64_t bufferSize = (pBufferUpdate->mSize > 0) ? pBufferUpdate->mSize : pBuffer->mDesc.mSize;
  // TODO: remove???
  const uint64_t alignment =
      pBuffer->mDesc.mDescriptors & DESCRIPTOR_TYPE_UNIFORM_BUFFER
      ? pRenderer->pActiveGpuSettings->mUniformBufferAlignment : 1;
  const uint64_t offset = round_up_64(pBufferUpdate->mDstOffset, alignment);
  void *pDstBufferAddress = (uint8_t *) (pBuffer->pCpuMappedAddress) + offset;

  if (pBufferUpdate->pData) {
    uint8_t *pSrcBufferAddress = (uint8_t *) (pBufferUpdate->pData) + pBufferUpdate->mSrcOffset;
    memcpy(pDstBufferAddress, pSrcBufferAddress, bufferSize);
  } else {
    memset(pDstBufferAddress, 0, bufferSize);
  }

  if (map) {
    UnmapBuffer(pRenderer, pBuffer);
  }
}

void queueResourceUpdate(ResourceLoader *pLoader, BufferUpdateDesc *pBufferUpdate, SyncToken *token) {
  pLoader->mQueueMutex.Acquire();
  SyncToken t = Os_AtomicAdd64_relaxed(&pLoader->mTokenCounter, 1) + 1;
  pLoader->mRequestQueue.emplace_back(StreamerRequest(*pBufferUpdate));
  pLoader->mRequestQueue.back().mToken = t;
  pLoader->mQueueMutex.Release();
  pLoader->mQueueCond.Set();
  if (token) { *token = t; }
}

void queueResourceUpdate(ResourceLoader *pLoader, TextureUpdateDesc *pTextureUpdate, SyncToken *token) {
  pLoader->mQueueMutex.Acquire();
  SyncToken t = Os_AtomicAdd64_relaxed(&pLoader->mTokenCounter, 1) + 1;
  pLoader->mRequestQueue.emplace_back(StreamerRequest(*pTextureUpdate));
  pLoader->mRequestQueue.back().mToken = t;
  pLoader->mQueueMutex.Release();
  pLoader->mQueueCond.Set();
  if (token) { *token = t; }
}

bool isTokenCompleted(ResourceLoader *pLoader, SyncToken token) {
  bool completed = Os_AtomicLoad64_acquire(&pLoader->mTokenCompleted) >= token;
  return completed;
}

void waitTokenCompleted(ResourceLoader *pLoader, SyncToken token) {
  pLoader->mTokenMutex.Acquire();
  while (!IsTokenCompleted(pLoader, token)) {
    pLoader->mTokenCond.Wait(pLoader->mTokenMutex, -1);
  }
  pLoader->mTokenMutex.Release();
}

} // end anon namespace

//////////////////////////////////////////////////////////////////////////
// Resource Loader Implementation
//////////////////////////////////////////////////////////////////////////

void Init(Renderer *pRenderer, uint64_t memoryBudget, bool useThreads, Handle *handle) {
  addResourceLoader(pRenderer, memoryBudget, (ResourceLoader **) handle);
}

void Destroy(Handle handle) {
  removeResourceLoader((ResourceLoader *) handle);
}

void AddBuffer(Handle handle, BufferLoadDesc *pBufferDesc, bool batch) {
  SyncToken token = 0;
  AddBufferAsync(handle, pBufferDesc, &token);
  if (!batch) { WaitTokenCompleted(handle, token); }
}

void AddTexture(Handle handle, TextureLoadDesc *pTextureDesc, bool batch) {
  SyncToken token = 0;
  AddTextureAsync(handle, pTextureDesc, &token);
  if (!batch) { WaitTokenCompleted(handle, token); }
}

void UpdateBuffer(Handle handle, BufferUpdateDesc *pBufferUpdate, bool batch) {
  SyncToken token = 0;
  UpdateBufferAsync(handle, pBufferUpdate, &token);
  if (!batch) { WaitTokenCompleted(handle, token); }
}

void UpdateTexture(Handle handle, TextureUpdateDesc *pTextureUpdate, bool batch) {
  SyncToken token = 0;
  UpdateTextureAsync(handle, pTextureUpdate, &token);
  if (!batch) { WaitTokenCompleted(handle, token); }
}

void UpdateTextureAsync(Handle handle, TextureUpdateDesc *pTextureUpdate, SyncToken *token) {
  ResourceLoader *rl = (ResourceLoader *) handle;
  queueResourceUpdate(rl, pTextureUpdate, token);
}

void UpdateResources(Handle handle, uint32_t resourceCount, ResourceUpdateDesc *pResources) {
  SyncToken token = 0;
  UpdateResourcesAsync(handle, resourceCount, pResources, &token);
  WaitTokenCompleted(handle, token);
}

void AddBufferAsync(Handle handle, BufferLoadDesc *pBufferDesc, SyncToken *token) {
  ASSERT(pBufferDesc->ppBuffer);

  bool update = pBufferDesc->pData || pBufferDesc->mForceReset;

  ResourceLoader *rl = (ResourceLoader *) handle;

  pBufferDesc->mDesc.mStartState =
      update ? RESOURCE_STATE_COMMON : util_determine_resource_start_state(&pBufferDesc->mDesc);
  TheForge::AddBuffer(rl->pRenderer, &pBufferDesc->mDesc, pBufferDesc->ppBuffer);

  if (update) {
    BufferUpdateDesc bufferUpdate{
        *pBufferDesc->ppBuffer,
        pBufferDesc->pData
    };

    UpdateBuffer(rl, &bufferUpdate, token);
  }
}

void UpdateBufferAsync(Handle handle, BufferUpdateDesc *pBufferUpdate, SyncToken *token) {
  ResourceLoader *rl = (ResourceLoader *) handle;

  if (pBufferUpdate->pBuffer->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY ||
      pBufferUpdate->pBuffer->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_TO_CPU) {
    queueResourceUpdate(rl, pBufferUpdate, token);
  } else {
    updateCPUbuffer(rl->pRenderer, pBufferUpdate);
  }
}

void AddTextureAsync(Handle handle, TextureLoadDesc *pTextureDesc, SyncToken *token) {
  ASSERT(pTextureDesc->ppTexture);

  ResourceLoader *rl = (ResourceLoader *) handle;

  bool freeImage = false;
  Image::Image *pImage = nullptr;
  if (pTextureDesc->pFilename) {
    VFile::ScopedFile file = VFile::File::FromFile(pTextureDesc->pFilename, Os_FM_ReadBinary);
    pImage = Image::Image::From(Image_Load(file));
    if (!pImage) { return; }
  } else if (!pTextureDesc->pFilename && !pTextureDesc->pImage) {
    pTextureDesc->pDesc->mStartState = util_determine_resource_start_state(pTextureDesc->pDesc->mDescriptors);
    TheForge::AddTexture(rl->pRenderer, pTextureDesc->pDesc, pTextureDesc->ppTexture);
    // TODO: what about barriers???
    // Only need transition for vulkan and durango since resource will decay to srv on graphics queue in PC dx12
    //if (pLoader->pRenderer->mSettings.mApi == RENDERER_API_VULKAN || pLoader->pRenderer->mSettings.mApi == RENDERER_API_XBOX_D3D12)
    //{
    //	TextureBarrier barrier = { *pEmptyTexture->ppTexture, pEmptyTexture->pDesc->mStartState };
    //	cmdResourceBarrier(pCmd, 0, NULL, 1, &barrier, true);
    //}
    return;
  } else {
    pImage = Image::Image::From(pTextureDesc->pImage);
  }
  ASSERT(pImage);

  TextureDesc desc = {};
  desc.mFlags = pTextureDesc->mCreationFlag;
  desc.mWidth = pImage->width;
  desc.mHeight = pImage->height;
  desc.mDepth = Math_MaxU32(1U, pImage->depth);
  desc.mArraySize = pImage->slices;
  if (pImage->HasMipmaps()) {
    desc.mMipLevels = pImage->LinkedCount();
  } else {
    desc.mMipLevels = 1;
  }

  desc.mSampleCount = SAMPLE_COUNT_1;
  desc.mSampleQuality = 0;
  desc.mFormat = pImage->format;
  desc.mClearValue = ClearValue();
  desc.mDescriptors = DESCRIPTOR_TYPE_TEXTURE;
  desc.mStartState = RESOURCE_STATE_COPY_DEST;
  desc.pNativeHandle = NULL;
  desc.mHostVisible = false;
  desc.mSrgb = pTextureDesc->mSrgb;
  desc.mNodeIndex = pTextureDesc->mNodeIndex;

  if (pImage->IsCubemap()) {
    desc.mDescriptors |= DESCRIPTOR_TYPE_TEXTURE_CUBE;
  }

//TODO Deano  desc.pDebugName = pImage->GetName().c_str(); // unsafe it image gets deleted?

  TheForge::AddTexture(rl->pRenderer, &desc, pTextureDesc->ppTexture);

  TextureUpdateDesc updateDesc = {*pTextureDesc->ppTexture, pImage, freeImage};
  UpdateTexture(handle, &updateDesc, token);
}

void UpdateResourcesAsync(Handle handle, uint32_t resourceCount, ResourceUpdateDesc *pResources, SyncToken *token) {
  for (uint32_t i = 0; i < resourceCount; ++i) {
    if (pResources[i].mType == TheForge_ResourceLoader_RT_BUFFER) {
      UpdateBuffer(handle, &pResources[i].buf, token);
    } else {
      UpdateTexture(handle, &pResources[i].tex, token);
    }
  }
}

void RemoveTexture(Handle handle, Texture *pTexture) {
  ResourceLoader *rl = (ResourceLoader *) handle;
  RemoveTexture(rl->pRenderer, pTexture);
}

void RemoveBuffer(Handle handle, Buffer *pBuffer) {
  ResourceLoader *rl = (ResourceLoader *) handle;
  RemoveBuffer(rl->pRenderer, pBuffer);
}

bool IsTokenCompleted(Handle handle, SyncToken token) {
  ResourceLoader *rl = (ResourceLoader *) handle;
  return isTokenCompleted(rl, token);
}

void WaitTokenCompleted(Handle handle, SyncToken token) {
  ResourceLoader *rl = (ResourceLoader *) handle;
  waitTokenCompleted(rl, token);
}

void WaitBatchCompleted(Handle handle) {
  ResourceLoader *rl = (ResourceLoader *) handle;
  SyncToken token = Os_AtomicLoad64_relaxed(&rl->mTokenCounter);
  waitTokenCompleted(rl, token);
}

void FlushResourceUpdates(Handle handle) {
  WaitBatchCompleted(handle);
}

void FinishResourceLoading(Handle handle) {
  WaitBatchCompleted(handle);
}
bool find_shader_stage(
    const tinystl::string& fileName,
    BinaryShaderDesc *pBinaryDesc,
    BinaryShaderStageDesc **pOutStage,
    ShaderStage *pStage) {
  tinystl::string ext = Os::FileSystem::GetExtension(fileName);
  if (ext == ".vert") {
    *pOutStage = &pBinaryDesc->mVert;
    *pStage = SHADER_STAGE_VERT;
  } else if (ext == ".frag") {
    *pOutStage = &pBinaryDesc->mFrag;
    *pStage = SHADER_STAGE_FRAG;
  }
    else if (ext == ".tesc") {
      *pOutStage = &pBinaryDesc->mHull;
      *pStage = SHADER_STAGE_HULL;
    } else if (ext == ".tese") {
      *pOutStage = &pBinaryDesc->mDomain;
      *pStage = SHADER_STAGE_DOMN;
    } else if (ext == ".geom") {
      *pOutStage = &pBinaryDesc->mGeom;
      *pStage = SHADER_STAGE_GEOM;
    }
  else if (ext == ".comp") {
    *pOutStage = &pBinaryDesc->mComp;
    *pStage = SHADER_STAGE_COMP;
  } else if ((ext == ".rgen") ||
      (ext == ".rmiss") ||
      (ext == ".rchit") ||
      (ext == ".rint") ||
      (ext == ".rahit") ||
      (ext == ".rcall")) {
    *pOutStage = &pBinaryDesc->mComp;
    *pStage = SHADER_STAGE_LIB;
  } else {
    return false;
  }
  return true;
}

// Loads the bytecode from file if the binary shader file is newer than the source
bool check_for_byte_code(const tinystl::string& binaryShaderName,
                         uint32_t sourceTimeStamp,
                         tinystl::vector<char>& byteCode) {
  using namespace VFile;
  if (!Os::FileSystem::FileExists(binaryShaderName)) {
    return false;
  }

  // If source code is loaded from a package, its timestamp will be zero. Else check that binary is not older
  // than source
  if (sourceTimeStamp && Os::FileSystem::GetLastModifiedTime(binaryShaderName) < sourceTimeStamp) {
    return false;
  }

  ScopedFile file(File::FromFile(binaryShaderName, Os_FM_ReadBinary));
  if (!file) {
    LOGERRORF("%s is not a valid shader bytecode file", binaryShaderName.c_str());
    return false;
  }

  byteCode.resize(file->Size());
  memcpy(byteCode.data(), file->ReadString().c_str(), byteCode.size());

  return true;
}

// Saves bytecode to a file
bool save_byte_code(const tinystl::string& binaryShaderName, const tinystl::vector<char>& byteCode) {
  using namespace VFile;

  tinystl::string path = Os::FileSystem::GetParentPath(binaryShaderName);
  if (!Os::FileSystem::DirExists(path)) {
    Os::FileSystem::CreateDir(path);
  }

  ScopedFile outFile(File::FromFile(binaryShaderName, Os_FM_WriteBinary));
  if (!outFile) {
    return false;
  }

  outFile->Write(byteCode.data(), (uint32_t) byteCode.size() * sizeof(char));

  return true;
}
// Function to generate the timestamp of this shader source file considering all include file timestamp
bool GenerateShaderTimestamp(VFile::File *file,
                             uint64_t& outTimeStamp) {
  using namespace VFile;

  // If the source if a non-packaged file, store the timestamp
  if (file) {
    tinystl::string fullName = file->GetName();
    uint64_t fileTimeStamp = Os::FileSystem::GetLastModifiedTime(fullName);
    if (fileTimeStamp > outTimeStamp) {
      outTimeStamp = fileTimeStamp;
    }
  }

  const tinystl::string pIncludeDirective = "#include";
  while (!file->IsEOF()) {
    tinystl::string line = file->ReadLine();
    tinystl::string::size_type filePos = line.find(pIncludeDirective, 0);
    const tinystl::string::size_type commentPosCpp = line.find("//", 0);
    const tinystl::string::size_type commentPosC = line.find("/*", 0);

    // if we have an "#include \"" in our current line
    const bool bLineHasIncludeDirective = filePos != tinystl::string::npos;
    const bool bLineIsCommentedOut = (commentPosCpp != tinystl::string::npos && commentPosCpp < filePos) ||
        (commentPosC != tinystl::string::npos && commentPosC < filePos);

    if (bLineHasIncludeDirective && !bLineIsCommentedOut) {
      // get the include file name
      uint32_t currentPos = filePos + (uint32_t) pIncludeDirective.size();
      tinystl::string fileName;
      while (line.at(currentPos++) == ' ') {}    // skip empty spaces
      if (currentPos >= (uint32_t) line.size()) {
        continue;
      }
      if (line.at(currentPos - 1) != '\"') {
        continue;
      } else {
        // read char by char until we have the include file name
        while (line.at(currentPos) != '\"') {
          fileName.push_back(line.at(currentPos));
          ++currentPos;
        }
      }

      tinystl::string includeFileName;
      tinystl::string includeExtension;
      Os::FileSystem::SplitPath(file->GetName(), includeFileName, includeExtension);
      // get the include file path
      if (includeFileName.at(0) == '<') {    // disregard bracketsauthop
        continue;
      }

      // open the include file
      ScopedFile includeFile(File::FromFile(includeFileName, Os_FM_ReadBinary));
      if (!includeFile) {
        LOGERRORF("Cannot open #include file: %s", includeFileName.c_str());
        return false;
      }

      // Add the include file into the current code recursively
      if (!GenerateShaderTimestamp(includeFile.owned, outTimeStamp)) {
        return false;
      }
    }
  }
  return true;
}

bool load_shader_stage_byte_code(
    Renderer *pRenderer,
    ShaderTarget target,
    ShaderStage stage,
    const char *fileName,
    uint32_t macroCount,
    ShaderReflection::Macro *pMacros,
    uint32_t rendererMacroCount,
    ShaderReflection::Macro *pRendererMacros,
    tinystl::vector<char>& byteCode,
    const char *pEntryPoint) {
  using namespace VFile;

  tinystl::string code;
  uint64_t timeStamp = 0;

#ifndef METAL
  const char *shaderName = fileName;
#else
  // Metal shader files need to have the .metal extension.
  tinystl::string metalShaderName = tinystl::string(fileName) + ".metal";
  const char *shaderName = metalShaderName.c_str();
#endif

  ScopedFile shaderSource(File::FromFile(shaderName, Os_FM_ReadBinary));
  ASSERT(shaderSource);

  if (!GenerateShaderTimestamp(shaderSource.owned, timeStamp)) {
    return false;
  }

  tinystl::string name, extension;
  Os::FileSystem::SplitPath(fileName, name, extension);
  tinystl::string shaderDefines;
  // Apply user specified macros
  for (uint32_t i = 0; i < macroCount; ++i) {
    shaderDefines += tinystl::string::format("%s %i\n", pMacros[i].definition, pMacros[i].value);
  }
  // Apply renderer specified macros
  for (uint32_t i = 0; i < rendererMacroCount; ++i) {
    shaderDefines += tinystl::string::format("%s %i\n", pRendererMacros[i].definition, pRendererMacros[i].value);
  }

  tinystl::string rendererApi;
  switch (pRenderer->mSettings.mApi) {
    case RENDERER_API_D3D12:
    case RENDERER_API_XBOX_D3D12: rendererApi = "D3D12";
      break;
    case RENDERER_API_D3D11: rendererApi = "D3D11";
      break;
    case RENDERER_API_VULKAN: rendererApi = "Vulkan";
      break;
    case RENDERER_API_METAL: rendererApi = "Metal";
      break;
    default: break;
  }

  tinystl::string appName(pRenderer->pName);
#ifdef __linux__
  tinystl::string lowerStr = appName.to_lower();
  appName = lowerStr != pRenderer->pName ? lowerStr : lowerStr + "_";
#endif

  tinystl::string
      binaryShaderName = Os::FileSystem::GetExePath() + "/" + appName + tinystl::string("/Shaders/") + rendererApi +
      tinystl::string("/CompiledShadersBinary/") + Os::FileSystem::GetFileName(fileName) +
      tinystl::string::format("_%zu", tinystl::hash(shaderDefines)) + extension +
      tinystl::string::format("%u", (uint32_t) target) + ".bin";

  // Shader source is newer than binary
  if (!check_for_byte_code(binaryShaderName, timeStamp, byteCode)) {

    char *pByteCode = NULL;
    uint32_t byteCodeSize = 0;

    TheForge::ShaderReflection::CompileShader(
        pRenderer,
        shaderSource->GetName(),
        macroCount,
        pMacros,
        &pByteCode,
        pEntryPoint);

    byteCode.resize(byteCodeSize);
    memcpy(byteCode.data(), pByteCode, byteCodeSize);
    free(pByteCode);
    if (!save_byte_code(binaryShaderName, byteCode)) {
      LOGWARNINGF("Failed to save byte code for file %s", shaderSource->GetName());
    }

#if defined(VULKAN)
#if defined(__ANDROID__)
    vk_compileShader(pRenderer, stage, (uint32_t)code.size(), code.c_str(), binaryShaderName, macroCount, pMacros, &byteCode, pEntryPoint);
#else
    vk_compileShader(pRenderer, target, shaderSource.GetName(), binaryShaderName, macroCount, pMacros, &byteCode, pEntryPoint);
#endif
#elif defined(DIRECT3D12) || defined(DIRECT3D11)
    compileShader(
        pRenderer, target, stage, shaderSource.GetName(), (uint32_t)code.size(), code.c_str(), macroCount, pMacros, conf_malloc,
        &byteCodeSize, &pByteCode, pEntryPoint);
#endif
  }

  if (!byteCode.size()) {
    LOGERRORF("Error while generating bytecode for shader %s", fileName);
    return false;
  }

  return true;
}

void AddShader(Handle handle,
               const ShaderLoadDesc *pDesc,
               Shader **ppShader) {
  ResourceLoader *rl = (ResourceLoader *) handle;

  BinaryShaderDesc binaryDesc = {};
  tinystl::vector<char> byteCodes[SHADER_STAGE_COUNT] = {};
  for (uint32_t i = 0; i < SHADER_STAGE_COUNT; ++i) {
    ShaderReflection::RendererDefinesDesc const* rendererDefinesDesc = ShaderReflection::GetRendererShaderDefines(rl->pRenderer);

    if (pDesc->mStages[i].mFileName != nullptr) {
      tinystl::string filename = pDesc->mStages[i].mFileName;

      ShaderStage stage;
      BinaryShaderStageDesc *pStage = NULL;
      if (find_shader_stage(filename, &binaryDesc, &pStage, &stage)) {
        if (!load_shader_stage_byte_code(
            rl->pRenderer,
            pDesc->mTarget,
            stage,
            filename.c_str(),
            pDesc->mStages[i].mMacroCount,
            pDesc->mStages[i].pMacros,
            rendererDefinesDesc->rendererShaderDefinesCnt,
            rendererDefinesDesc->rendererShaderDefines,
            byteCodes[i],
            pDesc->mStages[i].mEntryPointName)) {
          return;
        }

        binaryDesc.mStages = (ShaderStage) (binaryDesc.mStages | stage);
        pStage->pByteCode = byteCodes[i].data();
        pStage->mByteCodeSize = (uint32_t) byteCodes[i].size();

#if defined(METAL)
        if (pDesc->mStages[i].mEntryPointName) {
          pStage->mEntryPoint = pDesc->mStages[i].mEntryPointName;
        } else {
          pStage->mEntryPoint = "stageMain";
        }
        // In metal, we need the shader source for our reflection system.
        VFile::ScopedFile metalFile(VFile::File::FromFile(filename + ".metal", Os_FM_ReadBinary));
        tinystl::string shaderSource = metalFile->ReadString();
        pStage->mSource = (char *) malloc(shaderSource.size() + 1); // leak here!
        strcpy((char *) pStage->mSource, shaderSource.c_str());

#else
        if (pDesc->mStages[i].mEntryPointName) {
          pStage->mEntryPoint = pDesc->mStages[i].mEntryPointName;
        } else {
          pStage->mEntryPoint = "main";
        }
#endif
      }
    }
  }

  AddShaderBinary(rl->pRenderer, &binaryDesc, ppShader);
}

}} // end namespace