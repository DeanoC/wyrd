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
#include "vfile/vfile.hpp"
#include "theforge/renderer.hpp"
#include "tinystl/vector.h"

#include "resource_loader.h"

// TODO
extern unsigned getSystemTime();


//this is needed for unix as PATH_MAX is defined instead of MAX_PATH
#ifndef _WIN32
//linux needs limits.h for PATH_MAX
#ifdef __linux__
#include <limits.h>
#endif
#if defined(__ANDROID__)
#include <shaderc/shaderc.h>
#endif
#define MAX_PATH PATH_MAX
#endif

namespace TheForge {
// buffer functions
extern void AddBuffer(Renderer *pRenderer, const BufferDesc *desc, Buffer **pp_buffer);
extern void RemoveBuffer(Renderer *pRenderer, Buffer *p_buffer);
extern void MapBuffer(Renderer *pRenderer, Buffer *pBuffer, ReadRange *pRange);
extern void UnmapBuffer(Renderer *pRenderer, Buffer *pBuffer);
extern void AddTexture(Renderer *pRenderer, const TextureDesc *pDesc, Texture **pp_texture);
extern void RemoveTexture(Renderer *pRenderer, Texture *p_texture);
extern void CmdUpdateBuffer(Cmd *p_cmd,
                            uint64_t srcOffset,
                            uint64_t dstOffset,
                            uint64_t size,
                            Buffer *p_src_buffer,
                            Buffer *p_buffer);
extern void CmdUpdateSubresources(
    Cmd *pCmd,
    uint32_t startSubresource,
    uint32_t numSubresources,
    SubresourceDataDesc *pSubresources,
    Buffer *pIntermediate,
    uint64_t intermediateOffset,
    Texture *pTexture);
extern const RendererShaderDefinesDesc get_renderer_shaderdefines(Renderer *pRenderer);

/************************************************************************/
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Resource CopyEngine Structures
//////////////////////////////////////////////////////////////////////////
typedef struct MappedMemoryRange {
  void *pData;
  Buffer *pBuffer;
  uint64_t mOffset;
  uint64_t mSize;
} MappedMemoryRange;

typedef struct ResourceSet {
  Fence *pFence;
  Cmd *pCmd;
  tinystl::vector<Buffer *> mBuffers;
} CopyResourceSet;

#define NUM_RESOURCE_SETS 2

//Synchronization?
typedef struct CopyEngine {
  Queue *pQueue;
  CmdPool *pCmdPool;
  ResourceSet resourceSets[NUM_RESOURCE_SETS];
  uint64_t bufferSize;
  uint64_t allocatedSpace;
  bool isRecording;
} CopyEngine;

//////////////////////////////////////////////////////////////////////////
// Resource Loader Internal Functions
//////////////////////////////////////////////////////////////////////////
static void setupCopyEngine(Renderer *pRenderer, CopyEngine *pCopyEngine, uint32_t nodeIndex, uint64_t size) {
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
    AddBuffer(pRenderer, &bufferDesc, &resourceSet.mBuffers.back());
  }

  pCopyEngine->bufferSize = size;
  pCopyEngine->allocatedSpace = 0;
  pCopyEngine->isRecording = false;
}

static void cleanupCopyEngine(Renderer *pRenderer, CopyEngine *pCopyEngine) {
  for (auto& resourceSet : pCopyEngine->resourceSets) {
    for (size_t i = 0; i < resourceSet.mBuffers.size(); ++i) {
      RemoveBuffer(pRenderer, resourceSet.mBuffers[i]);
    }

    RemoveCmd(pCopyEngine->pCmdPool, resourceSet.pCmd);

    RemoveFence(pRenderer, resourceSet.pFence);
  }

  RemoveCmdPool(pRenderer, pCopyEngine->pCmdPool);

  RemoveQueue(pCopyEngine->pQueue);
}

static void waitCopyEngineSet(Renderer *pRenderer, CopyEngine *pCopyEngine, size_t activeSet) {
  ASSERT(!pCopyEngine->isRecording);
  ResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
  WaitForFences(pRenderer, 1, &resourceSet.pFence);
}

static void resetCopyEngineSet(Renderer *pRenderer, CopyEngine *pCopyEngine, size_t activeSet) {
  ASSERT(!pCopyEngine->isRecording);
  ResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
  for (size_t i = 1; i < resourceSet.mBuffers.size(); ++i) {
    RemoveBuffer(pRenderer, resourceSet.mBuffers[i]);
  }
  resourceSet.mBuffers.resize(1);
  pCopyEngine->allocatedSpace = 0;
  pCopyEngine->isRecording = false;
}

static Cmd *aquireCmd(CopyEngine *pCopyEngine, size_t activeSet) {
  ResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
  if (!pCopyEngine->isRecording) {
    BeginCmd(resourceSet.pCmd);
    pCopyEngine->isRecording = true;
  }
  return resourceSet.pCmd;
}

static void streamerFlush(CopyEngine *pCopyEngine, size_t activeSet) {
  if (pCopyEngine->isRecording) {
    ResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
    EndCmd(resourceSet.pCmd);
    QueueSubmit(pCopyEngine->pQueue, 1, &resourceSet.pCmd, resourceSet.pFence, 0, 0, 0, 0);
    pCopyEngine->isRecording = false;
  }
}

static void finish(CopyEngine *pCopyEngine, size_t activeSet) {
  streamerFlush(pCopyEngine, activeSet);
  WaitQueueIdle(pCopyEngine->pQueue);
}

/// Return memory from pre-allocated staging buffer or create a temporary buffer if the streamer ran out of memory
static MappedMemoryRange allocateStagingMemory(Renderer *pRenderer,
                                               CopyEngine *pCopyEngine,
                                               size_t activeSet,
                                               uint64_t memoryRequirement,
                                               uint32_t alignment) {
  uint64_t offset = pCopyEngine->allocatedSpace;
  if (alignment != 0) {
    offset = round_up_64(offset, alignment);
  }

  CopyResourceSet *pResourceSet = &pCopyEngine->resourceSets[activeSet];
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
    AddBuffer(pRenderer, &desc, &tempStagingBuffer);

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

static ResourceStateFlags util_determine_resource_start_state(DescriptorTypeFlags usage) {
  ResourceStateFlags state = RESOURCE_STATE_UNDEFINED;
  if (usage & DESCRIPTOR_TYPE_RW_TEXTURE) {
    return RESOURCE_STATE_UNORDERED_ACCESS;
  } else if (usage & DESCRIPTOR_TYPE_TEXTURE) {
    return RESOURCE_STATE_SHADER_RESOURCE;
  }
  return state;
}

static ResourceStateFlagBits util_determine_resource_start_state(const BufferDesc *pBuffer) {
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

static void updateTexture(Renderer *pRenderer,
                          CopyEngine *pCopyEngine,
                          size_t activeSet,
                          TextureUpdateDesc *pTextureUpdate) {
#if defined(METAL)
  const Image& img = *pTextureUpdate->pImage;
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
  uint32_t nSlices = img.IsCube() ? 6 : 1;

  for (uint32_t n = 0; n < img.GetArrayCount(); ++n) {
    for (uint32_t k = 0; k < nSlices; ++k) {
      for (uint32_t i = 0; i < img.GetMipMapCount(); ++i) {
        uint32_t pitch, slicePitch;
        if (IsCompressedFormat(img.getFormat())) {
          pitch = ((img.GetWidth(i) + 3) >> 2) * GetBytesPerBlock(img.getFormat());
          slicePitch = pitch * ((img.GetHeight(i) + 3) >> 2);
        } else {
          pitch = img.GetWidth(i) * GetBytesPerPixel(img.getFormat());
          slicePitch = pitch * img.GetHeight(i);
        }

        dest->pData = img.GetPixels(i, n) + k * slicePitch;
        dest->mRowPitch = pitch;
        dest->mSlicePitch = slicePitch;
        ++dest;
      }
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
    pTextureUpdate->pImage->Destroy();
    delete(pTextureUpdate->pImage);
  }
}

static void updateBuffer(Renderer *pRenderer,
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
typedef enum StreamerRequestType {
  STREAMER_REQUEST_UPDATE_BUFFER,
  STREAMER_REQUEST_UPDATE_TEXTURE,
  STREAMER_REQUEST_INVALID,
} StreamerRequestType;

typedef struct StreamerRequest {
  StreamerRequest() : mType(STREAMER_REQUEST_INVALID) {}
  StreamerRequest(BufferUpdateDesc& buffer) : mType(STREAMER_REQUEST_UPDATE_BUFFER), bufUpdateDesc(buffer) {}
  StreamerRequest(TextureUpdateDesc& texture) : mType(STREAMER_REQUEST_UPDATE_TEXTURE), texUpdateDesc(texture) {}

  StreamerRequestType mType;
  SyncToken mToken = 0;
  union {
    BufferUpdateDesc bufUpdateDesc;
    TextureUpdateDesc texUpdateDesc;
  };
} StreamerRequest;

//////////////////////////////////////////////////////////////////////////
// Resource Loader Implementation
//////////////////////////////////////////////////////////////////////////
typedef struct ResourceLoader {
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
} ResourceLoader;

static CopyEngine *getCopyEngine(ResourceLoader *pLoader, uint32_t nodeIndex) {
  CopyEngine *& pCopyEngine = pLoader->pCopyEngines[nodeIndex];
  if (!pCopyEngine) {
    pCopyEngine = new CopyEngine();
    setupCopyEngine(pLoader->pRenderer, pCopyEngine, nodeIndex, pLoader->mSize);
  }
  return pCopyEngine;
}

static void streamerThreadFunc(void *pThreadData) {
#define TIME_SLICE_DURATION_MS 4

  ResourceLoader *pLoader = (ResourceLoader *) pThreadData;
  ASSERT(pLoader);

  unsigned nextTimeslot = getSystemTime() + TIME_SLICE_DURATION_MS;
  SyncToken maxToken[NUM_RESOURCE_SETS] = {0};
  size_t activeSet = 0;
  StreamerRequest request;
  while (pLoader->mRun) {
    pLoader->mQueueMutex.Acquire();
    while (pLoader->mRun && pLoader->mRequestQueue.empty() && getSystemTime() < nextTimeslot) {
      unsigned time = getSystemTime();
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

    if (getSystemTime() > nextTimeslot) {
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
      nextTimeslot = getSystemTime() + TIME_SLICE_DURATION_MS;
    }

  }
  for (size_t i = 0; i < MAX_GPUS; ++i) {
    if (pLoader->pCopyEngines[i]) {
      finish(pLoader->pCopyEngines[i], activeSet);
    }
  }
}

static void addResourceLoader(Renderer *pRenderer, uint64_t size, ResourceLoader **ppLoader) {
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

static void removeResourceLoader(ResourceLoader *pLoader) {
  pLoader->mRun = false;
  pLoader->mQueueCond.Set();
  Os_ThreadDestroy(&pLoader->mThread);

  for (size_t i = 0; i < MAX_GPUS; ++i) {
    if (pLoader->pCopyEngines[i]) {
      cleanupCopyEngine(pLoader->pRenderer, pLoader->pCopyEngines[i]);
      delete(pLoader->pCopyEngines[i]);
    }
  }

  delete(pLoader);
}

static void updateCPUbuffer(Renderer *pRenderer, BufferUpdateDesc *pBufferUpdate) {
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
    uint8_t * pSrcBufferAddress = (uint8_t *) (pBufferUpdate->pData) + pBufferUpdate->mSrcOffset;
    memcpy(pDstBufferAddress, pSrcBufferAddress, bufferSize);
  } else {
    memset(pDstBufferAddress, 0, bufferSize);
  }

  if (map) {
    UnmapBuffer(pRenderer, pBuffer);
  }
}

static void queueResourceUpdate(ResourceLoader *pLoader, BufferUpdateDesc *pBufferUpdate, SyncToken *token) {
  pLoader->mQueueMutex.Acquire();
  SyncToken t = Os_AtomicAdd64_relaxed(&pLoader->mTokenCounter, 1) + 1;
  pLoader->mRequestQueue.emplace_back(StreamerRequest(*pBufferUpdate));
  pLoader->mRequestQueue.back().mToken = t;
  pLoader->mQueueMutex.Release();
  pLoader->mQueueCond.Set();
  if (token) { *token = t; }
}

static void queueResourceUpdate(ResourceLoader *pLoader, TextureUpdateDesc *pTextureUpdate, SyncToken *token) {
  pLoader->mQueueMutex.Acquire();
  SyncToken t = Os_AtomicAdd64_relaxed(&pLoader->mTokenCounter, 1) + 1;
  pLoader->mRequestQueue.emplace_back(StreamerRequest(*pTextureUpdate));
  pLoader->mRequestQueue.back().mToken = t;
  pLoader->mQueueMutex.Release();
  pLoader->mQueueCond.Set();
  if (token) { *token = t; }
}

static bool IsTokenCompleted(ResourceLoader *pLoader, SyncToken token) {
  bool completed = Os_AtomicLoad64_acquire(&pLoader->mTokenCompleted) >= token;
  return completed;
}

static void WaitTokenCompleted(ResourceLoader *pLoader, SyncToken token) {
  pLoader->mTokenMutex.Acquire();
  while (!IsTokenCompleted(token)) {
    pLoader->mTokenCond.Wait(pLoader->mTokenMutex, -1);
  }
  pLoader->mTokenMutex.Release();
}

//////////////////////////////////////////////////////////////////////////
// Resource Loader Implementation
//////////////////////////////////////////////////////////////////////////
static ResourceLoader *pResourceLoader = NULL;

void InitResourceLoaderInterface(Renderer *pRenderer, uint64_t memoryBudget, bool useThreads) {
  addResourceLoader(pRenderer, memoryBudget, &pResourceLoader);
}

void RemoveResourceLoaderInterface(Renderer *pRenderer) {
  removeResourceLoader(pResourceLoader);
}

void AddResource(BufferLoadDesc *pBufferDesc, bool batch) {
  SyncToken token = 0;
  AddResource(pBufferDesc, &token);
  if (!batch) { WaitTokenCompleted(token); }
}

void AddResource(TextureLoadDesc *pTextureDesc, bool batch) {
  SyncToken token = 0;
  AddResource(pTextureDesc, &token);
  if (!batch) { WaitTokenCompleted(token); }
}

void AddResource(BufferLoadDesc *pBufferDesc, SyncToken *token) {
  ASSERT(pBufferDesc->ppBuffer);

  bool update = pBufferDesc->pData || pBufferDesc->mForceReset;

  pBufferDesc->mDesc.mStartState =
      update ? RESOURCE_STATE_COMMON : util_determine_resource_start_state(&pBufferDesc->mDesc);
  AddBuffer(pResourceLoader->pRenderer, &pBufferDesc->mDesc, pBufferDesc->ppBuffer);

  if (update) {
    BufferUpdateDesc bufferUpdate(*pBufferDesc->ppBuffer, pBufferDesc->pData);
    UpdateResource(&bufferUpdate, token);
  }
}

void AddResource(TextureLoadDesc *pTextureDesc, SyncToken *token) {
  ASSERT(pTextureDesc->ppTexture);

  bool freeImage = false;
  Image *pImage = pTextureDesc->pImage;
  if (pTextureDesc->pFilename) {
    pImage = new Image();
    if (!pImage->loadImage(pTextureDesc->pFilename, pTextureDesc->mUseMipmaps, nullptr, nullptr)) {
      delete(pImage);
      return;
    }
    freeImage = true;
  } else if (!pTextureDesc->pFilename && !pTextureDesc->pImage) {
    pTextureDesc->pDesc->mStartState = util_determine_resource_start_state(pTextureDesc->pDesc->mDescriptors);
    AddTexture(pResourceLoader->pRenderer, pTextureDesc->pDesc, pTextureDesc->ppTexture);
    // TODO: what about barriers???
    // Only need transition for vulkan and durango since resource will decay to srv on graphics queue in PC dx12
    //if (pLoader->pRenderer->mSettings.mApi == RENDERER_API_VULKAN || pLoader->pRenderer->mSettings.mApi == RENDERER_API_XBOX_D3D12)
    //{
    //	TextureBarrier barrier = { *pEmptyTexture->ppTexture, pEmptyTexture->pDesc->mStartState };
    //	cmdResourceBarrier(pCmd, 0, NULL, 1, &barrier, true);
    //}
    return;
  }

  TextureDesc desc = {};
  desc.mFlags = pTextureDesc->mCreationFlag;
  desc.mWidth = pImage->GetWidth();
  desc.mHeight = pImage->GetHeight();
  desc.mDepth = Math_MaxU32(1U, pImage->GetDepth());
  desc.mArraySize = pImage->GetArrayCount();
  desc.mMipLevels = pImage->GetMipMapCount();
  desc.mSampleCount = SAMPLE_COUNT_1;
  desc.mSampleQuality = 0;
  desc.mFormat = pImage->getFormat();
  desc.mClearValue = ClearValue();
  desc.mDescriptors = DESCRIPTOR_TYPE_TEXTURE;
  desc.mStartState = RESOURCE_STATE_COPY_DEST;
  desc.pNativeHandle = NULL;
  desc.mHostVisible = false;
  desc.mSrgb = pTextureDesc->mSrgb;
  desc.mNodeIndex = pTextureDesc->mNodeIndex;

  if (pImage->IsCube()) {
    desc.mDescriptors |= DESCRIPTOR_TYPE_TEXTURE_CUBE;
    desc.mArraySize *= 6;
  }

  desc.pDebugName = pImage->GetName().c_str(); // unsafe it image gets deleted?

  AddTexture(pResourceLoader->pRenderer, &desc, pTextureDesc->ppTexture);

  TextureUpdateDesc updateDesc = {*pTextureDesc->ppTexture, pImage, freeImage};
  UpdateResource(&updateDesc, token);
}

void UpdateResource(BufferUpdateDesc *pBufferUpdate, bool batch) {
  SyncToken token = 0;
  UpdateResource(pBufferUpdate, &token);
  if (!batch) { WaitTokenCompleted(token); }
}

void UpdateResource(TextureUpdateDesc *pTextureUpdate, bool batch) {
  SyncToken token = 0;
  UpdateResource(pTextureUpdate, &token);
  if (!batch) { WaitTokenCompleted(token); }
}

void UpdateResources(uint32_t resourceCount, ResourceUpdateDesc *pResources) {
  SyncToken token = 0;
  UpdateResources(resourceCount, pResources, &token);
  WaitTokenCompleted(token);
}

void UpdateResource(BufferUpdateDesc *pBufferUpdate, SyncToken *token) {
  if (pBufferUpdate->pBuffer->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY ||
      pBufferUpdate->pBuffer->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_TO_CPU) {
    queueResourceUpdate(pResourceLoader, pBufferUpdate, token);
  } else {
    updateCPUbuffer(pResourceLoader->pRenderer, pBufferUpdate);
  }
}

void UpdateResource(TextureUpdateDesc *pTextureUpdate, SyncToken *token) {
  queueResourceUpdate(pResourceLoader, pTextureUpdate, token);
}

void UpdateResources(uint32_t resourceCount, ResourceUpdateDesc *pResources, SyncToken *token) {
  for (uint32_t i = 0; i < resourceCount; ++i) {
    if (pResources[i].mType == RESOURCE_TYPE_BUFFER) {
      UpdateResource(&pResources[i].buf, token);
    } else {
      UpdateResource(&pResources[i].tex, token);
    }
  }
}

void RemoveResource(Texture *pTexture) {
  RemoveTexture(pResourceLoader->pRenderer, pTexture);
}

void RemoveResource(Buffer *pBuffer) {
  RemoveBuffer(pResourceLoader->pRenderer, pBuffer);
}

bool IsTokenCompleted(SyncToken token) {
  return IsTokenCompleted(pResourceLoader, token);
}

void waitTokenCompleted(SyncToken token) {
  WaitTokenCompleted(pResourceLoader, token);
}

void waitBatchCompleted() {
  SyncToken token = Os_AtomicLoad64_relaxed(&pResourceLoader->mTokenCounter);
  waitTokenCompleted(token);
}

void flushResourceUpdates() {
  waitBatchCompleted();
}

void finishResourceLoading() {
  waitBatchCompleted();
}

/************************************************************************/
// Shader loading
/************************************************************************/
#if defined(__ANDROID__)
// Translate Vulkan Shader Type to shaderc shader type
shaderc_shader_kind getShadercShaderType(ShaderStage type)
{
    switch (type)
    {
        case ShaderStage::SHADER_STAGE_VERT: return shaderc_glsl_vertex_shader;
        case ShaderStage::SHADER_STAGE_FRAG: return shaderc_glsl_fragment_shader;
        case ShaderStage::SHADER_STAGE_TESC: return shaderc_glsl_tess_control_shader;
        case ShaderStage::SHADER_STAGE_TESE: return shaderc_glsl_tess_evaluation_shader;
        case ShaderStage::SHADER_STAGE_GEOM: return shaderc_glsl_geometry_shader;
        case ShaderStage::SHADER_STAGE_COMP: return shaderc_glsl_compute_shader;
        default: ASSERT(0); abort();
    }
    return static_cast<shaderc_shader_kind>(-1);
}
#endif

#if defined(VULKAN)
#if defined(__ANDROID__)
// Android:
// Use shaderc to compile glsl to spirV
//@todo add support to macros!!
void vk_compileShader(
    Renderer* pRenderer, ShaderStage stage, uint32_t codeSize, const char* code, const tinystl::string& outFile, uint32_t macroCount,
    ShaderMacro* pMacros, tinystl::vector<char>* pByteCode, const char* pEntryPoint)
{
    // compile into spir-V shader
    shaderc_compiler_t           compiler = shaderc_compiler_initialize();
    shaderc_compilation_result_t spvShader =
        shaderc_compile_into_spv(compiler, code, codeSize, getShadercShaderType(stage), "shaderc_error", pEntryPoint ? pEntryPoint : "main", nullptr);
    if (shaderc_result_get_compilation_status(spvShader) != shaderc_compilation_status_success)
    {
        LOGERRORF("Shader compiling failed! with status");
        abort();
    }

    // Resize the byteCode block based on the compiled shader size
    pByteCode->resize(shaderc_result_get_length(spvShader));
    memcpy(pByteCode->data(), shaderc_result_get_bytes(spvShader), pByteCode->size());

    // Release resources
    shaderc_result_release(spvShader);
    shaderc_compiler_release(compiler);
}
#else
// PC:
// Vulkan has no builtin functions to compile source to spirv
// So we call the glslangValidator tool located inside VulkanSDK on user machine to compile the glsl code to spirv
// This code is not added to Vulkan.cpp since it calls no Vulkan specific functions
void vk_compileShader(
    Renderer* pRenderer, ShaderTarget target, const tinystl::string& fileName, const tinystl::string& outFile, uint32_t macroCount,
    ShaderMacro* pMacros, tinystl::vector<char>* pByteCode, const char* pEntryPoint)
{
    if (!FileSystem::DirExists(FileSystem::GetPath(outFile)))
        FileSystem::CreateDir(FileSystem::GetPath(outFile));

    tinystl::string                  commandLine;
    tinystl::vector<tinystl::string> args;
    tinystl::string                  configFileName;

    // If there is a config file located in the shader source directory use it to specify the limits
    if (FileSystem::FileExists(FileSystem::GetPath(fileName) + "/config.conf", FSRoot::FSR_Absolute))
    {
        configFileName = FileSystem::GetPath(fileName) + "/config.conf";
        // Add command to compile from Vulkan GLSL to Spirv
        commandLine += tinystl::string::format(
            "\"%s\" -V \"%s\" -o \"%s\"", configFileName.size() ? configFileName.c_str() : "", fileName.c_str(), outFile.c_str());
    }
    else
    {
        commandLine += tinystl::string::format("-V \"%s\" -o \"%s\"", fileName.c_str(), outFile.c_str());
    }

    if (target >= shader_target_6_0)
        commandLine += " --target-env vulkan1.1 ";
        //commandLine += " \"-D" + tinystl::string("VULKAN") + "=" + "1" + "\"";

    if (pEntryPoint != nullptr)
        commandLine += tinystl::string::format(" -e %s", pEntryPoint);

        // Add platform macro
#ifdef _WINDOWS
    commandLine += " \"-D WINDOWS\"";
#elif defined(__ANDROID__)
    commandLine += " \"-D ANDROID\"";
#elif defined(__linux__)
    commandLine += " \"-D LINUX\"";
#endif

    // Add user defined macros to the command line
    for (uint32_t i = 0; i < macroCount; ++i)
    {
        commandLine += " \"-D" + pMacros[i].definition + "=" + pMacros[i].value + "\"";
    }
    args.push_back(commandLine);

    tinystl::string glslangValidator = getenv("VULKAN_SDK");
    if (glslangValidator.size())
        glslangValidator += "/bin/glslangValidator";
    else
        glslangValidator = "/usr/bin/glslangValidator";
    if (FileSystem::SystemRun(glslangValidator, args, outFile + "_compile.log") == 0)
    {
        File file = {};
        file.Open(outFile, FileMode::FM_ReadBinary, FSRoot::FSR_Absolute);
        ASSERT(file.IsOpen());
        pByteCode->resize(file.GetSize());
        memcpy(pByteCode->data(), file.ReadText().c_str(), pByteCode->size());
        file.Close();
    }
    else
    {
        File errorFile = {};
        errorFile.Open(outFile + "_compile.log", FM_ReadBinary, FSR_Absolute);
        // If for some reason the error file could not be created just log error msg
        if (!errorFile.IsOpen())
        {
            ErrorMsg("Failed to compile shader %s", fileName.c_str());
        }
        else
        {
            tinystl::string errorLog = errorFile.ReadText();
            errorFile.Close();
            ErrorMsg("Failed to compile shader %s with error\n%s", fileName.c_str(), errorLog.c_str());
            errorFile.Close();
        }
    }
}
#endif
#elif defined(METAL)
// On Metal, on the other hand, we can compile from code into a MTLLibrary, but cannot save this
// object's bytecode to disk. We instead use the xcbuild bash tool to compile the shaders.
void mtl_compileShader(
    Renderer *pRenderer,
    const tinystl::string& fileName,
    const tinystl::string& outFile,
    uint32_t macroCount,
    ShaderMacro *pMacros,
    tinystl::vector<char> *pByteCode,
    const char * /*pEntryPoint*/) {

  if (!Os::FileSystem::DirExists(Os::FileSystem::GetParentPath(outFile)))
    Os::FileSystem::CreateDir(Os::FileSystem::GetParentPath(outFile));

  tinystl::string xcrun = "/usr/bin/xcrun";
  tinystl::string intermediateFile = outFile + ".air";
  tinystl::vector<tinystl::string> args;
  tinystl::string tmpArg = "";

  // Compile the source into a temporary .air file.
  args.push_back("-sdk");
  args.push_back("macosx");
  args.push_back("metal");
  args.push_back("-c");
  tmpArg = tinystl::string::format(
      ""
      "%s"
      "",
      fileName.c_str());
  args.push_back(tmpArg);
  args.push_back("-o");
  args.push_back(intermediateFile.c_str());

  //enable the 2 below for shader debugging on xcode 10.0
  //args.push_back("-MO");
  //args.push_back("-gline-tables-only");
  args.push_back("-D");
  args.push_back("MTL_SHADER=1");    // Add MTL_SHADER macro to differentiate structs in headers shared by app/shader code.
  // Add user defined macros to the command line
  for (uint32_t i = 0; i < macroCount; ++i) {
    args.push_back("-D");
    args.push_back(tinystl::string::format("%s = %i", pMacros[i].definition, pMacros[i].value));
  }
  if (Os::FileSystem::SystemRun(xcrun, args) == 0) {
    // Create a .metallib file from the .air file.
    args.clear();
    tmpArg = "";
    args.push_back("-sdk");
    args.push_back("macosx");
    args.push_back("metallib");
    args.push_back(intermediateFile.c_str());
    args.push_back("-o");
    tmpArg = tinystl::string::format(
        ""
        "%s"
        "",
        outFile.c_str());
    args.push_back(tmpArg);
    if (Os::FileSystem::SystemRun(xcrun, args) == 0) {
      // Remove the temp .air file.
      args.clear();
      args.push_back(intermediateFile.c_str());
      Os::FileSystem::SystemRun("rm", args);

      // Store the compiled bytecode.
      VFile::ScopedFile file(VFile::File::FromFile(outFile, Os_FM_ReadBinary));
      ASSERT(file);
      pByteCode->resize(file->Size());
      memcpy(pByteCode->data(), file->ReadString().c_str(), pByteCode->size());
    } else
      LOGERRORF("Failed to assemble shader's %s .metallib file", fileName.c_str());
  } else {
    LOGERRORF("Failed to compile shader %s", fileName.c_str());
  }
}
#endif
#if (defined(DIRECT3D12) || defined(DIRECT3D11)) && !defined(ENABLE_RENDERER_RUNTIME_SWITCH)
extern void compileShader(
    Renderer* pRenderer, ShaderTarget target, ShaderStage stage, const char* fileName, uint32_t codeSize, const char* code,
    uint32_t macroCount, ShaderMacro* pMacros, void* (*allocator)(size_t a), uint32_t* pByteCodeSize, char** ppByteCode, const char* pEntryPoint);
#endif

// Function to generate the timestamp of this shader source file considering all include file timestamp
static bool process_source_file(VFile::File *original,
                                VFile::File *file,
                                uint64_t& outTimeStamp,
                                tinystl::string& outCode) {
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
      if (!process_source_file(original, includeFile.owned, outTimeStamp, outCode)) {
        return false;
      }
    }

#ifdef TARGET_IOS
    // iOS doesn't have support for resolving user header includes in shader code
    // when compiling with shader source using Metal runtime.
    // https://developer.apple.com/library/archive/documentation/3DDrawing/Conceptual/MTLBestPracticesGuide/FunctionsandLibraries.html
    //
    // Here we write out the contents of the header include into the original source
    // where its included from -- we're expanding the headers as the pre-processor
    // would do.
    //
    //const bool bAreWeProcessingAnIncludedHeader = file != original;
    if (!bLineHasIncludeDirective)
    {
        outCode += line + "\n";
    }
#else
    // Simply write out the current line if we are not in a header file
    const bool bAreWeProcessingTheShaderSource = file == original;
    if (bAreWeProcessingTheShaderSource) {
      outCode += line + "\n";
    }
#endif
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

bool load_shader_stage_byte_code(
    Renderer *pRenderer,
    ShaderTarget target,
    ShaderStage stage,
    const char *fileName,
    uint32_t macroCount,
    ShaderMacro *pMacros,
    uint32_t rendererMacroCount,
    ShaderMacro *pRendererMacros,
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

  if (!process_source_file(shaderSource.owned, shaderSource.owned, timeStamp, code)) {
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

#if 0    //#ifdef _DURANGO
  // Using Durango application data storage requires appmanifest(from application) changes.
  tinystl::string binaryShaderName = FileSystem::GetAppPreferencesDir(NULL,NULL) + "/" + pRenderer->pName + "/CompiledShadersBinary/" +
      FileSystem::GetFileName(fileName) + tinystl::string::format("_%zu", tinystl::hash(shaderDefines)) + extension + ".bin";
#else
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
#endif

  // Shader source is newer than binary
  if (!check_for_byte_code(binaryShaderName, timeStamp, byteCode)) {
    if (pRenderer->mSettings.mApi == RENDERER_API_METAL || pRenderer->mSettings.mApi == RENDERER_API_VULKAN) {
#if defined(VULKAN)
#if defined(__ANDROID__)
      vk_compileShader(pRenderer, stage, (uint32_t)code.size(), code.c_str(), binaryShaderName, macroCount, pMacros, &byteCode, pEntryPoint);
#else
      vk_compileShader(pRenderer, target, shaderSource.GetName(), binaryShaderName, macroCount, pMacros, &byteCode, pEntryPoint);
#endif
#elif defined(METAL)
      mtl_compileShader(pRenderer,
                        shaderSource->GetName(),
                        binaryShaderName,
                        macroCount,
                        pMacros,
                        &byteCode,
                        pEntryPoint);
#endif
    } else {
#if defined(DIRECT3D12) || defined(DIRECT3D11)
      char*    pByteCode = NULL;
      uint32_t byteCodeSize = 0;
      compileShader(
          pRenderer, target, stage, shaderSource.GetName(), (uint32_t)code.size(), code.c_str(), macroCount, pMacros, conf_malloc,
          &byteCodeSize, &pByteCode, pEntryPoint);
      byteCode.resize(byteCodeSize);
      memcpy(byteCode.data(), pByteCode, byteCodeSize);
      conf_free(pByteCode);
      if (!save_byte_code(binaryShaderName, byteCode))
      {
          const char* shaderName = shaderSource.GetName();
          LOGWARNINGF("Failed to save byte code for file %s", shaderName);
      }
#endif
    }
    if (!byteCode.size()) {
      LOGERRORF("Error while generating bytecode for shader %s", fileName);
      return false;
    }
  }

  return true;
}
#ifdef TARGET_IOS
bool find_shader_stage(const tinystl::string& fileName, ShaderDesc* pDesc, ShaderStageDesc** pOutStage, ShaderStage* pStage)
{
    tinystl::string ext = FileSystem::GetExtension(fileName);
    if (ext == ".vert")
    {
        *pOutStage = &pDesc->mVert;
        *pStage = SHADER_STAGE_VERT;
    }
    else if (ext == ".frag")
    {
        *pOutStage = &pDesc->mFrag;
        *pStage = SHADER_STAGE_FRAG;
    }
#ifndef METAL
    else if (ext == ".tesc")
    {
        *pOutStage = &pDesc->mHull;
        *pStage = SHADER_STAGE_HULL;
    }
    else if (ext == ".tese")
    {
        *pOutStage = &pDesc->mDomain;
        *pStage = SHADER_STAGE_DOMN;
    }
    else if (ext == ".geom")
    {
        *pOutStage = &pDesc->mGeom;
        *pStage = SHADER_STAGE_GEOM;
    }
#endif
    else if (ext == ".comp")
    {
        *pOutStage = &pDesc->mComp;
        *pStage = SHADER_STAGE_COMP;
    }
    else if ((ext == ".rgen")    ||
             (ext == ".rmiss")    ||
             (ext == ".rchit")    ||
             (ext == ".rint")    ||
             (ext == ".rahit")    ||
             (ext == ".rcall"))
    {
        *pOutStage = &pDesc->mComp;
        *pStage = SHADER_STAGE_COMP;
    }
    else
    {
        return false;
    }

    return true;
}
#else
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
#ifndef METAL
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
#endif
  else if (ext == ".comp") {
    *pOutStage = &pBinaryDesc->mComp;
    *pStage = SHADER_STAGE_COMP;
  } else if ((ext == ".rgen") ||
      (ext == ".rmiss") ||
      (ext == ".rchit") ||
      (ext == ".rint") ||
      (ext == ".rahit") ||
      (ext == ".rcall")) {
#ifndef METAL
    *pOutStage = &pBinaryDesc->mComp;
    *pStage = SHADER_STAGE_LIB;
#else
    *pOutStage = &pBinaryDesc->mComp;
    *pStage = SHADER_STAGE_COMP;
#endif
  } else {
    return false;
  }

  return true;
}
#endif
void addShader(Renderer *pRenderer, const ShaderLoadDesc *pDesc, Shader **ppShader) {
#ifndef TARGET_IOS
  BinaryShaderDesc binaryDesc = {};
  tinystl::vector<char> byteCodes[SHADER_STAGE_COUNT] = {};
  for (uint32_t i = 0; i < SHADER_STAGE_COUNT; ++i) {
    const RendererShaderDefinesDesc rendererDefinesDesc = get_renderer_shaderdefines(pRenderer);

    if (pDesc->mStages[i].mFileName.size() != 0) {
      tinystl::string filename = pDesc->mStages[i].mFileName;

      ShaderStage stage;
      BinaryShaderStageDesc *pStage = NULL;
      if (find_shader_stage(filename, &binaryDesc, &pStage, &stage)) {
        if (!load_shader_stage_byte_code(
            pRenderer,
            pDesc->mTarget,
            stage,
            filename.c_str(),
            pDesc->mStages[i].mMacroCount,
            pDesc->mStages[i].pMacros,
            rendererDefinesDesc.rendererShaderDefinesCnt,
            rendererDefinesDesc.rendererShaderDefines,
            byteCodes[i],
            pDesc->mStages[i].mEntryPointName)) {
          return;
        }

        binaryDesc.mStages = (ShaderStage)(binaryDesc.mStages | stage);
        pStage->pByteCode = byteCodes[i].data();
        pStage->mByteCodeSize = (uint32_t) byteCodes[i].size();
#if defined(METAL)
        if (pDesc->mStages[i].mEntryPointName)
          pStage->mEntryPoint = pDesc->mStages[i].mEntryPointName;
        else
          pStage->mEntryPoint = "stageMain";
        // In metal, we need the shader source for our reflection system.
        VFile::ScopedFile metalFile(VFile::File::FromFile(filename + ".metal", Os_FM_ReadBinary));
        tinystl::string shaderSource = metalFile->ReadString();
        pStage->mSource = (char*) malloc(shaderSource.size()+1); // leak here!
        strcpy((char*)pStage->mSource, shaderSource.c_str());

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

  AddShaderBinary(pRenderer, &binaryDesc, ppShader);
#else
  // Binary shaders are not supported on iOS.
  ShaderDesc desc = {};
  for (uint32_t i = 0; i < SHADER_STAGE_COUNT; ++i)
  {
      const RendererShaderDefinesDesc rendererDefinesDesc = get_renderer_shaderdefines(pRenderer);

      if (pDesc->mStages[i].mFileName.size() > 0)
      {
          tinystl::string filename = pDesc->mStages[i].mFileName;
          if (pDesc->mStages[i].mRoot != FSR_SrcShaders)
              filename = FileSystem::GetRootPath(FSR_SrcShaders) + filename;

          ShaderStage stage;
          ShaderStageDesc* pStage = NULL;
          if (find_shader_stage(filename, &desc, &pStage, &stage))
          {
              File shaderSource = {};
              shaderSource.Open(filename + ".metal", FM_ReadBinary, pDesc->mStages[i].mRoot);
              ASSERT(shaderSource.IsOpen());

              pStage->mName = pDesc->mStages[i].mFileName;
              uint timestamp = 0;
              process_source_file(&shaderSource, &shaderSource, timestamp, pStage->mCode);
              if (pDesc->mStages[i].mEntryPointName)
                  pStage->mEntryPoint = pDesc->mStages[i].mEntryPointName;
              else
                  pStage->mEntryPoint = "stageMain";
              // Apply user specified shader macros
              for (uint32_t j = 0; j < pDesc->mStages[i].mMacroCount; j++)
              {
                  pStage->mMacros.push_back(pDesc->mStages[i].pMacros[j]);
              }
              // Apply renderer specified shader macros
              for (uint32_t j = 0; j < rendererDefinesDesc.rendererShaderDefinesCnt; j++)
              {
                  pStage->mMacros.push_back(rendererDefinesDesc.rendererShaderDefines[j]);
              }
              shaderSource.Close();
              desc.mStages |= stage;
          }
      }
  }

  addShader(pRenderer, &desc, ppShader);
#endif
}

} // end namespace