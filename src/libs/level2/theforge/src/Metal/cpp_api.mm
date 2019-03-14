#include "core/core.h"
#include "theforge/renderer.hpp"
#include "theforge/renderer_descs.hpp"
#include "theforge/renderer_enums.hpp"
#include "theforge/renderer_structs.hpp"
#include "theforge/shader_reflection.hpp"
#include "renderer.hpp"

namespace TheForge {

void InitRenderer(const char *app_name, const RendererDesc *p_settings, Renderer **ppRenderer) {
  Metal::InitRenderer(app_name, p_settings, (Metal::Renderer **) ppRenderer);
}
void RemoveRenderer(Renderer *pRenderer) {
  Metal::RemoveRenderer((Metal::Renderer *) pRenderer);
}

void AddFence(Renderer *pRenderer, Fence **pp_fence) {
  Metal::AddFence((Metal::Renderer *) pRenderer, (Metal::Fence **) pp_fence);
}

void RemoveFence(Renderer *pRenderer, Fence *p_fence) {
  Metal::RemoveFence((Metal::Renderer *) pRenderer, (Metal::Fence *) p_fence);
}

void AddSemaphore(Renderer *pRenderer, Semaphore **pp_semaphore) {
  Metal::AddSemaphore((Metal::Renderer *) pRenderer, (Metal::Semaphore **) pp_semaphore);
}

void RemoveSemaphore(Renderer *pRenderer, Semaphore *p_semaphore) {
  Metal::RemoveSemaphore((Metal::Renderer *) pRenderer, (Metal::Semaphore *) p_semaphore);
}

void AddQueue(Renderer *pRenderer, QueueDesc *pQDesc, Queue **ppQueue) {
  Metal::AddQueue((Metal::Renderer *) pRenderer, pQDesc, (Metal::Queue **) ppQueue);
}

void RemoveQueue(Renderer *pRenderer, Queue *pQueue) {
  Metal::RemoveQueue((Metal::Renderer *) pRenderer, (Metal::Queue *) pQueue);
}

void AddSwapChain(Renderer *pRenderer, const SwapChainDesc *p_desc, SwapChain **pp_swap_chain) {
  Metal::AddSwapChain((Metal::Renderer *) pRenderer, p_desc, (Metal::SwapChain **) pp_swap_chain);
}

void RemoveSwapChain(Renderer *pRenderer, SwapChain *p_swap_chain) {
  Metal::RemoveSwapChain((Metal::Renderer *) pRenderer, (Metal::SwapChain *) p_swap_chain);
}

void RemoveRenderTarget(Renderer *pRenderer, RenderTarget *p_render_target) {
  Metal::RemoveRenderTarget((Metal::Renderer *) pRenderer, (Metal::RenderTarget *) p_render_target);
}

void AddSampler(Renderer *pRenderer, const SamplerDesc *pDesc, Sampler **pp_sampler) {
  Metal::AddSampler((Metal::Renderer *) pRenderer, pDesc, (Metal::Sampler **) pp_sampler);
}

void RemoveSampler(Renderer *pRenderer, Sampler *p_sampler) {
  Metal::RemoveSampler((Metal::Renderer *) pRenderer, (Metal::Sampler *) p_sampler);
}

void AddShader(Renderer *pRenderer, const ShaderDesc *p_desc, Shader **p_shader_program) {
  Metal::AddShader((Metal::Renderer *) pRenderer, p_desc, (Metal::Shader **) p_shader_program);
}

void AddShaderBinary(Renderer *pRenderer, const BinaryShaderDesc *p_desc, Shader **p_shader_program) {
  Metal::AddShaderBinary((Metal::Renderer *) pRenderer, p_desc, (Metal::Shader **) p_shader_program);
}

void RemoveShader(Renderer *pRenderer, Shader *p_shader_program) {
  Metal::RemoveShader((Metal::Renderer *) pRenderer, (Metal::Shader *) p_shader_program);
}

void AddRootSignature(Renderer *pRenderer, const RootSignatureDesc *pRootDesc, RootSignature **pp_root_signature) {
  Metal::AddRootSignature((Metal::Renderer *) pRenderer, pRootDesc, (Metal::RootSignature **) pp_root_signature);
}

void RemoveRootSignature(Renderer *pRenderer, RootSignature *pRootSignature) {
  Metal::RemoveRootSignature((Metal::Renderer *) pRenderer, (Metal::RootSignature *) pRootSignature);
}

void AddPipeline(Renderer *pRenderer, const GraphicsPipelineDesc *p_pipeline_settings, Pipeline **pp_pipeline) {
  Metal::AddPipeline((Metal::Renderer *) pRenderer, p_pipeline_settings, (Metal::Pipeline **) pp_pipeline);
}

void AddComputePipeline(Renderer *pRenderer, const ComputePipelineDesc *p_pipeline_settings, Pipeline **pp_pipeline) {
  Metal::AddComputePipeline((Metal::Renderer *) pRenderer, p_pipeline_settings, (Metal::Pipeline **) pp_pipeline);
}

void RemovePipeline(Renderer *pRenderer, Pipeline *p_pipeline) {
  Metal::RemovePipeline((Metal::Renderer *) pRenderer, (Metal::Pipeline *) p_pipeline);
}

void AddBlendState(Renderer *pRenderer, const BlendStateDesc *pDesc, BlendState **ppBlendState) {
  Metal::AddBlendState((Metal::Renderer *) pRenderer, pDesc, (Metal::BlendState **) ppBlendState);
}

void RemoveBlendState(Renderer *pRenderer, BlendState *pBlendState) {
  Metal::RemoveBlendState((Metal::Renderer *) pRenderer, (Metal::BlendState *) pBlendState);
}

void AddDepthState(Renderer *pRenderer, const DepthStateDesc *pDesc, DepthState **ppDepthState) {
  Metal::AddDepthState((Metal::Renderer *) pRenderer, pDesc, (Metal::DepthState **) ppDepthState);
}

void RemoveDepthState(Renderer *pRenderer, DepthState *pDepthState) {
  Metal::RemoveDepthState((Metal::Renderer *) pRenderer, (Metal::DepthState *) pDepthState);
}

void AddRasterizerState(Renderer *pRenderer, const RasterizerStateDesc *pDesc, RasterizerState **ppRasterizerState) {
  Metal::AddRasterizerState((Metal::Renderer *) pRenderer, pDesc, (Metal::RasterizerState **) ppRasterizerState);
}

void RemoveRasterizerState(Renderer *pRenderer, RasterizerState *pRasterizerState) {
  Metal::RemoveRasterizerState((Metal::Renderer *) pRenderer, (Metal::RasterizerState *) pRasterizerState);
}

void AddCmdPool(Renderer *pRenderer, Queue *p_queue, bool transient, CmdPool **pp_CmdPool) {
  Metal::AddCmdPool((Metal::Renderer *) pRenderer, (Metal::Queue *) p_queue, transient, (Metal::CmdPool **) pp_CmdPool);
}

void RemoveCmdPool(Renderer *pRenderer, CmdPool *p_CmdPool) {
  Metal::RemoveCmdPool((Metal::Renderer *) pRenderer, (Metal::CmdPool *) p_CmdPool);
}

void AddRenderTarget(Renderer *pRenderer, const RenderTargetDesc *p_desc, RenderTarget **pp_render_target) {
  Metal::AddRenderTarget((Metal::Renderer *) pRenderer, p_desc, (Metal::RenderTarget **) pp_render_target);
}

void AddBuffer(Renderer *pRenderer, const BufferDesc *pDesc, Buffer **pp_buffer) {
  Metal::AddBuffer((Metal::Renderer *) pRenderer, pDesc, (Metal::Buffer **) pp_buffer);
}

void RemoveBuffer(Renderer *pRenderer, Buffer *pBuffer) {
  Metal::RemoveBuffer((Metal::Renderer *) pRenderer, (Metal::Buffer *) pBuffer);
}

void AddTexture(Renderer *pRenderer, const TextureDesc *pDesc, Texture **ppTexture) {
  Metal::AddTexture((Metal::Renderer *) pRenderer, pDesc, (Metal::Texture **) ppTexture, false, false );
}

void RemoveTexture(Renderer *pRenderer, Texture *pTexture) {
  Metal::RemoveTexture((Metal::Renderer *) pRenderer, (Metal::Texture *) pTexture);
}

void CalculateMemoryStats(Renderer *pRenderer, char **stats) {
  Metal::CalculateMemoryStats((Metal::Renderer *) pRenderer, stats);
}

void FreeMemoryStats(Renderer *pRenderer, char *stats) {
  Metal::FreeMemoryStats((Metal::Renderer *) pRenderer, stats);
}

void QueueSubmit(Queue *p_queue,
                 uint32_t cmd_count,
                 Cmd **pp_cmds,
                 Fence *pFence,
                 uint32_t wait_semaphore_count,
                 Semaphore **pp_wait_semaphores,
                 uint32_t signal_semaphore_count,
                 Semaphore **pp_signal_semaphores) {
  Metal::QueueSubmit((Metal::Queue *) p_queue,
                     cmd_count,
                     (Metal::Cmd **) pp_cmds,
                     (Metal::Fence *) pFence,
                     wait_semaphore_count,
                     (Metal::Semaphore **) pp_wait_semaphores,
                     signal_semaphore_count,
                     (Metal::Semaphore **) pp_signal_semaphores);
}

void QueuePresent(Queue *p_queue,
                  SwapChain *p_swap_chain,
                  uint32_t swap_chain_image_index,
                  uint32_t wait_semaphore_count,
                  Semaphore **pp_wait_semaphores) {
  Metal::QueuePresent((Metal::Queue *) p_queue,
                      (Metal::SwapChain *) p_swap_chain,
                      swap_chain_image_index,
                      wait_semaphore_count,
                      (Metal::Semaphore **) pp_wait_semaphores);
}

void WaitQueueIdle(Queue *p_queue) {
  Metal::WaitQueueIdle((Metal::Queue *) p_queue);
}

void GetFenceStatus(Renderer *pRenderer,
                    Fence *p_fence,
                    FenceStatus *p_fence_status) {
  Metal::GetFenceStatus((Metal::Renderer *) pRenderer, (Metal::Fence *) p_fence, p_fence_status);
}

void WaitForFences(Renderer *pRenderer,
                   uint32_t fence_count,
                   Fence **pp_fences) {
  Metal::WaitForFences((Metal::Renderer *) pRenderer, fence_count, (Metal::Fence **) pp_fences);
}

void ToggleVSync(Renderer *pRenderer, SwapChain **ppSwapchain) {
  Metal::ToggleVSync((Metal::Renderer *) pRenderer, (Metal::SwapChain **) ppSwapchain);
}

void AddIndirectCommandSignature(Renderer *pRenderer,
                                 const CommandSignatureDesc *p_desc,
                                 CommandSignature **ppCommandSignature) {
  Metal::AddIndirectCommandSignature((Metal::Renderer *) pRenderer,
                                     p_desc,
                                     (Metal::CommandSignature **) ppCommandSignature);
}

void RemoveIndirectCommandSignature(Renderer *pRenderer,
                                    CommandSignature *pCommandSignature) {
  Metal::RemoveIndirectCommandSignature((Metal::Renderer *) pRenderer, (Metal::CommandSignature *) pCommandSignature);
}

bool IsQuerySupported(Renderer* pRenderer) {
  return false;
}

void GetTimestampFrequency(Queue *pQueue, double *pFrequency) {
  ASSERT(false);
}

void AddQueryHeap(Renderer *pRenderer,
                  const QueryHeapDesc *pDesc,
                  QueryHeap **ppQueryHeap) {
  ASSERT(false);
}

void RemoveQueryHeap(Renderer *pRenderer, QueryHeap *pQueryHeap) {
  ASSERT(false);
}

void CmdBeginQuery(Cmd *pCmd, QueryHeap *pQueryHeap, QueryDesc *pQuery) {
  ASSERT(false);
}

void CmdEndQuery(Cmd *pCmd, QueryHeap *pQueryHeap, QueryDesc *pQuery) {
  ASSERT(false);
}

void CmdResolveQuery(Cmd *pCmd,
                     QueryHeap *pQueryHeap,
                     Buffer *pReadbackBuffer,
                     uint32_t startQuery,
                     uint32_t queryCount) {
  ASSERT(false);
}

void AcquireNextImage(Renderer *pRenderer,
                      SwapChain *p_swap_chain,
                      Semaphore *p_signal_semaphore,
                      Fence *p_fence,
                      uint32_t *p_image_index) {
  Metal::AcquireNextImage((Metal::Renderer *) pRenderer,
                          (Metal::SwapChain *) p_swap_chain,
                          (Metal::Semaphore *) p_signal_semaphore,
                          (Metal::Fence *) p_fence,
                          p_image_index);
}

void SetBufferName(Renderer *pRenderer, Buffer *pBuffer, const char *pName) {
  // not supported on metal yet
}

void SetTextureName(Renderer *pRenderer, Texture *pTexture, const char *pName) {
  // not supported on metal yet
}

void AddCmd(CmdPool *p_CmdPool, bool secondary, Cmd **pp_cmd) {
  Metal::AddCmd((Metal::CmdPool *) p_CmdPool, secondary, (Metal::Cmd **) pp_cmd);
}

void RemoveCmd(CmdPool *p_CmdPool, Cmd *p_cmd) {
  Metal::RemoveCmd((Metal::CmdPool *) p_CmdPool, (Metal::Cmd *) p_cmd);
}

void AddCmd_n(CmdPool *p_CmdPool, bool secondary, uint32_t cmd_count, Cmd ***ppp_cmd) {
  Metal::AddCmd_n((Metal::CmdPool *) p_CmdPool, secondary, cmd_count, (Metal::Cmd ***) ppp_cmd);
}

void RemoveCmd_n(CmdPool *p_CmdPool, uint32_t cmd_count, Cmd **pp_cmd) {
  Metal::RemoveCmd_n((Metal::CmdPool *) p_CmdPool, cmd_count, (Metal::Cmd **) pp_cmd);
}

void BeginCmd(Cmd *p_cmd) {
  Metal::BeginCmd((Metal::Cmd *) p_cmd);
}

void EndCmd(Cmd *p_cmd) {
  Metal::EndCmd((Metal::Cmd *) p_cmd);
}

void CmdBindRenderTargets(Cmd *p_cmd, uint32_t render_target_count,
                          RenderTarget **pp_render_targets,
                          RenderTarget *p_depth_stencil,
                          const LoadActionsDesc *loadActions,
                          uint32_t *pColorArraySlices,
                          uint32_t *pColorMipSlices,
                          uint32_t depthArraySlice,
                          uint32_t depthMipSlice) {
  Metal::CmdBindRenderTargets((Metal::Cmd *) p_cmd,
                              render_target_count,
                              (Metal::RenderTarget **) pp_render_targets,
                              (Metal::RenderTarget *) p_depth_stencil,
                              loadActions,
                              pColorArraySlices,
                              pColorMipSlices,
                              depthArraySlice,
                              depthMipSlice);
}

void CmdSetViewport(Cmd *p_cmd,
                    float x,
                    float y,
                    float width,
                    float height,
                    float min_depth,
                    float max_depth) {
  Metal::CmdSetViewport((Metal::Cmd *) p_cmd, x, y, width, height, min_depth, max_depth);
}

void CmdSetScissor(Cmd *p_cmd, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
  Metal::CmdSetScissor((Metal::Cmd *) p_cmd, x, y, width, height);
}

void CmdBindPipeline(Cmd *p_cmd, Pipeline *p_pipeline) {
  Metal::CmdBindPipeline((Metal::Cmd *) p_cmd, (Metal::Pipeline *) p_pipeline);
}

void CmdBindDescriptors(Cmd *pCmd,
                        RootSignature *pRootSignature,
                        uint32_t numDescriptors,
                        DescriptorData *pDescParams) {
  Metal::CmdBindDescriptors((Metal::Cmd *) pCmd, (Metal::RootSignature *) pRootSignature, numDescriptors, pDescParams);
}

void CmdBindIndexBuffer(Cmd *p_cmd, Buffer *p_buffer, uint64_t offset) {
  Metal::CmdBindIndexBuffer((Metal::Cmd *) p_cmd, (Metal::Buffer *) p_buffer, offset);
}

void CmdBindVertexBuffer(Cmd *p_cmd,
                         uint32_t buffer_count,
                         Buffer **pp_buffers,
                         uint64_t *pOffsets) {
  Metal::CmdBindVertexBuffer((Metal::Cmd *) p_cmd, buffer_count, (Metal::Buffer **) pp_buffers, pOffsets);
}

void CmdDraw(Cmd *p_cmd, uint32_t vertex_count, uint32_t first_vertex) {
  Metal::CmdDraw((Metal::Cmd *) p_cmd, vertex_count, first_vertex);
}

void CmdDrawInstanced(Cmd *pCmd,
                      uint32_t vertexCount,
                      uint32_t firstVertex,
                      uint32_t instanceCount,
                      uint32_t firstInstance) {
  Metal::CmdDrawInstanced((Metal::Cmd *) pCmd, vertexCount, firstVertex, instanceCount, firstInstance);
}

void CmdDrawIndexed(Cmd *p_cmd,
                    uint32_t index_count,
                    uint32_t first_index,
                    uint32_t first_vertex) {
  Metal::CmdDrawIndexed((Metal::Cmd *) p_cmd, index_count, first_index, first_vertex);
}

void CmdDrawIndexedInstanced(Cmd *pCmd,
                             uint32_t indexCount,
                             uint32_t firstIndex,
                             uint32_t instanceCount,
                             uint32_t firstVertex,
                             uint32_t firstInstance) {
  Metal::CmdDrawIndexedInstanced((Metal::Cmd *) pCmd,
                                 indexCount,
                                 firstIndex,
                                 instanceCount,
                                 firstVertex,
                                 firstInstance);
}

void CmdDispatch(Cmd *p_cmd,
                 uint32_t group_count_x,
                 uint32_t group_count_y,
                 uint32_t group_count_z) {
  Metal::CmdDispatch((Metal::Cmd *) p_cmd, group_count_x, group_count_y, group_count_z);
}

void CmdResourceBarrier(Cmd *p_cmd,
                        uint32_t buffer_barrier_count,
                        BufferBarrier *p_buffer_barriers,
                        uint32_t texture_barrier_count,
                        TextureBarrier *p_texture_barriers,
                        bool batch) {
  Metal::CmdResourceBarrier((Metal::Cmd *) p_cmd,
                            buffer_barrier_count,
                            (Metal::BufferBarrier *) p_buffer_barriers,
                            texture_barrier_count,
                            (Metal::TextureBarrier *) p_texture_barriers,
                            batch);
}

void CmdSynchronizeResources(Cmd *p_cmd,
                             uint32_t buffer_count,
                             Buffer **pp_buffers,
                             uint32_t texture_count,
                             Texture **pp_textures,
                             bool batch) {
  Metal::CmdSynchronizeResources((Metal::Cmd *) p_cmd,
                                 buffer_count,
                                 (Metal::Buffer **) pp_buffers,
                                 texture_count,
                                 (Metal::Texture **) pp_textures,
                                 batch);
}

void CmdFlushBarriers(Cmd *p_cmd) {
  Metal::CmdFlushBarriers((Metal::Cmd *) p_cmd);
}

void CmdExecuteIndirect(Cmd *pCmd,
                        CommandSignature *pCommandSignature,
                        uint32_t maxCommandCount,
                        Buffer *pIndirectBuffer,
                        uint64_t bufferOffset,
                        Buffer *pCounterBuffer,
                        uint64_t counterBufferOffset) {
  Metal::CmdExecuteIndirect((Metal::Cmd *) pCmd,
                            (Metal::CommandSignature *) pCommandSignature,
                            maxCommandCount,
                            (Metal::Buffer *) pIndirectBuffer,
                            bufferOffset,
                            (Metal::Buffer *) pCounterBuffer,
                            counterBufferOffset);
}

void CmdBeginDebugMarker(Cmd *pCmd, float r, float g, float b, const char *pName) {
  Metal::CmdBeginDebugMarker((Metal::Cmd *) pCmd, r, g, b, pName);
}

void CmdEndDebugMarker(Cmd *pCmd) {
  Metal::CmdEndDebugMarker((Metal::Cmd *) pCmd);
}

void CmdAddDebugMarker(Cmd *pCmd, float r, float g, float b, const char *pName) {
  Metal::CmdAddDebugMarker((Metal::Cmd *) pCmd, r, g, b, pName);
}

} // end namespace TheForge
