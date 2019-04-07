#include "core/core.h"
#include "os/atomics.h"
#include "theforge/renderer.h"
#include "image/image.h"
#include "theforge_resourceloader/theforge_resourceloader.hpp"

using namespace TheForge::ResourceLoader;
EXTERN_C void TheForge_ResourceLoader_Init(
    TheForge_Renderer *pRenderer,
    uint64_t memoryBudget,
    bool useThreads,
    TheForge_ResourceLoader_Handle *handle) {
  Init(pRenderer, memoryBudget, useThreads, handle);
}
EXTERN_C void TheForge_ResourceLoader_Destroy(TheForge_ResourceLoader_Handle handle) {
  Destroy(handle);
}

EXTERN_C void TheForge_ResourceLoader_RemoveBuffer(
    TheForge_ResourceLoader_Handle handle,
    TheForge_Buffer *pBuffer) {
  RemoveBuffer(handle, pBuffer);
}
EXTERN_C void TheForge_ResourceLoader_RemoveTexture(
    TheForge_ResourceLoader_Handle handle,
    TheForge_Texture *pTexture) {
  RemoveTexture(handle,pTexture);
}
EXTERN_C void TheForge_ResourceLoader_FlushResourceUpdates(
    TheForge_ResourceLoader_Handle handle) {
  FlushResourceUpdates(handle);
}
EXTERN_C void TheForge_ResourceLoader_FinishResourceLoading(
    TheForge_ResourceLoader_Handle handle) {
  FinishResourceLoading(handle);
}

// sync interface
EXTERN_C void TheForge_ResourceLoader_UpdateResources(
    TheForge_ResourceLoader_Handle handle,
    uint32_t resourceCount,
    TheForge_ResourceLoader_ResourceUpdateDesc *pResources) {
  UpdateResources(handle, resourceCount, pResources);
}
EXTERN_C void TheForge_ResourceLoader_AddBuffer(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_BufferLoadDesc *pBuffer, bool batch) {
  AddBuffer(handle, pBuffer, batch);
}
EXTERN_C void TheForge_ResourceLoader_AddTexture(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_TextureLoadDesc *pTexture,
    bool batch) {
  AddTexture(handle, pTexture, batch);
}
EXTERN_C void TheForge_ResourceLoader_UpdateBuffer(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_BufferUpdateDesc *pBuffer,
    bool batch) {
  UpdateBuffer(handle, pBuffer, batch);
}
EXTERN_C void TheForge_ResourceLoader_UpdateTexture(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_TextureUpdateDesc *pTexture,
    bool batch) {
  UpdateTexture(handle, pTexture, batch);
}

// async interface
EXTERN_C void TheForge_ResourceLoader_UpdateResourcesAsync(
    TheForge_ResourceLoader_Handle handle,
    uint32_t resourceCount,
    TheForge_ResourceLoader_ResourceUpdateDesc *pResources,
    TheForge_ResourceLoader_SyncToken *token) {
  UpdateResourcesAsync(handle, resourceCount, pResources, token);
}
EXTERN_C void TheForge_ResourceLoader_WaitBatchCompleted(
    TheForge_ResourceLoader_Handle handle) {
  WaitBatchCompleted(handle);
}
EXTERN_C bool TheForge_ResourceLoader_IsTokenCompleted(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_SyncToken token) {
  return IsTokenCompleted(handle, token);
}
EXTERN_C void TheForge_ResourceLoader_WaitTokenCompleted(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_SyncToken token) {
  WaitTokenCompleted(handle, token);
}
EXTERN_C void TheForge_ResourceLoader_AddBufferAsync(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_BufferLoadDesc *pBufferDesc,
    TheForge_ResourceLoader_SyncToken *token) {
  AddBufferAsync(handle, pBufferDesc, token);
}
EXTERN_C void TheForge_ResourceLoader_AddTextureAsync(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_TextureLoadDesc *pTextureDesc,
    TheForge_ResourceLoader_SyncToken *token) {
  AddTextureAsync(handle, pTextureDesc, token);
}
EXTERN_C void TheForge_ResourceLoader_UpdateBufferAsync(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_BufferUpdateDesc *pBuffer,
    TheForge_ResourceLoader_SyncToken *token) {
  UpdateBufferAsync(handle, pBuffer, token);
}
EXTERN_C void TheForge_ResourceLoader_UpdateTextureAsync(
    TheForge_ResourceLoader_Handle handle,
    TheForge_ResourceLoader_TextureUpdateDesc *pTexture,
    TheForge_ResourceLoader_SyncToken *token) {
  UpdateTextureAsync(handle, pTexture, token);
}
