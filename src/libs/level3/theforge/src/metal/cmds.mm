#include "core/core.h"
#include "os/thread.hpp"
#include "theforge/metal/structs.hpp"
#include "theforge/metal/renderer.hpp"
#include "theforge/metal/descriptor.hpp"
#include "theforge/metal/utils.hpp"

namespace TheForge { namespace Metal {

void CmdBeginDebugMarker(Cmd *pCmd, float r, float g, float b, const char *pName) {
  if (pCmd->mtlRenderEncoder) {
    [pCmd->mtlRenderEncoder pushDebugGroup:[NSString stringWithFormat:@"%s", pName]];
  } else if (pCmd->mtlComputeEncoder) {
    [pCmd->mtlComputeEncoder pushDebugGroup:[NSString stringWithFormat:@"%s", pName]];
  } else if (pCmd->mtlBlitEncoder) {
    [pCmd->mtlBlitEncoder pushDebugGroup:[NSString stringWithFormat:@"%s", pName]];
  } else {
    [pCmd->mtlCommandBuffer pushDebugGroup:[NSString stringWithFormat:@"%s", pName]];
  }
}

void CmdBeginDebugMarkerf(Cmd *pCmd, float r, float g, float b, const char *pFormat, ...) {
  va_list argptr;
  va_start(argptr, pFormat);
  char buffer[65536];
  vsnprintf(buffer, sizeof(buffer), pFormat, argptr);
  va_end(argptr);
  CmdBeginDebugMarker(pCmd, r, g, b, buffer);
}

void CmdEndDebugMarker(Cmd *pCmd) {
  if (pCmd->mtlRenderEncoder) {
    [pCmd->mtlRenderEncoder popDebugGroup];
  } else if (pCmd->mtlComputeEncoder) {
    [pCmd->mtlComputeEncoder popDebugGroup];
  } else if (pCmd->mtlBlitEncoder) {
    [pCmd->mtlBlitEncoder popDebugGroup];
  } else {
    [pCmd->mtlCommandBuffer popDebugGroup];
  }
}

void CmdAddDebugMarker(Cmd *pCmd, float r, float g, float b, const char *pName) {
  if (pCmd->mtlRenderEncoder) {
    [pCmd->mtlRenderEncoder insertDebugSignpost:[NSString stringWithFormat:@"%s", pName]];
  } else if (pCmd->mtlComputeEncoder) {
    [pCmd->mtlComputeEncoder insertDebugSignpost:[NSString stringWithFormat:@"%s", pName]];
  } else if (pCmd->mtlBlitEncoder) {
    [pCmd->mtlBlitEncoder insertDebugSignpost:[NSString stringWithFormat:@"%s", pName]];
  }
}

void CmdAddDebugMarkerf(Cmd *pCmd, float r, float g, float b, const char *pFormat, ...) {
  va_list argptr;
  va_start(argptr, pFormat);
  char buffer[65536];
  vsnprintf(buffer, sizeof(buffer), pFormat, argptr);
  va_end(argptr);

  CmdAddDebugMarker(pCmd, r, g, b, buffer);
}

void CmdBindRenderTargets(
    Cmd *pCmd,
    uint32_t renderTargetCount,
    RenderTarget **ppRenderTargets,
    RenderTarget *pDepthStencil,
    const LoadActionsDesc *pLoadActions,
    uint32_t *pColorArraySlices,
    uint32_t *pColorMipSlices,
    uint32_t depthArraySlice,
    uint32_t depthMipSlice) {
  ASSERT(pCmd);

  if (pCmd->mRenderPassActive) {
    if (pCmd->pBoundRootSignature && pCmd->pBoundDescriptorBinder) {
      // Reset the bound resources flags for the current root signature's descriptor manager.
      ResetBoundResources((Metal::Renderer *) pCmd->pRenderer,
                          (Metal::DescriptorBinder *) pCmd->pBoundDescriptorBinder,
                          (Metal::RootSignature *) pCmd->pBoundRootSignature);
    } else {
      LOGWARNINGF("Render pass is active but no root signature is bound!");
    }

    @autoreleasepool {
      Util::EndCurrentEncoders(pCmd);
    }

    pCmd->mRenderPassActive = false;
    pCmd->mBoundRenderTargetCount = 0;
    pCmd->mBoundDepthStencilFormat = NONE;
  }

  if (!renderTargetCount && !pDepthStencil) {
    return;
  }

  uint64_t renderPassHash = 0;

  @autoreleasepool {
    pCmd->pRenderPassDesc = [MTLRenderPassDescriptor renderPassDescriptor];

    // Flush color attachments
    for (uint32_t i = 0; i < renderTargetCount; i++) {
      Texture *colorAttachment = (Metal::Texture *) (ppRenderTargets[i]->pTexture);

      pCmd->pRenderPassDesc.colorAttachments[i].texture = colorAttachment->mtlTexture;
      pCmd->pRenderPassDesc.colorAttachments[i].level = pColorMipSlices ? pColorMipSlices[i] : 0;
      if (colorAttachment->mDesc.mDescriptors & DESCRIPTOR_TYPE_RENDER_TARGET_ARRAY_SLICES) {
        pCmd->pRenderPassDesc.colorAttachments[i].slice = pColorArraySlices ? pColorArraySlices[i] : 0;
      } else if (colorAttachment->mDesc.mDescriptors & DESCRIPTOR_TYPE_RENDER_TARGET_DEPTH_SLICES) {
        pCmd->pRenderPassDesc.colorAttachments[i].depthPlane = pColorArraySlices ? pColorArraySlices[i] : 0;
      }
#ifndef TARGET_IOS
      pCmd->pRenderPassDesc.colorAttachments[i].loadAction =
          (pLoadActions != NULL ? Util::ToMtlLoadAction(pLoadActions->mLoadActionsColor[i]) : MTLLoadActionLoad);
      pCmd->pRenderPassDesc.colorAttachments[i].storeAction = MTLStoreActionStore;
#else
      if (colorAttachment->mtlTexture.storageMode == MTLStorageModeMemoryless)
            {
                pCmd->pRenderPassDesc.colorAttachments[i].loadAction = MTLLoadActionDontCare;
                pCmd->pRenderPassDesc.colorAttachments[i].storeAction = MTLStoreActionDontCare;
            }
            else
            {
                pCmd->pRenderPassDesc.colorAttachments[i].loadAction =
                    (pLoadActions != NULL ? util_to_mtl_load_action(pLoadActions->mLoadActionsColor[i]) : MTLLoadActionLoad);
                pCmd->pRenderPassDesc.colorAttachments[i].storeAction = MTLStoreActionStore;
            }
#endif
      if (pLoadActions != NULL) {
        const ClearValue& clearValue = pLoadActions->mClearColorValues[i];
        pCmd->pRenderPassDesc.colorAttachments[i].clearColor =
            MTLClearColorMake(clearValue.r, clearValue.g, clearValue.b, clearValue.a);
      }

      pCmd->pBoundColorFormats[i] = ppRenderTargets[i]->mDesc.mFormat;

      uint32_t hashValues[] = {
          (uint32_t) ppRenderTargets[i]->mDesc.mFormat,
          (uint32_t) ppRenderTargets[i]->mDesc.mSampleCount,
      };
      renderPassHash = stb_hash_fast(hashValues, sizeof(hashValues));
    }

    if (pDepthStencil != nil) {
      Metal::Texture *texture = (Metal::Texture *) pDepthStencil->pTexture;
      pCmd->pRenderPassDesc.depthAttachment.texture = texture->mtlTexture;
      pCmd->pRenderPassDesc.depthAttachment.level = (depthMipSlice != -1 ? depthMipSlice : 0);
      pCmd->pRenderPassDesc.depthAttachment.slice = (depthArraySlice != -1 ? depthArraySlice : 0);
#ifndef TARGET_IOS
      bool isStencilEnabled = texture->mtlPixelFormat == MTLPixelFormatDepth24Unorm_Stencil8;
      if (isStencilEnabled) {
        pCmd->pRenderPassDesc.stencilAttachment.texture = texture->mtlTexture;
        pCmd->pRenderPassDesc.stencilAttachment.level = (depthMipSlice != -1 ? depthMipSlice : 0);
        pCmd->pRenderPassDesc.stencilAttachment.slice = (depthArraySlice != -1 ? depthArraySlice : 0);
      }

      pCmd->pRenderPassDesc.depthAttachment.loadAction =
          pLoadActions ? Util::ToMtlLoadAction(pLoadActions->mLoadActionDepth) : MTLLoadActionClear;
      pCmd->pRenderPassDesc.depthAttachment.storeAction = MTLStoreActionStore;
      if (isStencilEnabled) {
        pCmd->pRenderPassDesc.stencilAttachment.loadAction =
            pLoadActions ? Util::ToMtlLoadAction(pLoadActions->mLoadActionStencil) : MTLLoadActionDontCare;
        pCmd->pRenderPassDesc.stencilAttachment.storeAction = MTLStoreActionStore;
      } else {
        pCmd->pRenderPassDesc.stencilAttachment.loadAction = MTLLoadActionDontCare;
        pCmd->pRenderPassDesc.stencilAttachment.storeAction = MTLStoreActionDontCare;
      }
#else
      bool isStencilEnabled = pDepthStencil->pStencil != nil;
            if (isStencilEnabled)
            {
                pCmd->pRenderPassDesc.stencilAttachment.texture = pDepthStencil->pStencil->mtlTexture;
                pCmd->pRenderPassDesc.stencilAttachment.level = (depthMipSlice != -1 ? depthMipSlice : 0);
                pCmd->pRenderPassDesc.stencilAttachment.slice = (depthArraySlice != -1 ? depthArraySlice : 0);
            }

            if (pDepthStencil->pTexture->mtlTexture.storageMode != MTLStorageModeMemoryless)
            {
                pCmd->pRenderPassDesc.depthAttachment.loadAction =
                    pLoadActions ? util_to_mtl_load_action(pLoadActions->mLoadActionDepth) : MTLLoadActionDontCare;
                pCmd->pRenderPassDesc.depthAttachment.storeAction = MTLStoreActionStore;
                if (isStencilEnabled)
                {
                    pCmd->pRenderPassDesc.stencilAttachment.loadAction =
                        pLoadActions ? util_to_mtl_load_action(pLoadActions->mLoadActionStencil) : MTLLoadActionDontCare;
                    pCmd->pRenderPassDesc.stencilAttachment.storeAction = MTLStoreActionStore;
                }
                else
                {
                    pCmd->pRenderPassDesc.stencilAttachment.loadAction = MTLLoadActionDontCare;
                    pCmd->pRenderPassDesc.stencilAttachment.storeAction = MTLStoreActionDontCare;
                }
            }
            else
            {
                pCmd->pRenderPassDesc.depthAttachment.loadAction = MTLLoadActionDontCare;
                pCmd->pRenderPassDesc.depthAttachment.storeAction = MTLStoreActionDontCare;
                pCmd->pRenderPassDesc.stencilAttachment.loadAction = MTLLoadActionDontCare;
                pCmd->pRenderPassDesc.stencilAttachment.storeAction = MTLStoreActionDontCare;
            }
#endif
      if (pLoadActions) {
        pCmd->pRenderPassDesc.depthAttachment.clearDepth = pLoadActions->mClearDepth.depth;
        if (isStencilEnabled) {
          pCmd->pRenderPassDesc.stencilAttachment.clearStencil = 0;
        }
      }

      pCmd->mBoundDepthStencilFormat = pDepthStencil->mDesc.mFormat;

      uint32_t hashValues[] = {
          (uint32_t) pDepthStencil->mDesc.mFormat,
          (uint32_t) pDepthStencil->mDesc.mSampleCount,
      };
      renderPassHash = stb_hash_fast(hashValues, sizeof(hashValues));
    } else {
      pCmd->pRenderPassDesc.depthAttachment.loadAction = MTLLoadActionDontCare;
      pCmd->pRenderPassDesc.stencilAttachment.loadAction = MTLLoadActionDontCare;
      pCmd->pRenderPassDesc.depthAttachment.storeAction = MTLStoreActionDontCare;
      pCmd->pRenderPassDesc.stencilAttachment.storeAction = MTLStoreActionDontCare;
      pCmd->mBoundDepthStencilFormat = NONE;
    }

    SampleCount
        sampleCount = renderTargetCount ? ppRenderTargets[0]->mDesc.mSampleCount : pDepthStencil->mDesc.mSampleCount;
    pCmd->mBoundWidth = renderTargetCount ? ppRenderTargets[0]->mDesc.mWidth : pDepthStencil->mDesc.mWidth;
    pCmd->mBoundHeight = renderTargetCount ? ppRenderTargets[0]->mDesc.mHeight : pDepthStencil->mDesc.mHeight;
    pCmd->mBoundSampleCount = sampleCount;
    pCmd->mBoundRenderTargetCount = renderTargetCount;

    // Check if we need to sync different types of encoders (only on direct cmds).
    bool switchedEncoders = Util::SyncEncoders(pCmd, CMD_POOL_DIRECT);
    Util::EndCurrentEncoders(pCmd);
    pCmd->mtlRenderEncoder = [pCmd->mtlCommandBuffer renderCommandEncoderWithDescriptor:pCmd->pRenderPassDesc];
    if (switchedEncoders) {
      [pCmd->mtlRenderEncoder waitForFence:pCmd->mtlEncoderFence beforeStages:MTLRenderStageVertex];
    }

    pCmd->mRenderPassActive = true;
  }
}

void CmdSetViewport(Cmd *pCmd, float x, float y, float width, float height, float minDepth, float maxDepth) {
  ASSERT(pCmd);
  if (pCmd->mtlRenderEncoder == nil) {
    InternalLog(LOG_TYPE_ERROR,
                "Using cmdSetViewport out of a cmdBeginRender / cmdEndRender block is not allowed",
                "cmdSetViewport");
    return;
  }

  MTLViewport viewport;
  viewport.originX = x;
  viewport.originY = y;
  viewport.width = width;
  viewport.height = height;
  viewport.znear = minDepth;
  viewport.zfar = maxDepth;

  [pCmd->mtlRenderEncoder setViewport:viewport];
}

void CmdSetScissor(Cmd *pCmd, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
  ASSERT(pCmd);
  if (pCmd->mtlRenderEncoder == nil) {
    InternalLog(LOG_TYPE_ERROR,
                "Using cmdSetScissor out of a cmdBeginRender / cmdEndRender block is not allowed",
                "cmdSetScissor");
    return;
  }

  // Get the maximum safe scissor values for the current render pass.
  uint32_t maxScissorX = pCmd->pRenderPassDesc.colorAttachments[0].texture.width > 0
                         ? (uint32_t) pCmd->pRenderPassDesc.colorAttachments[0].texture.width
                         : (uint32_t) pCmd->pRenderPassDesc.depthAttachment.texture.width;
  uint32_t maxScissorY = pCmd->pRenderPassDesc.colorAttachments[0].texture.height > 0
                         ? (uint32_t) pCmd->pRenderPassDesc.colorAttachments[0].texture.height
                         : (uint32_t) pCmd->pRenderPassDesc.depthAttachment.texture.height;
  uint32_t maxScissorW = maxScissorX - int32_t(Math_MaxU32(x, 0));
  uint32_t maxScissorH = maxScissorY - int32_t(Math_MaxU32(y, 0));

  // Make sure neither width or height are 0 (unsupported by Metal).
  if (width == 0u) {
    width = 1u;
  }
  if (height == 0u) {
    height = 1u;
  }

  MTLScissorRect scissor;
  scissor.x = Math_MinU32(x, maxScissorX);
  scissor.y = Math_MinU32(y, maxScissorY);
  scissor.width = Math_MinU32(width, maxScissorW);
  scissor.height = Math_MinU32(height, maxScissorH);

  [pCmd->mtlRenderEncoder setScissorRect:scissor];
}

void CmdBindPipeline(Cmd *pCmd, Pipeline *pPipeline) {
  ASSERT(pCmd);
  ASSERT(pPipeline);

  pCmd->pShader = pPipeline->pShader;

  @autoreleasepool {
    if (pPipeline->mType == PIPELINE_TYPE_GRAPHICS) {
      [pCmd->mtlRenderEncoder setRenderPipelineState:pPipeline->mtlRenderPipelineState];

      RasterizerState *rasterizerState = (RasterizerState *) pPipeline->mGraphics.pRasterizerState;
      [pCmd->mtlRenderEncoder setCullMode:rasterizerState->cullMode];
      [pCmd->mtlRenderEncoder setTriangleFillMode:rasterizerState->fillMode];
      [pCmd->mtlRenderEncoder setFrontFacingWinding:rasterizerState->frontFace];

      if (pCmd->pRenderPassDesc.depthAttachment.texture != nil) {
        DepthState *depthState = (DepthState *) pPipeline->mGraphics.pDepthState;
        [pCmd->mtlRenderEncoder setDepthStencilState:depthState->mtlDepthState];
      }

      switch (pPipeline->mGraphics.mPrimitiveTopo) {
        case PRIMITIVE_TOPO_POINT_LIST: pCmd->selectedPrimitiveType = MTLPrimitiveTypePoint;
          break;
        case PRIMITIVE_TOPO_LINE_LIST: pCmd->selectedPrimitiveType = MTLPrimitiveTypeLine;
          break;
        case PRIMITIVE_TOPO_LINE_STRIP: pCmd->selectedPrimitiveType = MTLPrimitiveTypeLineStrip;
          break;
        case PRIMITIVE_TOPO_TRI_LIST: pCmd->selectedPrimitiveType = MTLPrimitiveTypeTriangle;
          break;
        case PRIMITIVE_TOPO_TRI_STRIP: pCmd->selectedPrimitiveType = MTLPrimitiveTypeTriangleStrip;
          break;
        default: pCmd->selectedPrimitiveType = MTLPrimitiveTypeTriangle;
          break;
      }
    } else {
      if (!pCmd->mtlComputeEncoder) {
        Util::EndCurrentEncoders(pCmd);
        pCmd->mtlComputeEncoder = [pCmd->mtlCommandBuffer computeCommandEncoder];
      }
      [pCmd->mtlComputeEncoder setComputePipelineState:pPipeline->mtlComputePipelineState];
    }
  }
}

void CmdBindIndexBuffer(Cmd *pCmd, Buffer *pBuffer, uint64_t offset) {
  ASSERT(pCmd);
  ASSERT(pBuffer);

  pCmd->selectedIndexBuffer = pBuffer;
  pCmd->mSelectedIndexBufferOffset = offset;
}

void CmdBindVertexBuffer(Cmd *pCmd, uint32_t bufferCount, Buffer **ppBuffers, uint64_t *pOffsets) {
  ASSERT(pCmd);
  ASSERT(0 != bufferCount);
  ASSERT(ppBuffers);

  Shader *pShader = (Shader *) pCmd->pShader;
  // When using a poss-tessellation vertex shader, the first vertex buffer bound is used as the tessellation factors buffer.
  uint startIdx = 0;
  if (pShader && pShader->mtlVertexShader.patchType != MTLPatchTypeNone) {
    startIdx = 1;
    [pCmd->mtlRenderEncoder setTessellationFactorBuffer:ppBuffers[0]->mtlBuffer offset:0 instanceStride:0];
  }

  for (uint32_t i = startIdx; i < bufferCount; i++) {
    [pCmd->mtlRenderEncoder setVertexBuffer:ppBuffers[i]->mtlBuffer
                                     offset:(ppBuffers[i]->mPositionInHeap + (pOffsets ? pOffsets[i] : 0))
                                    atIndex:(i - startIdx)];
  }
}

void CmdDraw(Cmd *pCmd, uint32_t vertexCount, uint32_t firstVertex) {
  ASSERT(pCmd);
  Shader *pShader = (Shader *) pCmd->pShader;
  if (pShader->mtlVertexShader.patchType == MTLPatchTypeNone) {
    [pCmd->mtlRenderEncoder drawPrimitives:pCmd->selectedPrimitiveType vertexStart:firstVertex vertexCount:vertexCount];
  } else    // Tessellated draw version.
  {
    [pCmd->mtlRenderEncoder drawPatches:pShader->mtlVertexShader.patchControlPointCount
                             patchStart:firstVertex
                             patchCount:vertexCount
                       patchIndexBuffer:nil
                 patchIndexBufferOffset:0
                          instanceCount:1
                           baseInstance:0];
  }
}

void CmdDrawInstanced(Cmd *pCmd,
                      uint32_t vertexCount,
                      uint32_t firstVertex,
                      uint32_t instanceCount,
                      uint32_t firstInstance) {
  ASSERT(pCmd);
  Shader *pShader = (Shader *) pCmd->pShader;
  if (pShader->mtlVertexShader.patchType == MTLPatchTypeNone) {
    if (firstInstance == 0) {
      [pCmd->mtlRenderEncoder drawPrimitives:pCmd->selectedPrimitiveType
                                 vertexStart:firstVertex
                                 vertexCount:vertexCount
                               instanceCount:instanceCount];
    } else {
      [pCmd->mtlRenderEncoder drawPrimitives:pCmd->selectedPrimitiveType
                                 vertexStart:firstVertex
                                 vertexCount:vertexCount
                               instanceCount:instanceCount
                                baseInstance:firstInstance];
    }
  } else    // Tessellated draw version.
  {
    [pCmd->mtlRenderEncoder drawPatches:pShader->mtlVertexShader.patchControlPointCount
                             patchStart:firstVertex
                             patchCount:vertexCount
                       patchIndexBuffer:nil
                 patchIndexBufferOffset:0
                          instanceCount:instanceCount
                           baseInstance:firstInstance];
  }
}

void CmdDrawIndexed(Cmd *pCmd, uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex) {
  ASSERT(pCmd);
  Shader *pShader = (Shader *) pCmd->pShader;
  Buffer *indexBuffer = pCmd->selectedIndexBuffer;
  MTLIndexType
      indexType = (indexBuffer->mDesc.mIndexType == INDEX_TYPE_UINT16 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32);
  uint64_t offset =
      pCmd->mSelectedIndexBufferOffset + (firstIndex * (indexBuffer->mDesc.mIndexType == INDEX_TYPE_UINT16 ? 2 : 4));

  if (pShader->mtlVertexShader.patchType == MTLPatchTypeNone) {
    //only ios devices supporting gpu family 3_v1 and above can use baseVertex and baseInstance
    //if lower than 3_v1 render without base info but artifacts will occur if used.
#ifdef TARGET_IOS
    if ([pCmd->pRenderer->pDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v1])
#endif
    {
      [pCmd->mtlRenderEncoder drawIndexedPrimitives:pCmd->selectedPrimitiveType
                                         indexCount:indexCount
                                          indexType:indexType
                                        indexBuffer:indexBuffer->mtlBuffer
                                  indexBufferOffset:offset
                                      instanceCount:1
                                         baseVertex:firstVertex
                                       baseInstance:0];
    }
#ifdef TARGET_IOS
    else
        {
            LOGERRORF("Current device does not support ios gpuFamily 3_v1 feature set.");
            return;
        }
#endif
  } else    // Tessellated draw version.
  {
    //to supress warning passing nil to controlPointIndexBuffer
    //todo: Add control point index buffer to be passed when necessary
    id <MTLBuffer> _Nullable indexBuf = nil;
    [pCmd->mtlRenderEncoder drawIndexedPatches:pShader->mtlVertexShader.patchControlPointCount
                                    patchStart:firstIndex
                                    patchCount:indexCount
                              patchIndexBuffer:indexBuffer->mtlBuffer
                        patchIndexBufferOffset:0
                       controlPointIndexBuffer:indexBuf
                 controlPointIndexBufferOffset:0
                                 instanceCount:1
                                  baseInstance:0];
  }
}

void CmdDrawIndexedInstanced(
    Cmd *pCmd,
    uint32_t indexCount,
    uint32_t firstIndex,
    uint32_t instanceCount,
    uint32_t firstInstance,
    uint32_t firstVertex) {
  ASSERT(pCmd);

  Buffer *indexBuffer = pCmd->selectedIndexBuffer;
  Shader *pShader = (Shader *) pCmd->pShader;
  MTLIndexType
      indexType = (indexBuffer->mDesc.mIndexType == INDEX_TYPE_UINT16 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32);
  uint64_t offset =
      pCmd->mSelectedIndexBufferOffset + (firstIndex * (indexBuffer->mDesc.mIndexType == INDEX_TYPE_UINT16 ? 2 : 4));

  if (pShader->mtlVertexShader.patchType == MTLPatchTypeNone) {
    [pCmd->mtlRenderEncoder drawIndexedPrimitives:pCmd->selectedPrimitiveType
                                       indexCount:indexCount
                                        indexType:indexType
                                      indexBuffer:indexBuffer->mtlBuffer
                                indexBufferOffset:offset
                                    instanceCount:instanceCount
                                       baseVertex:firstVertex
                                     baseInstance:firstInstance];
  } else    // Tessellated draw version.
  {
    //to supress warning passing nil to controlPointIndexBuffer
    //todo: Add control point index buffer to be passed when necessary
    id <MTLBuffer> _Nullable indexBuf = nil;
    [pCmd->mtlRenderEncoder drawIndexedPatches:pShader->mtlVertexShader.patchControlPointCount
                                    patchStart:firstIndex
                                    patchCount:indexCount
                              patchIndexBuffer:indexBuffer->mtlBuffer
                        patchIndexBufferOffset:0
                       controlPointIndexBuffer:indexBuf
                 controlPointIndexBufferOffset:0
                                 instanceCount:instanceCount
                                  baseInstance:firstInstance];
  }
}

void CmdDispatch(Cmd *pCmd, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
  ASSERT(pCmd);
  ASSERT(pCmd->mtlComputeEncoder != nil);

  Shader *pShader = (Shader *) pCmd->pShader;

  MTLSize threadsPerThreadgroup =
      MTLSizeMake(pShader->mNumThreadsPerGroup[0], pShader->mNumThreadsPerGroup[1], pShader->mNumThreadsPerGroup[2]);
  MTLSize threadgroupCount = MTLSizeMake(groupCountX, groupCountY, groupCountZ);

  [pCmd->mtlComputeEncoder dispatchThreadgroups:threadgroupCount threadsPerThreadgroup:threadsPerThreadgroup];
}

void CmdExecuteIndirect(
    Cmd *pCmd,
    CommandSignature *pCommandSignature,
    uint maxCommandCount,
    Buffer *pIndirectBuffer,
    uint64_t bufferOffset,
    Buffer *pCounterBuffer,
    uint64_t counterBufferOffset) {
  Shader *pShader = (Shader *) pCmd->pShader;
  for (uint32_t i = 0; i < maxCommandCount; i++) {
    if (pCommandSignature->mDrawType == INDIRECT_DRAW) {
      uint64_t indirectBufferOffset = bufferOffset + sizeof(IndirectDrawArguments) * i;
      if (pShader->mtlVertexShader.patchType == MTLPatchTypeNone) {
        [pCmd->mtlRenderEncoder drawPrimitives:pCmd->selectedPrimitiveType
                                indirectBuffer:pIndirectBuffer->mtlBuffer
                          indirectBufferOffset:indirectBufferOffset];
      } else    // Tessellated draw version.
      {
#ifndef TARGET_IOS
        [pCmd->mtlRenderEncoder drawPatches:pShader->mtlVertexShader.patchControlPointCount
                           patchIndexBuffer:nil
                     patchIndexBufferOffset:0
                             indirectBuffer:pIndirectBuffer->mtlBuffer
                       indirectBufferOffset:indirectBufferOffset];
#else
        // Tessellated indirect-draw is not supported on iOS.
                // Instead, read regular draw arguments from the indirect draw buffer.
                mapBuffer(pCmd->pRenderer, pIndirectBuffer, NULL);
                IndirectDrawArguments* pDrawArgs = (IndirectDrawArguments*)(pIndirectBuffer->pCpuMappedAddress) + indirectBufferOffset;
                unmapBuffer(pCmd->pRenderer, pIndirectBuffer);

                [pCmd->mtlRenderEncoder drawPatches:pCmd->pShader->mtlVertexShader.patchControlPointCount
                                         patchStart:pDrawArgs->mStartVertex
                                         patchCount:pDrawArgs->mVertexCount
                                   patchIndexBuffer:nil
                             patchIndexBufferOffset:0
                                      instanceCount:pDrawArgs->mInstanceCount
                                       baseInstance:pDrawArgs->mStartInstance];
#endif
      }
    } else if (pCommandSignature->mDrawType == INDIRECT_DRAW_INDEX) {
      Buffer *indexBuffer = pCmd->selectedIndexBuffer;
      MTLIndexType
          indexType = (indexBuffer->mDesc.mIndexType == INDEX_TYPE_UINT16 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32);
      uint64_t indirectBufferOffset = bufferOffset + sizeof(IndirectDrawIndexArguments) * i;

      if (pShader->mtlVertexShader.patchType == MTLPatchTypeNone) {
        [pCmd->mtlRenderEncoder drawIndexedPrimitives:pCmd->selectedPrimitiveType
                                            indexType:indexType
                                          indexBuffer:indexBuffer->mtlBuffer
                                    indexBufferOffset:0
                                       indirectBuffer:pIndirectBuffer->mtlBuffer
                                 indirectBufferOffset:indirectBufferOffset];
      } else    // Tessellated draw version.
      {
#ifndef TARGET_IOS
        [pCmd->mtlRenderEncoder drawPatches:pShader->mtlVertexShader.patchControlPointCount
                           patchIndexBuffer:indexBuffer->mtlBuffer
                     patchIndexBufferOffset:0
                             indirectBuffer:pIndirectBuffer->mtlBuffer
                       indirectBufferOffset:indirectBufferOffset];
#else
        // Tessellated indirect-draw is not supported on iOS.
                // Instead, read regular draw arguments from the indirect draw buffer.
                mapBuffer(pCmd->pRenderer, pIndirectBuffer, NULL);
                IndirectDrawIndexArguments* pDrawArgs =
                    (IndirectDrawIndexArguments*)(pIndirectBuffer->pCpuMappedAddress) + indirectBufferOffset;
                unmapBuffer(pCmd->pRenderer, pIndirectBuffer);

                //to supress warning passing nil to controlPointIndexBuffer
                //todo: Add control point index buffer to be passed when necessary
                id<MTLBuffer> _Nullable ctrlPtIndexBuf = nil;
                [pCmd->mtlRenderEncoder drawIndexedPatches:pCmd->pShader->mtlVertexShader.patchControlPointCount
                                                patchStart:pDrawArgs->mStartIndex
                                                patchCount:pDrawArgs->mIndexCount
                                          patchIndexBuffer:indexBuffer->mtlBuffer
                                    patchIndexBufferOffset:0
                                   controlPointIndexBuffer:ctrlPtIndexBuf
                             controlPointIndexBufferOffset:0
                                             instanceCount:pDrawArgs->mInstanceCount
                                              baseInstance:pDrawArgs->mStartInstance];
#endif
      }
    } else if (pCommandSignature->mDrawType == INDIRECT_DISPATCH) {
      //TODO: Implement.
      ASSERT(0);
    }
  }
}

void CmdResourceBarrier(
    Cmd *pCmd,
    uint32_t numBufferBarriers,
    BufferBarrier *pBufferBarriers,
    uint32_t numTextureBarriers,
    TextureBarrier *pTextureBarriers,
    bool batch) {}

void CmdSynchronizeResources(Cmd *pCmd,
                             uint32_t numBuffers,
                             Buffer **ppBuffers,
                             uint32_t numTextures,
                             Texture **ppTextures,
                             bool batch) {}

void CmdFlushBarriers(Cmd *pCmd) {}

void CmdUpdateBuffer(Cmd *pCmd,
                     uint64_t srcOffset,
                     uint64_t dstOffset,
                     uint64_t size,
                     Buffer *pSrcBuffer,
                     Buffer *pBuffer) {
  ASSERT(pCmd);
  ASSERT(pSrcBuffer);
  ASSERT(pSrcBuffer->mtlBuffer);
  ASSERT(pBuffer);
  ASSERT(pBuffer->mtlBuffer);
  ASSERT(srcOffset + size <= pSrcBuffer->mDesc.mSize);
  ASSERT(dstOffset + size <= pBuffer->mDesc.mSize);

  Util::EndCurrentEncoders(pCmd);
  pCmd->mtlBlitEncoder = [pCmd->mtlCommandBuffer blitCommandEncoder];

  [pCmd->mtlBlitEncoder copyFromBuffer:pSrcBuffer->mtlBuffer
                          sourceOffset:srcOffset
                              toBuffer:pBuffer->mtlBuffer
                     destinationOffset:dstOffset
                                  size:size];
}

void CmdUpdateSubresources(
    Cmd *pCmd,
    uint32_t startSubresource,
    uint32_t numSubresources,
    SubresourceDataDesc *pSubresources,
    Buffer *pIntermediate,
    uint64_t intermediateOffset,
    Texture *pTexture) {
  Util::EndCurrentEncoders(pCmd);
  pCmd->mtlBlitEncoder = [pCmd->mtlCommandBuffer blitCommandEncoder];

  uint nLayers = pTexture->mDesc.mArraySize;
  uint nFaces = 1;
  uint nMips = pTexture->mDesc.mMipLevels;

  bool isPvrtc = Image_Format_IsPVR(pTexture->mDesc.mFormat);

  uint32_t subresourceOffset = 0;
  for (uint32_t layer = 0; layer < nLayers; ++layer) {
    for (uint32_t face = 0; face < nFaces; ++face) {
      for (uint32_t mip = 0; mip < nMips; ++mip) {
        SubresourceDataDesc *pRes = &pSubresources[(layer * nFaces * nMips) + (face * nMips) + mip];
        uint32_t mipmapWidth = Math_MaxU32(pTexture->mDesc.mWidth >> mip, 1);
        uint32_t mipmapHeight = Math_MaxU32(pTexture->mDesc.mHeight >> mip, 1);

        // NOTE: as of 2/4/2019, there is a bug in the Metal API when copying PVRTC textures using MTLBlitCommandEncoder
        // See: https://forums.developer.apple.com/thread/113262
        // Hence, for PVRTC we temporarily create a non-private texture so we can use replaceRegion.  We then copy that over to the private texture.
        // This is the better work around as we get to both keep the final texture private while still being able to use the Metal validation layer.

        if (isPvrtc) {
          Texture *pTmpTex = NULL;
          AddTexture((Renderer *) pCmd->pRenderer, &pTexture->mDesc, &pTmpTex, false, true);

          [pTmpTex->mtlTexture replaceRegion:MTLRegionMake2D(0, 0, mipmapWidth, mipmapHeight)
                                 mipmapLevel:mip
                                       slice:layer * nFaces + face
                                   withBytes:pRes->pData
                                 bytesPerRow:0
                               bytesPerImage:0];

          [pCmd->mtlBlitEncoder copyFromTexture:pTmpTex->mtlTexture
                                    sourceSlice:layer * nFaces + face
                                    sourceLevel:mip
                                   sourceOrigin:MTLOriginMake(0, 0, 0)
                                     sourceSize:MTLSizeMake(mipmapWidth, mipmapHeight, 1)
                                      toTexture:pTexture->mtlTexture
                               destinationSlice:layer * nFaces + face
                               destinationLevel:mip
                              destinationOrigin:MTLOriginMake(0, 0, 0)];

          [pCmd->mtlCommandBuffer addCompletedHandler:^(id <MTLCommandBuffer> buffer) {
            RemoveTexture((Renderer *) pCmd->pRenderer, pTmpTex);
          }];
        } else {
          // Copy the data for this resource to an intermediate buffer.
          memcpy((uint8_t *) pIntermediate->pCpuMappedAddress + intermediateOffset + subresourceOffset,
                 pRes->pData,
                 pRes->mSlicePitch);

          // Copy to the texture's final subresource.
          [pCmd->mtlBlitEncoder copyFromBuffer:pIntermediate->mtlBuffer
                                  sourceOffset:intermediateOffset + subresourceOffset
                             sourceBytesPerRow:pRes->mRowPitch
                           sourceBytesPerImage:pRes->mSlicePitch
                                    sourceSize:MTLSizeMake(mipmapWidth, mipmapHeight, 1)
                                     toTexture:pTexture->mtlTexture
                              destinationSlice:layer * nFaces + face
                              destinationLevel:mip
                             destinationOrigin:MTLOriginMake(0, 0, 0)];
        }

        // Increase the subresource offset.
        subresourceOffset += pRes->mSlicePitch;
      }
    }
  }
}

void BeginCmd(Cmd *pCmd) {
  @autoreleasepool {
    ASSERT(pCmd);
    pCmd->mtlRenderEncoder = nil;
    pCmd->mtlComputeEncoder = nil;
    pCmd->mtlBlitEncoder = nil;
    pCmd->pShader = nil;
    pCmd->pRenderPassDesc = nil;
    pCmd->selectedIndexBuffer = nil;
    pCmd->pBoundRootSignature = nil;
    Metal::Queue *q = (Metal::Queue *) pCmd->pCmdPool->pQueue;
    pCmd->mtlCommandBuffer = [q->mtlCommandQueue commandBuffer];
  }
}

void EndCmd(Cmd *pCmd) {
  if (pCmd->mRenderPassActive) {
    if (pCmd->pBoundRootSignature && pCmd->pBoundDescriptorBinder) {
      // Reset the bound resources flags for the current root signature's descriptor manager.
      ResetBoundResources((Metal::Renderer *) pCmd->pRenderer,
                          (Metal::DescriptorBinder *) pCmd->pBoundDescriptorBinder,
                          (Metal::RootSignature *) pCmd->pBoundRootSignature);
    }

    @autoreleasepool {
      Util::EndCurrentEncoders(pCmd);
    }
  }

  pCmd->mRenderPassActive = false;
  pCmd->mBoundRenderTargetCount = 0;
  pCmd->mBoundDepthStencilFormat = NONE;
}

void CmdBeginQuery(Cmd *pCmd, QueryHeap *pQueryHeap, QueryDesc *pQuery) {
  // NOT SUPPORTED
  ASSERT(false);
}

}} // end namespace TheForge::Metal