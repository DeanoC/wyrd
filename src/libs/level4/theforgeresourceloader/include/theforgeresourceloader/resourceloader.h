#pragma once
#ifndef WYRD_THEFORGE_RESOURCELOADER_RESOURCELOADER_H
#define WYRD_THEFORGE_RESOURCELOADER_RESOURCELOADER_H

#include "core/core.h"
#include "os/atomics.h"
#include "theforge/renderer.h"

typedef Os_atomic64_t TheForgeResourceLoader_SyncToken;
typedef void *TheForgeResourceLoader_Handle;

typedef struct TheForgeResourceLoader_BufferLoadDesc {
  TheForge_Buffer **ppBuffer;
  const void *pData;
  TheForge_BufferDesc mDesc;
  /// Force Reset buffer to NULL
  bool mForceReset;
} TheForgeResourceLoader_BufferLoadDesc;

typedef struct TheForgeResourceLoader_TextureLoadDesc {
  TheForge_Texture **ppTexture;
  /// Load texture from image
  //Image *pImage;
  /// Load empty texture
  TheForge_TextureDesc *pDesc;
  /// Load texture from disk
  const char *pFilename;
  uint32_t mNodeIndex;
  bool mUseMipmaps;
  bool mSrgb;

  // Following is ignored if pDesc != NULL.  pDesc->mFlags will be considered instead.
  TheForge_TextureCreationFlags mCreationFlag;
} TheForgeResourceLoader_TextureLoadDesc;

typedef struct TheForgeResourceLoader_BufferUpdateDesc {
  TheForge_Buffer *pBuffer;
  const void *pData;
  uint64_t mSrcOffset;
  uint64_t mDstOffset;
  uint64_t mSize;    // If 0, uses size of pBuffer
} TheForgeResourceLoader_BufferUpdateDesc;

typedef struct TheForgeResourceLoader_TextureUpdateDesc {
  TheForge_Texture *pTexture;
  //TheForge_Image *pImage;
  bool freeImage;
} TheForgeResourceLoader_TextureUpdateDesc;

enum {
  TheForgeResourceLoader_RT_BUFFER = 0,
  TheForgeResourceLoader_RT_TEXTURE,
};
typedef uint8_t TheForgeResourceLoader_ResourceType;

typedef struct TheForgeResourceLoader_ResourceLoadDesc {

  TheForgeResourceLoader_ResourceType mType;
  union {
    TheForgeResourceLoader_BufferLoadDesc buf;
    TheForgeResourceLoader_TextureLoadDesc tex;
  };
} TheForgeResourceLoader_ResourceLoadDesc;

typedef struct TheForgeResourceLoader_ResourceUpdateDesc {
  TheForgeResourceLoader_ResourceType mType;
  union {
    TheForgeResourceLoader_BufferUpdateDesc buf;
    TheForgeResourceLoader_TextureUpdateDesc tex;
  };
} TheForgeResourceLoader_ResourceUpdateDesc;

EXTERN_C void TheForgeResourceLoader_Init(
    TheForge_Renderer *pRenderer,
    uint64_t memoryBudget,
    bool useThreads,
    TheForgeResourceLoader_Handle *handle);

EXTERN_C void TheForgeResourceLoader_Remove(TheForgeResourceLoader_Handle handle);

EXTERN_C void TheForgeResourceLoader_UpdateResources(
    TheForgeResourceLoader_Handle handle,
    uint32_t resourceCount,
    TheForgeResourceLoader_ResourceUpdateDesc *pResources);

EXTERN_C void TheForgeResourceLoader_UpdateResourcesAsync(
    TheForgeResourceLoader_Handle handle,
    uint32_t resourceCount,
    TheForgeResourceLoader_ResourceUpdateDesc *pResources,
    TheForgeResourceLoader_SyncToken *token);

EXTERN_C void TheForgeResourceLoader_UpdateBuffer(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_BufferUpdateDesc *pBuffer,
    bool batch);

EXTERN_C void TheForgeResourceLoader_WaitBatchCompleted(
    TheForgeResourceLoader_Handle handle);
EXTERN_C bool TheForgeResourceLoader_IsTokenCompleted(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_SyncToken token);
EXTERN_C void TheForgeResourceLoader_WaitTokenCompleted(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_SyncToken token);

EXTERN_C void TheForgeResourceLoader_AddBuffer(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_BufferLoadDesc *pBuffer, bool batch);
EXTERN_C void TheForgeResourceLoader_AddTexture(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_TextureLoadDesc *pTexture,
    bool batch);
EXTERN_C void TheForgeResourceLoader_AddBufferAsync(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_BufferLoadDesc *pBufferDesc,
    TheForgeResourceLoader_SyncToken *token);
EXTERN_C void TheForgeResourceLoader_AddTextureAsync(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_TextureLoadDesc *pTextureDesc,
    TheForgeResourceLoader_SyncToken *token);
EXTERN_C void TheForgeResourceLoader_UpdateTexture(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_TextureUpdateDesc *pTexture,
    bool batch);
EXTERN_C void TheForgeResourceLoader_UpdateBufferAsync(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_BufferUpdateDesc *pBuffer,
    TheForgeResourceLoader_SyncToken *token);
EXTERN_C void TheForgeResourceLoader_UpdateTextureAsync(
    TheForgeResourceLoader_Handle handle,
    TheForgeResourceLoader_TextureUpdateDesc *pTexture,
    TheForgeResourceLoader_SyncToken *token);
EXTERN_C void TheForgeResourceLoader_RemoveBuffer(
    TheForgeResourceLoader_Handle handle,
    TheForge_Buffer *pBuffer);
EXTERN_C void TheForgeResourceLoader_RemoveTexture(
    TheForgeResourceLoader_Handle handle,
    TheForge_Texture *pTexture);
EXTERN_C void TheForgeResourceLoader_FlushResourceUpdates(
    TheForgeResourceLoader_Handle handle);
EXTERN_C void TheForgeResourceLoader_FinishResourceLoading(
    TheForgeResourceLoader_Handle handle);

#endif //WYRD_THEFORGE_RESOURCELOADER_RESOURCELOADER_H
