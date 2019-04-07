#pragma once
#ifndef WYRD_THEFORGE_RESOURCELOADER_THEFORGE_RESOURCELOADER_HPP
#define WYRD_THEFORGE_RESOURCELOADER_THEFORGE_RESOURCELOADER_HPP

#include "core/core.h"
#include "theforge/renderer.hpp"
#include "theforge_resourceloader/theforge_resourceloader.h"

namespace TheForge { namespace ResourceLoader {
using SyncToken = TheForge_ResourceLoader_SyncToken;
using Handle = TheForge_ResourceLoader_Handle;

using BufferLoadDesc = TheForge_ResourceLoader_BufferLoadDesc;
using TextureLoadDesc = TheForge_ResourceLoader_TextureLoadDesc;
using BufferUpdateDesc = TheForge_ResourceLoader_BufferUpdateDesc;

using TextureUpdateDesc = TheForge_ResourceLoader_TextureUpdateDesc;

static const uint8_t RT_BUFFER = TheForge_ResourceLoader_RT_BUFFER;
static const uint8_t RT_TEXTURE = TheForge_ResourceLoader_RT_TEXTURE;
using ResourceType = TheForge_ResourceLoader_ResourceType;
using ResourceLoadDesc = TheForge_ResourceLoader_ResourceLoadDesc;
using ResourceUpdateDesc = TheForge_ResourceLoader_ResourceUpdateDesc;

void Init(Renderer *pRenderer, uint64_t memoryBudget, bool useThreads, Handle *handle);
void Destroy(Handle handle);

void UpdateResources(Handle handle, uint32_t resourceCount, ResourceUpdateDesc *pResources);
void UpdateResourcesAsync(Handle handle, uint32_t resourceCount, ResourceUpdateDesc *pResources, SyncToken *token);

void WaitBatchCompleted(Handle handle);
bool IsTokenCompleted(Handle handle, SyncToken token);
void WaitTokenCompleted(Handle handle, SyncToken token);

void AddBuffer(Handle handle, BufferLoadDesc *pBuffer, bool batch);
void AddTexture(Handle handle, TextureLoadDesc *pTexture, bool batch);
void AddBufferAsync(Handle handle, BufferLoadDesc *pBufferDesc, SyncToken *token);
void AddTextureAsync(Handle handle, TextureLoadDesc *pTextureDesc, SyncToken *token);
void RemoveBuffer(Handle handle, Buffer *pBuffer);
void RemoveTexture(Handle handle, Texture *pTexture);

void UpdateTexture(Handle handle, TextureUpdateDesc *pTexture, bool batch);
void UpdateBuffer(Handle handle, BufferUpdateDesc *pBuffer, bool batch);
void UpdateBufferAsync(Handle handle, BufferUpdateDesc *pBuffer, SyncToken *token);
void UpdateTextureAsync(Handle handle, TextureUpdateDesc *pTexture, SyncToken *token);

void FlushResourceUpdates(Handle handle);
void FinishResourceLoading(Handle handle);

} } // end namespace

#endif //WYRD_THEFORGE_RESOURCELOADER_THEFORGE_RESOURCELOADER_HPP
