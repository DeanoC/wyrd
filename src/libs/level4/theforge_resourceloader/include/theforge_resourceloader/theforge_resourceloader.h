#pragma once
#ifndef WYRD_THEFORGE_RESOURCELOADER_RESOURCELOADER_H
#define WYRD_THEFORGE_RESOURCELOADER_RESOURCELOADER_H

#include "core/core.h"
#include "os/atomics.h"
#include "theforge/renderer.h"
#include "image/image.h"
#include "theforge_shaderreflection/theforge_shaderreflection.h"

typedef Os_atomic64_t TheForge_ResourceLoader_SyncToken;
typedef void *TheForge_ResourceLoader_Handle;

typedef struct TheForge_ResourceLoader_BufferLoadDesc {
  TheForge_Buffer **ppBuffer;
  const void *pData;
  TheForge_BufferDesc mDesc;
  /// Force Reset buffer to NULL
  bool mForceReset;
} TheForge_ResourceLoader_BufferLoadDesc;

typedef struct TheForge_ResourceLoader_TextureLoadDesc {
  TheForge_Texture **ppTexture;
  /// Load texture from image
  Image_ImageHeader *pImage;
  /// Load empty texture
  TheForge_TextureDesc *pDesc;
  /// Load texture from disk
  const char *pFilename;
  uint32_t mNodeIndex;
  bool mUseMipmaps;
  bool mSrgb;

  // Following is ignored if pDesc != NULL.  pDesc->mFlags will be considered instead.
  TheForge_TextureCreationFlags mCreationFlag;
} TheForge_ResourceLoader_TextureLoadDesc;

typedef struct TheForge_ResourceLoader_ShaderStageLoadDesc
{
  char const* mFileName;
  TheForge_ShaderReflection_Macro*    pMacros;
  uint32_t        mMacroCount;
  const char*     mEntryPointName;
} TheForge_ResourceLoader_ShaderStageLoadDesc;

typedef struct TheForge_ResourceLoader_ShaderLoadDesc
{
  TheForge_ResourceLoader_ShaderStageLoadDesc mStages[TheForge_SHADER_STAGE_COUNT];
  TheForge_ShaderTarget        mTarget;
} TheForge_ResourceLoader_ShaderLoadDesc;

typedef struct TheForge_ResourceLoader_BufferUpdateDesc {
  TheForge_Buffer *pBuffer;
  const void *pData;
  uint64_t mSrcOffset;
  uint64_t mDstOffset;
  uint64_t mSize;    // If 0, uses size of pBuffer
} TheForge_ResourceLoader_BufferUpdateDesc;

typedef struct TheForge_ResourceLoader_TextureUpdateDesc {
  TheForge_Texture *pTexture;
  Image_ImageHeader *pImage;
  bool freeImage;
} TheForge_ResourceLoader_TextureUpdateDesc;

enum {
  TheForge_ResourceLoader_RT_BUFFER = 0,
  TheForge_ResourceLoader_RT_TEXTURE,
};

typedef uint8_t TheForge_ResourceLoader_ResourceType;

typedef struct TheForge_ResourceLoader_ResourceLoadDesc {

  TheForge_ResourceLoader_ResourceType mType;
  union {
    TheForge_ResourceLoader_BufferLoadDesc buf;
    TheForge_ResourceLoader_TextureLoadDesc tex;
  };
} TheForge_ResourceLoader_ResourceLoadDesc;

typedef struct TheForge_ResourceLoader_ResourceUpdateDesc {
  TheForge_ResourceLoader_ResourceType mType;
  union {
    TheForge_ResourceLoader_BufferUpdateDesc buf;
    TheForge_ResourceLoader_TextureUpdateDesc tex;
  };
} TheForge_ResourceLoader_ResourceUpdateDesc;

EXTERN_C void TheForge_ResourceLoader_Init(
    TheForge_Renderer *pRenderer,
    uint64_t memoryBudget,
    bool useThreads,
    TheForge_ResourceLoader_Handle *handle);
EXTERN_C void TheForge_ResourceLoader_Destroy(
    TheForge_ResourceLoader_Handle handle);

EXTERN_C void TheForge_ResourceLoader_RemoveBuffer(
    TheForge_ResourceLoader_Handle handle,
    TheForge_Buffer *pBuffer);
EXTERN_C void TheForge_ResourceLoader_RemoveTexture(
    TheForge_ResourceLoader_Handle handle,
    TheForge_Texture *pTexture);
EXTERN_C void TheForge_ResourceLoader_FlushResourceUpdates(
    TheForge_ResourceLoader_Handle handle);
EXTERN_C void TheForge_ResourceLoader_FinishResourceLoading(
    TheForge_ResourceLoader_Handle handle);

// sync interface
EXTERN_C void TheForge_ResourceLoader_AddBuffer(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_BufferLoadDesc *pBuffer, bool batch);
EXTERN_C void TheForge_ResourceLoader_AddTexture(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_TextureLoadDesc *pTexture,
    bool batch);
EXTERN_C void TheForge_ResourceLoader_AddShader(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_ShaderLoadDesc *pDesc,
    TheForge_Shader **ppShader);

EXTERN_C void TheForge_ResourceLoader_UpdateResources(
    TheForge_ResourceLoader_Handle handle,
    uint32_t resourceCount,
    TheForge_ResourceLoader_ResourceUpdateDesc *pResources);
EXTERN_C void TheForge_ResourceLoader_UpdateBuffer(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_BufferUpdateDesc *pBuffer,
    bool batch);
EXTERN_C void TheForge_ResourceLoader_UpdateTexture(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_TextureUpdateDesc *pTexture,
    bool batch);

// async interface
EXTERN_C void TheForge_ResourceLoader_AddBufferAsync(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_BufferLoadDesc *pBufferDesc,
    TheForge_ResourceLoader_SyncToken *token);
EXTERN_C void TheForge_ResourceLoader_AddTextureAsync(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_TextureLoadDesc *pTextureDesc,
    TheForge_ResourceLoader_SyncToken *token);

EXTERN_C void TheForge_ResourceLoader_UpdateResourcesAsync(
    TheForge_ResourceLoader_Handle handle,
    uint32_t resourceCount,
    TheForge_ResourceLoader_ResourceUpdateDesc *pResources,
    TheForge_ResourceLoader_SyncToken *token);
EXTERN_C void TheForge_ResourceLoader_UpdateBufferAsync(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_BufferUpdateDesc *pBuffer,
    TheForge_ResourceLoader_SyncToken *token);
EXTERN_C void TheForge_ResourceLoader_UpdateTextureAsync(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_TextureUpdateDesc *pTexture,
    TheForge_ResourceLoader_SyncToken *token);

EXTERN_C void TheForge_ResourceLoader_WaitBatchCompleted(
    TheForge_ResourceLoader_Handle handle);
EXTERN_C bool TheForge_ResourceLoader_IsTokenCompleted(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_SyncToken token);
EXTERN_C void TheForge_ResourceLoader_WaitTokenCompleted(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_SyncToken token);

#endif //WYRD_THEFORGE_RESOURCELOADER_RESOURCELOADER_H
