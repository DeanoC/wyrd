#pragma once
#ifndef WYRD_THEFORGE_RENDERER_HPP
#define WYRD_THEFORGE_RENDERER_HPP

#include "core/core.h"
#include "theforge/renderer.hpp"
#include "theforge/renderer_descs.hpp"
#include "theforge/renderer_enums.hpp"
#include "theforge/renderer_structs.hpp"
#include "image/image.h"

namespace TheForge {

void InitRenderer(const char *app_name, const RendererDesc *p_settings, Renderer **ppRenderer);
void RemoveRenderer(Renderer *pRenderer);
void AddFence(Renderer *pRenderer, Fence **pp_fence);
void RemoveFence(Renderer *pRenderer, Fence *p_fence);
void AddSemaphore(Renderer *pRenderer, Semaphore **pp_semaphore);
void RemoveSemaphore(Renderer *pRenderer, Semaphore *p_semaphore);
void AddQueue(Renderer *pRenderer, QueueDesc *pQDesc, Queue **ppQueue);
void RemoveQueue(Renderer *pRenderer, Queue *pQueue);
void AddCmdPool(Renderer *pRenderer, Queue *p_queue, bool transient, CmdPool **pp_CmdPool);
void RemoveCmdPool(Renderer *pRenderer, CmdPool *p_CmdPool);
void AddCmd(CmdPool *p_CmdPool, bool secondary, Cmd **pp_cmd);
void RemoveCmd(CmdPool *p_CmdPool, Cmd *p_cmd);
void AddCmd_n(CmdPool *p_CmdPool, bool secondary, uint32_t cmd_count, Cmd ***ppp_cmd);
void RemoveCmd_n(CmdPool *p_CmdPool, uint32_t cmd_count, Cmd **pp_cmd);
void AddRenderTarget(Renderer *pRenderer, const RenderTargetDesc *p_desc, RenderTarget **pp_render_target);
void RemoveRenderTarget(Renderer *pRenderer, RenderTarget *p_render_target);
void AddSampler(Renderer *pRenderer, const SamplerDesc *pDesc, Sampler **pp_sampler);
void RemoveSampler(Renderer *pRenderer, Sampler *p_sampler);
void AddShader(Renderer *pRenderer, const ShaderDesc *p_desc, Shader **p_shader_program);
void AddShaderBinary(Renderer *pRenderer, const BinaryShaderDesc *p_desc, Shader **p_shader_program);
void RemoveShader(Renderer *pRenderer, Shader *p_shader_program);
void AddRootSignature(Renderer *pRenderer, const RootSignatureDesc *pRootDesc, RootSignature **pp_root_signature);
void RemoveRootSignature(Renderer *pRenderer, RootSignature *pRootSignature);
void AddPipeline(Renderer *pRenderer, const GraphicsPipelineDesc *p_pipeline_settings, Pipeline **pp_pipeline);
void AddComputePipeline(Renderer *pRenderer, const ComputePipelineDesc *p_pipeline_settings, Pipeline **pp_pipeline);
void RemovePipeline(Renderer *pRenderer, Pipeline *p_pipeline);
void AddBlendState(Renderer *pRenderer, const BlendStateDesc *pDesc, BlendState **ppBlendState);
void RemoveBlendState(Renderer *pRenderer, BlendState *pBlendState);
void AddDepthState(Renderer *pRenderer, const DepthStateDesc *pDesc, DepthState **ppDepthState);
void RemoveDepthState(Renderer *pRenderer, DepthState *pDepthState);
void AddRasterizerState(Renderer *pRenderer, const RasterizerStateDesc *pDesc, RasterizerState **ppRasterizerState);
void RemoveRasterizerState(Renderer *pRenderer, RasterizerState *pRasterizerState);
void AddBuffer(Renderer *pRenderer, const BufferDesc *pDesc, Buffer **pp_buffer);
void RemoveBuffer(Renderer *pRenderer, Buffer *pBuffer);
void AddTexture(Renderer *pRenderer, const TextureDesc *pDesc, Texture **ppTexture);
void RemoveTexture(Renderer *pRenderer, Texture *pTexture);

void BeginCmd(Cmd *p_cmd);
void EndCmd(Cmd *p_cmd);
void CmdBindRenderTargets(Cmd *p_cmd, uint32_t render_target_count,
                          RenderTarget **pp_render_targets,
                          RenderTarget *p_depth_stencil,
                          const LoadActionsDesc *loadActions,
                          uint32_t *pColorArraySlices,
                          uint32_t *pColorMipSlices,
                          uint32_t depthArraySlice,
                          uint32_t depthMipSlice);
void CmdSetViewport(Cmd *p_cmd,
                    float x,
                    float y,
                    float width,
                    float height,
                    float min_depth,
                    float max_depth);
void CmdSetScissor(Cmd *p_cmd, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void CmdBindPipeline(Cmd *p_cmd, Pipeline *p_pipeline);
void CmdBindDescriptors(Cmd *pCmd,
                        RootSignature *pRootSignature,
                        uint32_t numDescriptors,
                        DescriptorData *pDescParams);
void CmdBindIndexBuffer(Cmd *p_cmd, Buffer *p_buffer, uint64_t offset);
void CmdBindVertexBuffer(Cmd *p_cmd,
                         uint32_t buffer_count,
                         Buffer **pp_buffers,
                         uint64_t *pOffsets);
void CmdDraw(Cmd *p_cmd, uint32_t vertex_count, uint32_t first_vertex);
void CmdDrawInstanced(Cmd *pCmd,
                      uint32_t vertexCount,
                      uint32_t firstVertex,
                      uint32_t instanceCount,
                      uint32_t firstInstance);
void CmdDrawIndexed(Cmd *p_cmd,
                    uint32_t index_count,
                    uint32_t first_index,
                    uint32_t first_vertex);
void CmdDrawIndexedInstanced(Cmd *pCmd,
                             uint32_t indexCount,
                             uint32_t firstIndex,
                             uint32_t instanceCount,
                             uint32_t firstVertex,
                             uint32_t firstInstance);
void CmdDispatch(Cmd *p_cmd,
                 uint32_t group_count_x,
                 uint32_t group_count_y,
                 uint32_t group_count_z);
void CmdResourceBarrier(Cmd *p_cmd,
                        uint32_t buffer_barrier_count,
                        BufferBarrier *p_buffer_barriers,
                        uint32_t texture_barrier_count,
                        TextureBarrier *p_texture_barriers,
                        bool batch);
void CmdSynchronizeResources(Cmd *p_cmd,
                             uint32_t buffer_count,
                             Buffer **p_buffers,
                             uint32_t texture_count,
                             Texture **p_textures,
                             bool batch);
void CmdFlushBarriers(Cmd *p_cmd);
void QueueSubmit(Queue *p_queue,
                 uint32_t cmd_count,
                 Cmd **pp_cmds,
                 Fence *pFence,
                 uint32_t wait_semaphore_count,
                 Semaphore **pp_wait_semaphores,
                 uint32_t signal_semaphore_count,
                 Semaphore **pp_signal_semaphores);
void WaitQueueIdle(Queue *p_queue);
void GetFenceStatus(Renderer *pRenderer,
                    Fence *p_fence,
                    FenceStatus *p_fence_status);
void WaitForFences(Renderer *pRenderer,
                   uint32_t fence_count,
                   Fence **pp_fences);

void AddIndirectCommandSignature(Renderer *pRenderer,
                                 const CommandSignatureDesc *p_desc,
                                 CommandSignature **ppCommandSignature);
void RemoveIndirectCommandSignature(Renderer *pRenderer,
                                    CommandSignature *pCommandSignature);
void CmdExecuteIndirect(Cmd *pCmd,
                        CommandSignature *pCommandSignature,
                        uint32_t maxCommandCount,
                        Buffer *pIndirectBuffer,
                        uint64_t bufferOffset,
                        Buffer *pCounterBuffer,
                        uint64_t counterBufferOffset);
bool IsQuerySupported(Renderer *pRenderer);
void GetTimestampFrequency(Queue *pQueue, double *pFrequency);
void AddQueryHeap(Renderer *pRenderer,
                  const QueryHeapDesc *pDesc,
                  QueryHeap **ppQueryHeap);
void RemoveQueryHeap(Renderer *pRenderer, QueryHeap *pQueryHeap);
void CmdBeginQuery(Cmd *pCmd, QueryHeap *pQueryHeap, QueryDesc *pQuery);
void CmdEndQuery(Cmd *pCmd, QueryHeap *pQueryHeap, QueryDesc *pQuery);
void CmdResolveQuery(Cmd *pCmd,
                     QueryHeap *pQueryHeap,
                     Buffer *pReadbackBuffer,
                     uint32_t startQuery,
                     uint32_t queryCount);
void CalculateMemoryStats(Renderer *pRenderer, char **stats);
void FreeMemoryStats(Renderer *pRenderer, char *stats);
void CmdBeginDebugMarker(Cmd *pCmd, float r, float g, float b, const char *pName);
void CmdEndDebugMarker(Cmd *pCmd);
void CmdAddDebugMarker(Cmd *pCmd, float r, float g, float b, const char *pName);
void SetBufferName(Renderer *pRenderer, Buffer *pBuffer, const char *pName);
void SetTextureName(Renderer *pRenderer, Texture *pTexture, const char *pName);
Image_Format GetRecommendedSwapchainFormat(bool hintHDR);
void MapBuffer(Renderer *pRenderer, Buffer *pBuffer, ReadRange *pRange);
void UnmapBuffer(Renderer *pRenderer, Buffer *pBuffer);
void CmdUpdateBuffer(Cmd *p_cmd, uint64_t srcOffset,
                     uint64_t dstOffset,
                     uint64_t size,
                     Buffer *p_src_buffer,
                     Buffer *p_buffer);
void CmdUpdateSubresources(Cmd *pCmd,
                           uint32_t startSubresource,
                           uint32_t numSubresources,
                           SubresourceDataDesc *pSubresources,
                           Buffer *pIntermediate,
                           uint64_t intermediateOffset,
                           Texture *pTexture);
} // end namespace TheForge

#endif //WYRD_THEFORGE_RENDERER_HPP
