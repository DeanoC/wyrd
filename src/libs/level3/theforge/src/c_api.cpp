#include "core/core.h"
#include "theforge/renderer.hpp"
#include "theforge/shader_reflection.hpp"
#include "image/image.h"

// I usually don't global namespace things
using namespace TheForge;

EXTERN_C void TheForge_InitRenderer(const char *app_name,
                                    const RendererDesc *p_settings,
                                    Renderer **ppRenderer) {
  InitRenderer(app_name, p_settings, ppRenderer);
}
EXTERN_C void TheForge_RemoveRenderer(Renderer *pRenderer) {
  RemoveRenderer(pRenderer);
}
EXTERN_C void TheForge_AddFence(Renderer *pRenderer, Fence **pp_fence) {
  AddFence(pRenderer, pp_fence);
}
EXTERN_C void TheForge_RemoveFence(Renderer *pRenderer, Fence *p_fence) {
  RemoveFence(pRenderer, p_fence);
}
EXTERN_C void TheForge_AddSemaphore(Renderer *pRenderer, Semaphore **pp_semaphore) {
  AddSemaphore(pRenderer,
               pp_semaphore);
}
EXTERN_C void TheForge_RemoveSemaphore(Renderer *pRenderer, Semaphore *p_semaphore) {
  RemoveSemaphore(pRenderer, p_semaphore);
}
EXTERN_C void TheForge_AddQueue(Renderer *pRenderer, QueueDesc *pQDesc, Queue **ppQueue) {
  AddQueue(pRenderer,
           pQDesc,
           ppQueue);
}
EXTERN_C void TheForge_RemoveQueue(Renderer *pRenderer, Queue *pQueue) {
  RemoveQueue(pRenderer, pQueue);
}

EXTERN_C void TheForge_AddCmdPool(Renderer *pRenderer,
                                  Queue *p_queue,
                                  bool transient,
                                  CmdPool **pp_CmdPool) {
  AddCmdPool(pRenderer, p_queue, transient, pp_CmdPool);
}
EXTERN_C void TheForge_RemoveCmdPool(Renderer *pRenderer, CmdPool *p_CmdPool) {
  RemoveCmdPool(pRenderer, p_CmdPool);
}
EXTERN_C void TheForge_AddCmd(CmdPool *p_CmdPool, bool secondary, Cmd **pp_cmd) {
  AddCmd(p_CmdPool,
         secondary,
         pp_cmd);
}
EXTERN_C void TheForge_RemoveCmd(CmdPool *p_CmdPool, Cmd *p_cmd) {
  RemoveCmd(p_CmdPool, p_cmd);
}
EXTERN_C void TheForge_AddCmd_n(CmdPool *p_CmdPool, bool secondary, uint32_t cmd_count, Cmd ***ppp_cmd) {
  AddCmd_n(p_CmdPool,
           secondary,
           cmd_count,
           ppp_cmd);
}
EXTERN_C void TheForge_RemoveCmd_n(CmdPool *p_CmdPool, uint32_t cmd_count, Cmd **pp_cmd) {
  RemoveCmd_n(p_CmdPool,
              cmd_count,
              pp_cmd);
}
EXTERN_C void TheForge_AddRenderTarget(Renderer *pRenderer,
                                       const RenderTargetDesc *p_desc,
                                       RenderTarget **pp_render_target) {
  AddRenderTarget(pRenderer,
                  p_desc,
                  pp_render_target);
}
EXTERN_C void TheForge_RemoveRenderTarget(Renderer *pRenderer, RenderTarget *p_render_target) {
  RemoveRenderTarget(pRenderer,
                     p_render_target);
}
EXTERN_C void TheForge_AddSampler(Renderer *pRenderer, const SamplerDesc *pDesc, Sampler **pp_sampler) {
  AddSampler(pRenderer,
             pDesc,
             pp_sampler);
}
EXTERN_C void TheForge_RemoveSampler(Renderer *pRenderer, Sampler *p_sampler) {
  RemoveSampler(pRenderer, p_sampler);
}
EXTERN_C void TheForge_AddShader(Renderer *pRenderer, const ShaderDesc *p_desc, Shader **p_shader_program) {
  AddShader(pRenderer,
            p_desc,
            p_shader_program);
}
EXTERN_C void TheForge_AddShaderBinary(Renderer *pRenderer,
                                       const BinaryShaderDesc *p_desc,
                                       Shader **p_shader_program) {
  AddShaderBinary(pRenderer,
                  p_desc,
                  p_shader_program);
}
EXTERN_C void TheForge_RemoveShader(Renderer *pRenderer, Shader *p_shader_program) {
  RemoveShader(pRenderer,
               p_shader_program);
}
EXTERN_C void TheForge_AddRootSignature(Renderer *pRenderer,
                                        const RootSignatureDesc *pRootDesc,
                                        RootSignature **pp_root_signature) {
  AddRootSignature(pRenderer,
                   pRootDesc,
                   pp_root_signature);
}
EXTERN_C void TheForge_RemoveRootSignature(Renderer *pRenderer, RootSignature *pRootSignature) {
  RemoveRootSignature(pRenderer,
                      pRootSignature);
}
EXTERN_C void TheForge_AddPipeline(Renderer *pRenderer,
                                   const GraphicsPipelineDesc *p_pipeline_settings,
                                   Pipeline **pp_pipeline) {
  AddPipeline(pRenderer, p_pipeline_settings, pp_pipeline);
}
EXTERN_C void TheForge_AddComputePipeline(Renderer *pRenderer,
                                          const ComputePipelineDesc *p_pipeline_settings,
                                          Pipeline **pp_pipeline) {
  AddComputePipeline(pRenderer,
                     p_pipeline_settings,
                     pp_pipeline);
}
EXTERN_C void TheForge_RemovePipeline(Renderer *pRenderer, Pipeline *p_pipeline) {
  RemovePipeline(pRenderer,
                 p_pipeline);
}
EXTERN_C void TheForge_AddBlendState(Renderer *pRenderer,
                                     const BlendStateDesc *pDesc,
                                     BlendState **ppBlendState) {
  AddBlendState(pRenderer, pDesc, ppBlendState);
}
EXTERN_C void TheForge_RemoveBlendState(Renderer *pRenderer, BlendState *pBlendState) {
  RemoveBlendState(pRenderer, pBlendState);
}
EXTERN_C void TheForge_AddDepthState(Renderer *pRenderer,
                                     const DepthStateDesc *pDesc,
                                     DepthState **ppDepthState) {
  AddDepthState(pRenderer, pDesc, ppDepthState);
}
EXTERN_C void TheForge_RemoveDepthState(Renderer *pRenderer, DepthState *pDepthState) {
  RemoveDepthState(pRenderer, pDepthState);
}
EXTERN_C void TheForge_AddRasterizerState(Renderer *pRenderer,
                                          const RasterizerStateDesc *pDesc,
                                          RasterizerState **ppRasterizerState) {
  AddRasterizerState(pRenderer, pDesc,ppRasterizerState);
}
EXTERN_C void TheForge_RemoveRasterizerState(Renderer *pRenderer,
                                             RasterizerState *pRasterizerState) {
  RemoveRasterizerState(pRenderer, pRasterizerState);
}

EXTERN_C void TheForge_BeginCmd(Cmd *p_cmd) { BeginCmd(p_cmd); }

EXTERN_C void TheForge_EndCmd(Cmd *p_cmd) { EndCmd(p_cmd); }

EXTERN_C void TheForge_CmdBindRenderTargets(Cmd *p_cmd, uint32_t render_target_count,
                                            RenderTarget **pp_render_targets,
                                            RenderTarget *p_depth_stencil,
                                            const LoadActionsDesc *loadActions,
                                            uint32_t *pColorArraySlices,
                                            uint32_t *pColorMipSlices,
                                            uint32_t depthArraySlice,
                                            uint32_t depthMipSlice) {
  CmdBindRenderTargets(p_cmd,
                       render_target_count,
                       pp_render_targets,
                       p_depth_stencil,
                       loadActions,
                       pColorArraySlices,
                       pColorMipSlices,
                       depthArraySlice,
                       depthMipSlice);
}
EXTERN_C void TheForge_CmdSetViewport(Cmd *p_cmd,
                                      float x,
                                      float y,
                                      float width,
                                      float height,
                                      float min_depth,
                                      float max_depth) {
  CmdSetViewport(p_cmd,
                 x,
                 y,
                 width,
                 height,
                 min_depth,
                 max_depth);
}
EXTERN_C void TheForge_CmdSetScissor(Cmd *p_cmd,
                                     uint32_t x,
                                     uint32_t y,
                                     uint32_t width,
                                     uint32_t height) {
  CmdSetScissor(p_cmd, x, y, width, height);
}
EXTERN_C void TheForge_CmdBindPipeline(Cmd *p_cmd, TheForge_Pipeline *p_pipeline) {
  CmdBindPipeline(p_cmd, p_pipeline);
}
EXTERN_C void TheForge_CmdBindDescriptors(Cmd *pCmd,
                                          RootSignature *pRootSignature,
                                          uint32_t numDescriptors,
                                          DescriptorData *pDescParams) {
  CmdBindDescriptors(pCmd,
                     pRootSignature,
                     numDescriptors,
                     pDescParams);
}
EXTERN_C void TheForge_CmdBindIndexBuffer(Cmd *p_cmd, TheForge_Buffer *p_buffer, uint64_t offset) {
  CmdBindIndexBuffer(p_cmd, p_buffer, offset);
}
EXTERN_C void TheForge_CmdBindVertexBuffer(Cmd *p_cmd,
                                           uint32_t buffer_count,
                                           Buffer **pp_buffers,
                                           uint64_t *pOffsets) {
  CmdBindVertexBuffer(p_cmd,
                      buffer_count,
                      pp_buffers,
                      pOffsets);
}
EXTERN_C void TheForge_CmdDraw(Cmd *p_cmd, uint32_t vertex_count, uint32_t first_vertex) {
  CmdDraw(p_cmd, vertex_count, first_vertex);
}
EXTERN_C void TheForge_CmdDrawInstanced(Cmd *pCmd,
                                        uint32_t vertexCount,
                                        uint32_t firstVertex,
                                        uint32_t instanceCount,
                                        uint32_t firstInstance) {
  CmdDrawInstanced(pCmd,
                   vertexCount,
                   firstVertex,
                   instanceCount,
                   firstInstance);
}
EXTERN_C void TheForge_CmdDrawIndexed(Cmd *p_cmd,
                                      uint32_t index_count,
                                      uint32_t first_index,
                                      uint32_t first_vertex) {
  CmdDrawIndexed(p_cmd,
                 index_count,
                 first_index,
                 first_vertex);
}
EXTERN_C void TheForge_CmdDrawIndexedInstanced(Cmd *pCmd,
                                               uint32_t indexCount,
                                               uint32_t firstIndex,
                                               uint32_t instanceCount,
                                               uint32_t firstVertex,
                                               uint32_t firstInstance) {
  CmdDrawIndexedInstanced(pCmd,
                          indexCount,
                          firstIndex,
                          instanceCount,
                          firstVertex,
                          firstInstance);
}
EXTERN_C void TheForge_CmdDispatch(Cmd *p_cmd,
                                   uint32_t group_count_x,
                                   uint32_t group_count_y,
                                   uint32_t group_count_z) {
  CmdDispatch(p_cmd,
              group_count_x,
              group_count_y,
              group_count_z);
}
EXTERN_C void TheForge_CmdResourceBarrier(Cmd *p_cmd,
                                          uint32_t buffer_barrier_count,
                                          BufferBarrier *p_buffer_barriers,
                                          uint32_t texture_barrier_count,
                                          TextureBarrier *p_texture_barriers,
                                          bool batch) {
  CmdResourceBarrier(p_cmd,
                     buffer_barrier_count,
                     p_buffer_barriers,
                     texture_barrier_count,
                     p_texture_barriers,
                     batch);
}
EXTERN_C void TheForge_CmdSynchronizeResources(Cmd *p_cmd,
                                               uint32_t buffer_count,
                                               Buffer **p_buffers,
                                               uint32_t texture_count,
                                               Texture **p_textures,
                                               bool batch) {
  CmdSynchronizeResources(p_cmd,
                          buffer_count,
                          p_buffers,
                          texture_count,
                          p_textures,
                          batch);
}
EXTERN_C void TheForge_CmdFlushBarriers(Cmd *p_cmd) {
  CmdFlushBarriers(p_cmd);
}
EXTERN_C void TheForge_QueueSubmit(Queue *p_queue,
                                   uint32_t cmd_count,
                                   Cmd **pp_cmds,
                                   Fence *pFence,
                                   uint32_t wait_semaphore_count,
                                   Semaphore **pp_wait_semaphores,
                                   uint32_t signal_semaphore_count,
                                   Semaphore **pp_signal_semaphores) {
  QueueSubmit(p_queue,
              cmd_count,
              pp_cmds,
              pFence,
              wait_semaphore_count,
              pp_wait_semaphores,
              signal_semaphore_count,
              pp_signal_semaphores);
}
EXTERN_C void TheForge_WaitQueueIdle(Queue *p_queue) {
  WaitQueueIdle(p_queue);
}
EXTERN_C void TheForge_GetFenceStatus(Renderer *pRenderer,
                                      Fence *p_fence,
                                      FenceStatus *p_fence_status) {
  GetFenceStatus(pRenderer,
                 p_fence,
                 p_fence_status);
}
EXTERN_C void TheForge_WaitForFences(Renderer *pRenderer,
                                     uint32_t fence_count,
                                     Fence **pp_fences) {
  WaitForFences(pRenderer, fence_count, pp_fences);
}
EXTERN_C void TheForge_AddIndirectCommandSignature(Renderer *pRenderer,
                                                   const CommandSignatureDesc *p_desc,
                                                   CommandSignature **ppCommandSignature) {
  AddIndirectCommandSignature(pRenderer,
                              p_desc,
                              ppCommandSignature);
}
EXTERN_C void TheForge_RemoveIndirectCommandSignature(Renderer *pRenderer,
                                                      CommandSignature *pCommandSignature) {
  RemoveIndirectCommandSignature(pRenderer,
                                 pCommandSignature);
}
EXTERN_C void TheForge_CmdExecuteIndirect(Cmd *pCmd,
                                          CommandSignature *pCommandSignature,
                                          uint32_t maxCommandCount,
                                          Buffer *pIndirectBuffer,
                                          uint64_t bufferOffset,
                                          Buffer *pCounterBuffer,
                                          uint64_t counterBufferOffset) {
  CmdExecuteIndirect(pCmd,
                     pCommandSignature,
                     maxCommandCount,
                     pIndirectBuffer,
                     bufferOffset,
                     pCounterBuffer,
                     counterBufferOffset);
}
EXTERN_C bool TheForge_IsQuerySupported(TheForge_Renderer *pRenderer) {
  return IsQuerySupported(pRenderer);
}
EXTERN_C void TheForge_GetTimestampFrequency(Queue *pQueue, double *pFrequency) {
  GetTimestampFrequency(pQueue, pFrequency);
}
EXTERN_C void TheForge_AddQueryHeap(Renderer *pRenderer,
                                    const QueryHeapDesc *pDesc,
                                    QueryHeap **ppQueryHeap) {
  AddQueryHeap(pRenderer, pDesc, ppQueryHeap);
}
EXTERN_C void TheForge_RemoveQueryHeap(Renderer *pRenderer, QueryHeap *pQueryHeap) {
  RemoveQueryHeap(pRenderer, pQueryHeap);
}
EXTERN_C void TheForge_CmdBeginQuery(Cmd *pCmd, QueryHeap *pQueryHeap, QueryDesc *pQuery) {
  CmdBeginQuery(pCmd, pQueryHeap, pQuery);
}
EXTERN_C void TheForge_CmdEndQuery(Cmd *pCmd, QueryHeap *pQueryHeap, QueryDesc *pQuery) {
  CmdEndQuery(pCmd, pQueryHeap, pQuery);
}
EXTERN_C void TheForge_CmdResolveQuery(Cmd *pCmd,
                                       QueryHeap *pQueryHeap,
                                       Buffer *pReadbackBuffer,
                                       uint32_t startQuery,
                                       uint32_t queryCount) {
  CmdResolveQuery(pCmd,
                  pQueryHeap,
                  pReadbackBuffer,
                  startQuery,
                  queryCount);
}
EXTERN_C void TheForge_CalculateMemoryStats(Renderer *pRenderer, char **stats) {
  CalculateMemoryStats(pRenderer, stats);
}
EXTERN_C void TheForge_FreeMemoryStats(Renderer *pRenderer, char *stats) {
  FreeMemoryStats(pRenderer, stats);
}
EXTERN_C void TheForge_CmdBeginDebugMarker(Cmd *pCmd,
                                           float r,
                                           float g,
                                           float b,
                                           const char *pName) {
  CmdBeginDebugMarker(pCmd, r, g, b, pName);
}
EXTERN_C void TheForge_CmdEndDebugMarker(Cmd *pCmd) {
  CmdEndDebugMarker(pCmd);
}
EXTERN_C void TheForge_CmdAddDebugMarker(Cmd *pCmd, float r, float g, float b, const char *pName) {
  CmdAddDebugMarker(pCmd, r, g, b, pName);
}
EXTERN_C void TheForge_SetBufferName(Renderer *pRenderer, Buffer *pBuffer, const char *pName) {
  SetBufferName(pRenderer, pBuffer, pName);
}
EXTERN_C void TheForge_SetTextureName(Renderer *pRenderer, Texture *pTexture, const char *pName) {
  SetTextureName(pRenderer, pTexture, pName);
}
EXTERN_C void TheForge_AddBuffer(Renderer *pRenderer, const BufferDesc *pDesc, Buffer **pp_buffer) {
  AddBuffer(pRenderer, pDesc, pp_buffer);
}
EXTERN_C void TheForge_RemoveBuffer(Renderer *pRenderer, Buffer *pBuffer) {
  RemoveBuffer(pRenderer, pBuffer);
}
EXTERN_C void TheForge_AddTexture(Renderer *pRenderer, const TextureDesc *pDesc, Texture **ppTexture) {
  AddTexture(pRenderer, pDesc, ppTexture);
}
EXTERN_C void TheForge_RemoveTexture(Renderer *pRenderer, Texture *pTexture) {
  RemoveTexture(pRenderer, pTexture);
}
EXTERN_C void TheForge_DestroyShaderReflection(TheForge_ShaderReflection *pReflection) {
  DestroyShaderReflection(pReflection);
}
EXTERN_C void TheForge_CreatePipelineReflection(TheForge_ShaderReflection *pReflection,
                                                uint32_t stageCount,
                                                TheForge_PipelineReflection *pOutReflection) {
  CreatePipelineReflection(pReflection, stageCount, pOutReflection);
}
EXTERN_C void TheForge_DestroyPipelineReflection(TheForge_PipelineReflection *pReflection) {
  DestroyPipelineReflection(pReflection);
}

EXTERN_C Image_Format TheForge_GetRecommendedSwapchainFormat(bool hintHDR) {
  return GetRecommendedSwapchainFormat(hintHDR);
}

EXTERN_C void TheForge_MapBuffer(TheForge_Renderer *pRenderer, TheForge_Buffer *pBuffer, TheForge_ReadRange *pRange) {
  return MapBuffer(pRenderer, pBuffer, pRange);
}
EXTERN_C void TheForge_UnmapBuffer(TheForge_Renderer *pRenderer, TheForge_Buffer *pBuffer) {
  return UnmapBuffer(pRenderer, pBuffer);
}
EXTERN_C void TheForge_CmdUpdateBuffer(TheForge_Cmd *p_cmd,
                                       uint64_t srcOffset,
                                       uint64_t dstOffset,
                                       uint64_t size,
                                       TheForge_Buffer *p_src_buffer,
                                       TheForge_Buffer *p_buffer) {
  CmdUpdateBuffer(p_cmd, srcOffset, dstOffset, size, p_src_buffer, p_buffer);
}
EXTERN_C void TheForge_CmdUpdateSubresources(TheForge_Cmd *pCmd,
                                             uint32_t startSubresource,
                                             uint32_t numSubresources,
                                             TheForge_SubresourceDataDesc *pSubresources,
                                             TheForge_Buffer *pIntermediate,
                                             uint64_t intermediateOffset,
                                             TheForge_Texture *pTexture) {

  CmdUpdateSubresources(pCmd, startSubresource, numSubresources, pSubresources, pIntermediate, intermediateOffset, pTexture );
}

