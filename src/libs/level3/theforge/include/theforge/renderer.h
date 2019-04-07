#pragma once
#ifndef WYRD_THEFORGE_RENDERER_H
#define WYRD_THEFORGE_RENDERER_H

#include "core/core.h"
#include "theforge/renderer_enums.h"
#include "theforge/renderer_descs.h"
#include "theforge/renderer_structs.h"

// API functions
// allocates memory and initializes the renderer -> returns pRenderer
//
EXTERN_C void TheForge_InitRenderer(const char *app_name,
                                    const TheForge_RendererDesc *p_settings,
                                    TheForge_Renderer **ppRenderer);
EXTERN_C void TheForge_RemoveRenderer(TheForge_Renderer *pRenderer);
EXTERN_C void TheForge_AddFence(TheForge_Renderer *pRenderer, TheForge_Fence **pp_fence);
EXTERN_C void TheForge_RemoveFence(TheForge_Renderer *pRenderer, TheForge_Fence *p_fence);
EXTERN_C void TheForge_AddSemaphore(TheForge_Renderer *pRenderer, TheForge_Semaphore **pp_semaphore);
EXTERN_C void TheForge_RemoveSemaphore(TheForge_Renderer *pRenderer, TheForge_Semaphore *p_semaphore);
EXTERN_C void TheForge_AddQueue(TheForge_Renderer *pRenderer, TheForge_QueueDesc *pQDesc, TheForge_Queue **ppQueue);
EXTERN_C void TheForge_RemoveQueue(TheForge_Renderer *pRenderer, TheForge_Queue *pQueue);
EXTERN_C void TheForge_AddRootSignature(TheForge_Renderer *pRenderer,
                                        const TheForge_RootSignatureDesc *pRootDesc,
                                        TheForge_RootSignature **pp_root_signature);
EXTERN_C void TheForge_RemoveRootSignature(TheForge_Renderer *pRenderer, TheForge_RootSignature *pRootSignature);
EXTERN_C void TheForge_AddBlendState(TheForge_Renderer *pRenderer,
                                     const TheForge_BlendStateDesc *pDesc,
                                     TheForge_BlendState **ppBlendState);
EXTERN_C void TheForge_RemoveBlendState(TheForge_Renderer *pRenderer, TheForge_BlendState *pBlendState);
EXTERN_C void TheForge_AddDepthState(TheForge_Renderer *pRenderer,
                                     const TheForge_DepthStateDesc *pDesc,
                                     TheForge_DepthState **ppDepthState);
EXTERN_C void TheForge_RemoveDepthState(TheForge_Renderer *pRenderer, TheForge_DepthState *pDepthState);
EXTERN_C void TheForge_AddRasterizerState(TheForge_Renderer *pRenderer,
                                          const TheForge_RasterizerStateDesc *pDesc,
                                          TheForge_RasterizerState **ppRasterizerState);
EXTERN_C void TheForge_RemoveRasterizerState(TheForge_Renderer *pRenderer, TheForge_RasterizerState *pRasterizerState);
EXTERN_C void TheForge_AddRenderTarget(TheForge_Renderer *pRenderer,
                                       const TheForge_RenderTargetDesc *p_desc,
                                       TheForge_RenderTarget **pp_render_target);
EXTERN_C void TheForge_RemoveRenderTarget(TheForge_Renderer *pRenderer, TheForge_RenderTarget *p_render_target);
EXTERN_C void TheForge_AddSampler(TheForge_Renderer *pRenderer,
                                  const TheForge_SamplerDesc *pDesc,
                                  TheForge_Sampler **pp_sampler);
EXTERN_C void TheForge_RemoveSampler(TheForge_Renderer *pRenderer, TheForge_Sampler *p_sampler);
EXTERN_C void TheForge_AddSwapChain(TheForge_Renderer *pRenderer,
                                    const TheForge_SwapChainDesc *p_desc,
                                    TheForge_SwapChain **pp_swap_chain);
EXTERN_C void TheForge_RemoveSwapChain(TheForge_Renderer *pRenderer, TheForge_SwapChain *p_swap_chain);
EXTERN_C void TheForge_AddBuffer(TheForge_Renderer *pRenderer,
                                 const TheForge_BufferDesc *pDesc,
                                 TheForge_Buffer **pp_buffer);
EXTERN_C void TheForge_RemoveBuffer(TheForge_Renderer *pRenderer, TheForge_Buffer *pBuffer);
EXTERN_C void TheForge_AddTexture(TheForge_Renderer *pRenderer,
                                  const TheForge_TextureDesc *pDesc,
                                  TheForge_Texture **ppTexture);
EXTERN_C void TheForge_RemoveTexture(TheForge_Renderer *pRenderer, TheForge_Texture *pTexture);

// command pool functions
EXTERN_C void TheForge_AddCmdPool(TheForge_Renderer *pRenderer,
                                  TheForge_Queue *p_queue,
                                  bool transient,
                                  TheForge_CmdPool **pp_CmdPool);
EXTERN_C void TheForge_RemoveCmdPool(TheForge_Renderer *pRenderer, TheForge_CmdPool *p_CmdPool);
EXTERN_C void TheForge_AddCmd(TheForge_CmdPool *p_CmdPool, bool secondary, TheForge_Cmd **pp_cmd);
EXTERN_C void TheForge_RemoveCmd(TheForge_CmdPool *p_CmdPool, TheForge_Cmd *p_cmd);
EXTERN_C void TheForge_AddCmd_n(TheForge_CmdPool *p_CmdPool,
                                bool secondary,
                                uint32_t cmd_count,
                                TheForge_Cmd ***ppp_cmd);
EXTERN_C void TheForge_RemoveCmd_n(TheForge_CmdPool *p_CmdPool, uint32_t cmd_count, TheForge_Cmd **pp_cmd);

// shader functions
EXTERN_C void TheForge_AddShader(TheForge_Renderer *pRenderer,
                                 const TheForge_ShaderDesc *p_desc,
                                 TheForge_Shader **p_shader_program);
EXTERN_C void TheForge_AddShaderBinary(TheForge_Renderer *pRenderer,
                                       const TheForge_BinaryShaderDesc *p_desc,
                                       TheForge_Shader **p_shader_program);
EXTERN_C void TheForge_RemoveShader(TheForge_Renderer *pRenderer, TheForge_Shader *p_shader_program);

// pipeline functions
EXTERN_C void TheForge_AddPipeline(TheForge_Renderer *pRenderer,
                                   const TheForge_GraphicsPipelineDesc *p_pipeline_settings,
                                   TheForge_Pipeline **pp_pipeline);
EXTERN_C void TheForge_AddComputePipeline(TheForge_Renderer *pRenderer,
                                          const TheForge_ComputePipelineDesc *p_pipeline_settings,
                                          TheForge_Pipeline **p_pipeline);
EXTERN_C void TheForge_RemovePipeline(TheForge_Renderer *pRenderer, TheForge_Pipeline *p_pipeline);

// command buffer functions
EXTERN_C void TheForge_BeginCmd(TheForge_Cmd *p_cmd);
EXTERN_C void TheForge_EndCmd(TheForge_Cmd *p_cmd);
EXTERN_C void TheForge_CmdBindRenderTargets(TheForge_Cmd *p_cmd,
                                            uint32_t render_target_count,
                                            TheForge_RenderTarget **pp_render_targets,
                                            TheForge_RenderTarget *p_depth_stencil,
                                            const TheForge_LoadActionsDesc *loadActions,
                                            uint32_t *pColorArraySlices,
                                            uint32_t *pColorMipSlices,
                                            uint32_t depthArraySlice,
                                            uint32_t depthMipSlice);
EXTERN_C void TheForge_CmdSetViewport(TheForge_Cmd *p_cmd,
                                      float x,
                                      float y,
                                      float width,
                                      float height,
                                      float min_depth,
                                      float max_depth);
EXTERN_C void TheForge_CmdSetScissor(TheForge_Cmd *p_cmd, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
EXTERN_C void TheForge_CmdBindPipeline(TheForge_Cmd *p_cmd, TheForge_Pipeline *p_pipeline);
EXTERN_C void TheForge_CmdBindDescriptors(TheForge_Cmd *pCmd,
                                          TheForge_RootSignature *pRootSignature,
                                          uint32_t numDescriptors,
                                          TheForge_DescriptorData *pDescParams);
EXTERN_C void TheForge_CmdBindIndexBuffer(TheForge_Cmd *p_cmd, TheForge_Buffer *p_buffer, uint64_t offset);
EXTERN_C void TheForge_CmdBindVertexBuffer(TheForge_Cmd *p_cmd,
                                           uint32_t buffer_count,
                                           TheForge_Buffer **pp_buffers,
                                           uint64_t *pOffsets);
EXTERN_C void TheForge_CmdDraw(TheForge_Cmd *p_cmd, uint32_t vertex_count, uint32_t first_vertex);
EXTERN_C void TheForge_CmdDrawInstanced(TheForge_Cmd *pCmd,
                                        uint32_t vertexCount,
                                        uint32_t firstVertex,
                                        uint32_t instanceCount,
                                        uint32_t firstInstance);
EXTERN_C void TheForge_CmdDrawIndexed(TheForge_Cmd *p_cmd,
                                      uint32_t index_count,
                                      uint32_t first_index,
                                      uint32_t first_vertex);
EXTERN_C void TheForge_CmdDrawIndexedInstanced(TheForge_Cmd *pCmd,
                                               uint32_t indexCount,
                                               uint32_t firstIndex,
                                               uint32_t instanceCount,
                                               uint32_t firstVertex,
                                               uint32_t firstInstance);
EXTERN_C void TheForge_CmdDispatch(TheForge_Cmd *p_cmd,
                                   uint32_t group_count_x,
                                   uint32_t group_count_y,
                                   uint32_t group_count_z);

// Transition Commands
EXTERN_C void TheForge_CmdResourceBarrier(TheForge_Cmd *p_cmd,
                                          uint32_t buffer_barrier_count,
                                          TheForge_BufferBarrier *p_buffer_barriers,
                                          uint32_t texture_barrier_count,
                                          TheForge_TextureBarrier *p_texture_barriers,
                                          bool batch);
EXTERN_C void TheForge_CmdSynchronizeResources(TheForge_Cmd *p_cmd,
                                               uint32_t buffer_count,
                                               TheForge_Buffer **p_buffers,
                                               uint32_t texture_count,
                                               TheForge_Texture **p_textures,
                                               bool batch);
/// Flushes all the batched transitions requested in cmdResourceBarrier
EXTERN_C void TheForge_CmdFlushBarriers(TheForge_Cmd *p_cmd);

// queue/fence/swapchain functions
EXTERN_C void TheForge_AcquireNextImage(TheForge_Renderer *pRenderer,
                                        TheForge_SwapChain *p_swap_chain,
                                        TheForge_Semaphore *p_signal_semaphore,
                                        TheForge_Fence *p_fence,
                                        uint32_t *p_image_index);
EXTERN_C void TheForge_QueueSubmit(TheForge_Queue *p_queue,
                                   uint32_t cmd_count,
                                   TheForge_Cmd **pp_cmds,
                                   TheForge_Fence *pFence,
                                   uint32_t wait_semaphore_count,
                                   TheForge_Semaphore **pp_wait_semaphores,
                                   uint32_t signal_semaphore_count,
                                   TheForge_Semaphore **pp_signal_semaphores);
EXTERN_C void TheForge_QueuePresent(TheForge_Queue *p_queue,
                                    TheForge_SwapChain *p_swap_chain,
                                    uint32_t swap_chain_image_index,
                                    uint32_t wait_semaphore_count,
                                    TheForge_Semaphore **pp_wait_semaphores);
EXTERN_C void TheForge_WaitQueueIdle(TheForge_Queue *p_queue);

EXTERN_C void TheForge_GetFenceStatus(TheForge_Renderer *pRenderer,
                                      TheForge_Fence *p_fence,
                                      TheForge_FenceStatus *p_fence_status);
EXTERN_C void TheForge_WaitForFences(TheForge_Renderer *pRenderer,
                                     uint32_t fence_count,
                                     TheForge_Fence **pp_fences);

EXTERN_C void TheForge_ToggleVSync(TheForge_Renderer *pRenderer, TheForge_SwapChain **ppSwapchain);

// image related functions
EXTERN_C bool TheForge_IsImageFormatSupported(Image_Format format);

//Returns the recommended format for the swapchain.
//If true is passed for the hintHDR parameter, it will return an HDR format IF the platform supports it
//If false is passed or the platform does not support HDR a non HDR format is returned.
EXTERN_C Image_Format TheForge_GetRecommendedSwapchainFormat(bool hintHDR);

//indirect Draw functions
EXTERN_C void TheForge_AddIndirectCommandSignature(TheForge_Renderer *pRenderer,
                                                   const TheForge_CommandSignatureDesc *p_desc,
                                                   TheForge_CommandSignature **ppCommandSignature);
EXTERN_C void TheForge_RemoveIndirectCommandSignature(TheForge_Renderer *pRenderer,
                                                      TheForge_CommandSignature *pCommandSignature);
EXTERN_C void TheForge_CmdExecuteIndirect(TheForge_Cmd *pCmd,
                                          TheForge_CommandSignature *pCommandSignature,
                                          uint32_t maxCommandCount,
                                          TheForge_Buffer *pIndirectBuffer,
                                          uint64_t bufferOffset,
                                          TheForge_Buffer *pCounterBuffer,
                                          uint64_t counterBufferOffset);

// GPU Query Interface
EXTERN_C bool TheForge_IsQuerySupported(TheForge_Renderer *pRenderer);

EXTERN_C void TheForge_GetTimestampFrequency(TheForge_Queue *pQueue, double *pFrequency);
EXTERN_C void TheForge_AddQueryHeap(TheForge_Renderer *pRenderer,
                                    const TheForge_QueryHeapDesc *pDesc,
                                    TheForge_QueryHeap **ppQueryHeap);
EXTERN_C void TheForge_RemoveQueryHeap(TheForge_Renderer *pRenderer, TheForge_QueryHeap *pQueryHeap);
EXTERN_C void TheForge_CmdBeginQuery(TheForge_Cmd *pCmd, TheForge_QueryHeap *pQueryHeap, TheForge_QueryDesc *pQuery);
EXTERN_C void TheForge_CmdEndQuery(TheForge_Cmd *pCmd, TheForge_QueryHeap *pQueryHeap, TheForge_QueryDesc *pQuery);
EXTERN_C void TheForge_CmdResolveQuery(TheForge_Cmd *pCmd,
                                       TheForge_QueryHeap *pQueryHeap,
                                       TheForge_Buffer *pReadbackBuffer,
                                       uint32_t startQuery,
                                       uint32_t queryCount);

// Stats Info Interface
EXTERN_C void TheForge_CalculateMemoryStats(TheForge_Renderer *pRenderer, char **stats);
EXTERN_C void TheForge_FreeMemoryStats(TheForge_Renderer *pRenderer, char *stats);

// Debug Marker Interface
EXTERN_C void TheForge_CmdBeginDebugMarker(TheForge_Cmd *pCmd, float r, float g, float b, const char *pName);
EXTERN_C void TheForge_CmdEndDebugMarker(TheForge_Cmd *pCmd);
EXTERN_C void TheForge_CmdAddDebugMarker(TheForge_Cmd *pCmd, float r, float g, float b, const char *pName);

EXTERN_C void TheForge_MapBuffer(TheForge_Renderer *pRenderer, TheForge_Buffer *pBuffer, TheForge_ReadRange *pRange);
EXTERN_C void TheForge_UnmapBuffer(TheForge_Renderer *pRenderer, TheForge_Buffer *pBuffer);
EXTERN_C void TheForge_CmdUpdateBuffer(TheForge_Cmd *p_cmd,
                                       uint64_t srcOffset,
                                       uint64_t dstOffset,
                                       uint64_t size,
                                       TheForge_Buffer *p_src_buffer,
                                       TheForge_Buffer *p_buffer);
EXTERN_C void TheForge_CmdUpdateSubresources(TheForge_Cmd *pCmd,
                                    uint32_t startSubresource,
                                    uint32_t numSubresources,
                                    TheForge_SubresourceDataDesc *pSubresources,
                                    TheForge_Buffer *pIntermediate,
                                    uint64_t intermediateOffset,
                                    TheForge_Texture *pTexture);

// Resource Debug Naming Interface
EXTERN_C void TheForge_SetBufferName(TheForge_Renderer *pRenderer, TheForge_Buffer *pBuffer, const char *pName);
EXTERN_C void TheForge_SetTextureName(TheForge_Renderer *pRenderer, TheForge_Texture *pTexture, const char *pName);

#endif //WYRD_THEFORGE_RENDERER_H
