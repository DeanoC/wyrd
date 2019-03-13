// ***************************************************
// NOTE:
// "IRenderer.h" MUST be included before this header!
// ***************************************************

#pragma once

#include "Image/image.h"
#include "theforge/renderer_structs.hpp"
#include "os/thread.h"

#if !defined(TARGET_IOS)
#define DEFAULT_MEMORY_BUDGET (uint64_t)8e+7
#else
#define DEFAULT_MEMORY_BUDGET (uint64_t)6e+7
#endif

namespace TheForge {

struct BufferLoadDesc {
  Buffer **ppBuffer;
  const void *pData;
  BufferDesc mDesc;
  /// Force Reset buffer to NULL
  bool mForceReset;
};

struct TextureLoadDesc {
  Texture **ppTexture;
  /// Load texture from image
  Image *pImage;
  /// Load empty texture
  TextureDesc *pDesc;
  /// Load texture from disk
  const char *pFilename;
  uint32_t mNodeIndex;
  bool mUseMipmaps;
  bool mSrgb;

  // Following is ignored if pDesc != NULL.  pDesc->mFlags will be considered instead.
  TextureCreationFlags mCreationFlag;
};

struct BufferUpdateDesc {
  BufferUpdateDesc(TheForge_Buffer *buf = NULL,
                   const void *data = NULL,
                   uint64_t srcOff = 0,
                   uint64_t dstOff = 0,
                   uint64_t size = 0) :
      pBuffer(buf),
      pData(data),
      mSrcOffset(srcOff),
      mDstOffset(dstOff),
      mSize(size) {
  }

  Buffer *pBuffer;
  const void *pData;
  uint64_t mSrcOffset;
  uint64_t mDstOffset;
  uint64_t mSize;    // If 0, uses size of pBuffer
};

struct TextureUpdateDesc {
  Texture *pTexture;
  Image *pImage;
  bool freeImage;
};

enum ResourceType {
  RESOURCE_TYPE_BUFFER = 0,
  RESOURCE_TYPE_TEXTURE,
};

struct ResourceLoadDesc {
  ResourceLoadDesc(BufferLoadDesc& buffer) : mType(RESOURCE_TYPE_BUFFER), buf(buffer) {}
  ResourceLoadDesc(TextureLoadDesc& texture) : mType(RESOURCE_TYPE_TEXTURE), tex(texture) {}

  ResourceType mType;
  union {
    BufferLoadDesc buf;
    TextureLoadDesc tex;
  };
};

struct ResourceUpdateDesc {
  ResourceUpdateDesc(BufferUpdateDesc& buffer) : mType(RESOURCE_TYPE_BUFFER), buf(buffer) {}
  ResourceUpdateDesc(TextureUpdateDesc& texture) : mType(RESOURCE_TYPE_TEXTURE), tex(texture) {}

  ResourceType mType;
  union {
    BufferUpdateDesc buf;
    TextureUpdateDesc tex;
  };
};

struct ShaderStageLoadDesc {
  tinystl::string mFileName;
  ShaderMacro *pMacros;
  uint32_t mMacroCount;
  const char *mEntryPointName;
};

struct ShaderLoadDesc {
  ShaderStageLoadDesc mStages[TheForge_SHADER_STAGE_COUNT];
  ShaderTarget mTarget;
};

typedef Os_atomic64_t SyncToken;

void InitResourceLoaderInterface(Renderer *pRenderer,
                                 uint64_t memoryBudget = DEFAULT_MEMORY_BUDGET,
                                 bool useThreads = false);
void RemoveResourceLoaderInterface(Renderer *pRenderer);

void AddResource(BufferLoadDesc *pBuffer, bool batch = false);
void AddResource(TextureLoadDesc *pTexture, bool batch = false);
void AddResource(BufferLoadDesc *pBufferDesc, SyncToken *token);
void AddResource(TextureLoadDesc *pTextureDesc, SyncToken *token);

void UpdateResource(BufferUpdateDesc *pBuffer, bool batch = false);
void UpdateResource(TextureUpdateDesc *pTexture, bool batch = false);
void UpdateResources(uint32_t resourceCount, ResourceUpdateDesc *pResources);
void UpdateResource(BufferUpdateDesc *pBuffer, SyncToken *token);
void UpdateResource(TextureUpdateDesc *pTexture, SyncToken *token);
void UpdateResources(uint32_t resourceCount, ResourceUpdateDesc *pResources, SyncToken *token);

void WaitBatchCompleted();
bool IsTokenCompleted(SyncToken token);
void WaitTokenCompleted(SyncToken token);

void RemoveResource(Buffer *pBuffer);
void RemoveResource(Texture *pTexture);

/// Either loads the cached shader bytecode or compiles the shader to create new bytecode depending on whether source is newer than binary
void AddShader(Renderer *pRenderer, const ShaderLoadDesc *pDesc, Shader **ppShader);

void FlushResourceUpdates();
void FinishResourceLoading();

} // end namespace TheForge

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
