
#include "core/core.h"
#include "theforge/metal/structs.hpp"
#include "theforge/metal/renderer.hpp"
#include "theforge/metal/descriptor.hpp"
#include "os/thread.hpp"
#include "theforge/metal/utils.hpp"

namespace TheForge { namespace Metal {

void ResetBoundResources(Renderer *pRenderer,
                         DescriptorBinder *pDescriptorBinder,
                         RootSignature *pRootSignature) {
  DescriptorBinderNode& node = pDescriptorBinder->mRootSignatureNodes[pRootSignature];

  node.mBoundStaticSamplers = false;
  for (uint32_t i = 0; i < pRootSignature->mDescriptorCount; ++i) {
    DescriptorInfo *descInfo = &pRootSignature->pDescriptors[i];

    node.pDescriptorDataArray[i].mCount = 1;
    node.pDescriptorDataArray[i].pOffsets = NULL;
    node.pDescriptorDataArray[i].pSizes = NULL;

    // Metal requires that the bound textures match the texture type present in the shader.
    Texture **ppDefaultTexture = nil;
    if (descInfo->mDesc.type == DESCRIPTOR_TYPE_RW_TEXTURE || descInfo->mDesc.type == DESCRIPTOR_TYPE_TEXTURE) {
      switch ((MTLTextureType) descInfo->mDesc.backend.mtlTextureType) {
        case MTLTextureType1D: ppDefaultTexture = &pRenderer->pDefault1DTexture;
          break;
        case MTLTextureType1DArray: ppDefaultTexture = &pRenderer->pDefault1DTextureArray;
          break;
        case MTLTextureType2D: ppDefaultTexture = &pRenderer->pDefault2DTexture;
          break;
        case MTLTextureType2DArray: ppDefaultTexture = &pRenderer->pDefault2DTextureArray;
          break;
        case MTLTextureType3D: ppDefaultTexture = &pRenderer->pDefault3DTexture;
          break;
        case MTLTextureTypeCube: ppDefaultTexture = &pRenderer->pDefaultCubeTexture;
          break;
        case MTLTextureTypeCubeArray: ppDefaultTexture = &pRenderer->pDefaultCubeTextureArray;
          break;
        default: break;
      }
    }

    switch (descInfo->mDesc.type) {
      case DESCRIPTOR_TYPE_RW_TEXTURE:
      case DESCRIPTOR_TYPE_TEXTURE: node.pDescriptorDataArray[i].ppTextures = (TheForge_Texture **) ppDefaultTexture;
        break;
      case DESCRIPTOR_TYPE_SAMPLER: node.pDescriptorDataArray[i].ppSamplers = (TheForge_Sampler **)&pRenderer->pDefaultSampler;
        break;
      case DESCRIPTOR_TYPE_ROOT_CONSTANT: node.pDescriptorDataArray[i].pRootConstant = &pRenderer->pDefaultBuffer;
        break;
      case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      case DESCRIPTOR_TYPE_RW_BUFFER:
      case DESCRIPTOR_TYPE_BUFFER: {
        node.pDescriptorDataArray[i].ppBuffers = (TheForge_Buffer**)&pRenderer->pDefaultBuffer;
        break;
        default: break;
      }
    }
    node.pBoundDescriptors[i] = false;
  }
}
void BindArgumentBuffer(Cmd *pCmd,
                        DescriptorBinderNode& node,
                        const DescriptorInfo *descInfo,
                        const DescriptorData *descData) {
  Buffer *argumentBuffer;
  bool bufferNeedsReencoding = false;

  Shader *pShader = (Shader *) pShader;

  id <MTLArgumentEncoder> argumentEncoder = nil;
  id <MTLFunction> shaderStage = nil;

  // Look for the argument buffer (or create one if needed).
  size_t hash = tinystl::hash(descData->pName);
  {
    DescriptorBinderNode::ArgumentBufferMap::iterator jt = node.mArgumentBuffers.find(hash);

    // If not previous argument buffer was found, create a new bufffer.
    if (jt.node == nil) {
      // Find a shader stage using this argument buffer.
      ShaderStage stageMask = descInfo->mDesc.used_stages;
      if ((stageMask & SHADER_STAGE_VERT) != 0) {
        shaderStage = pShader->mtlVertexShader;
      } else if ((stageMask & SHADER_STAGE_FRAG) != 0) {
        shaderStage = pShader->mtlFragmentShader;
      } else if ((stageMask & SHADER_STAGE_COMP) != 0) {
        shaderStage = pShader->mtlComputeShader;
      }
      assert(shaderStage != nil);

      // Create the argument buffer/encoder pair.
      argumentEncoder = [shaderStage newArgumentEncoderWithBufferIndex:descInfo->mDesc.reg];
      BufferDesc bufferDesc = {};
      bufferDesc.mSize = argumentEncoder.encodedLength;
      bufferDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
      bufferDesc.mFlags = BUFFER_CREATION_FLAG_OWN_MEMORY_BIT;
      AddBuffer((Renderer *) pCmd->pRenderer, &bufferDesc, &argumentBuffer);

      node.mArgumentBuffers[hash] = {argumentBuffer, true};
      bufferNeedsReencoding = true;
    } else {
      argumentBuffer = jt->second.first;
      bufferNeedsReencoding = jt->second.second;
    }
  }

  // Update the argument buffer's data.
  if (bufferNeedsReencoding) {
    if (!argumentEncoder) {
      argumentEncoder = [shaderStage newArgumentEncoderWithBufferIndex:descInfo->mDesc.reg];
    }

    [argumentEncoder setArgumentBuffer:argumentBuffer->mtlBuffer offset:0];
    for (uint32_t i = 0; i < descData->mCount; i++) {
      switch (descInfo->mDesc.backend.mtlArgumentBufferType) {
        case DESCRIPTOR_TYPE_SAMPLER:
          [argumentEncoder setSamplerState:((Sampler *) descData->ppSamplers[i])->mtlSamplerState
                                   atIndex:i];
          break;
        case DESCRIPTOR_TYPE_BUFFER:
          [pCmd->mtlRenderEncoder useResource:((Buffer *) descData->ppBuffers[i])->mtlBuffer
                                        usage:(MTLResourceUsageRead | MTLResourceUsageSample)];
          [argumentEncoder setBuffer:((Buffer *) descData->ppBuffers[i])->mtlBuffer
                              offset:(descData->ppBuffers[i]->mPositionInHeap
                                  + (descData->pOffsets ? descData->pOffsets[i] : 0))
                             atIndex:i];
          break;
        case DESCRIPTOR_TYPE_TEXTURE:[pCmd->mtlRenderEncoder useResource:((Texture *) descData->ppTextures[i])->mtlTexture usage:MTLResourceUsageRead];
          [argumentEncoder setTexture:((Texture *) descData->ppTextures[i])->mtlTexture
                              atIndex:i];
          break;
      }
    }

    node.mArgumentBuffers[hash].second = false;
  }

  // Bind the argument buffer.
  if ((descInfo->mDesc.used_stages & SHADER_STAGE_VERT) != 0) {
    [pCmd->mtlRenderEncoder setVertexBuffer:argumentBuffer->mtlBuffer
                                     offset:argumentBuffer->mPositionInHeap
                                    atIndex:descInfo->mDesc.reg];
  }
  if ((descInfo->mDesc.used_stages & SHADER_STAGE_FRAG) != 0) {
    [pCmd->mtlRenderEncoder setFragmentBuffer:argumentBuffer->mtlBuffer
                                       offset:argumentBuffer->mPositionInHeap
                                      atIndex:descInfo->mDesc.reg];
  }
  if ((descInfo->mDesc.used_stages & SHADER_STAGE_COMP) != 0) {
    [pCmd->mtlComputeEncoder setBuffer:argumentBuffer->mtlBuffer
                                offset:argumentBuffer->mPositionInHeap
                               atIndex:descInfo->mDesc.reg];
  }
}

void CmdBindDescriptors(TheForge::Metal::Cmd *pCmd,
                         TheForge::Metal::DescriptorBinder *pDescriptorBinder,
                         TheForge::Metal::RootSignature *pRootSignature,
                         uint32_t numDescriptors,
                         TheForge_DescriptorData *pDescParams) {
  ASSERT(pCmd);
  ASSERT(pDescriptorBinder);
  ASSERT(pRootSignature);

  DescriptorBinderNode& node = pDescriptorBinder->mRootSignatureNodes[pRootSignature];

  // Compare the currently bound descriptor binder with the new descriptor binder
  // If these values dont match, we must bind the new descriptor binder
  // If the values match, no op is required
  if (pCmd->pBoundDescriptorBinder != (TheForge_DescriptorBinder *) pDescriptorBinder ||
      pCmd->pBoundRootSignature != pRootSignature) {
    // Bind the new root signature and reset its bound resources (if any).
    pCmd->pBoundDescriptorBinder = (TheForge_DescriptorBinder *) pDescriptorBinder;
    pCmd->pBoundRootSignature = pRootSignature;
    ResetBoundResources((Renderer *) pCmd->pRenderer, pDescriptorBinder, pRootSignature);
  }

  // Loop through input params to check for new data
  for (uint32_t paramIdx = 0; paramIdx < numDescriptors; ++paramIdx) {
    const DescriptorData *pParam = &pDescParams[paramIdx];
    ASSERT(pParam);
    if (!pParam->pName) {
      LOGERRORF("Name of Descriptor at index (%u) is NULL", paramIdx);
      return;
    }

    uint32_t hash = stb_hash(pParam->pName);
    uint32_t descIndex = ~0u;
    const DescriptorInfo *pDesc = GetDescriptor(pRootSignature, pParam->pName, &descIndex);
    if (!pDesc) {
      continue;
    }

    const uint32_t arrayCount = Math_MaxI32(1U, pParam->mCount);

    // Replace the default DescriptorData by the new data pased into this function.
    node.pDescriptorDataArray[descIndex].pName = pParam->pName;
    node.pDescriptorDataArray[descIndex].mCount = arrayCount;
    node.pDescriptorDataArray[descIndex].pOffsets = pParam->pOffsets;
    switch (pDesc->mDesc.type) {
      case DESCRIPTOR_TYPE_RW_TEXTURE:
      case DESCRIPTOR_TYPE_TEXTURE:
        if (!pParam->ppTextures) {
          LOGERRORF("Texture descriptor (%s) is NULL", pParam->pName);
          return;
        }
        node.pDescriptorDataArray[descIndex].ppTextures = pParam->ppTextures;
        break;
      case DESCRIPTOR_TYPE_SAMPLER:
        if (!pParam->ppSamplers) {
          LOGERRORF("Sampler descriptor (%s) is NULL", pParam->pName);
          return;
        }
        node.pDescriptorDataArray[descIndex].ppSamplers = pParam->ppSamplers;
        break;
      case DESCRIPTOR_TYPE_ROOT_CONSTANT:
        if (!pParam->pRootConstant) {
          LOGERRORF("RootConstant array (%s) is NULL", pParam->pName);
          return;
        }
        node.pDescriptorDataArray[descIndex].pRootConstant = pParam->pRootConstant;
        break;
      case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      case DESCRIPTOR_TYPE_RW_BUFFER:
      case DESCRIPTOR_TYPE_BUFFER:
        if (!pParam->ppBuffers) {
          LOGERRORF("Buffer descriptor (%s) is NULL", pParam->pName);
          return;
        }
        node.pDescriptorDataArray[descIndex].ppBuffers = pParam->ppBuffers;

        // In case we're binding an argument buffer, signal that we need to re-encode the resources into the buffer.
        if (arrayCount > 1 && node.mArgumentBuffers.find(hash).node) {
          node.mArgumentBuffers[hash].second = true;
        }

        break;
      default: break;
    }

    // Mark this descriptor as unbound, so it's values are updated.
    node.pBoundDescriptors[descIndex] = false;
  }

  // If we're binding descriptors for a compute pipeline, we must ensure that we have a correct compute enconder recording commands.
  if (pCmd->pBoundRootSignature->mPipelineType == PIPELINE_TYPE_COMPUTE && !pCmd->mtlComputeEncoder) {
    Util::EndCurrentEncoders(pCmd);
    pCmd->mtlComputeEncoder = [pCmd->mtlCommandBuffer computeCommandEncoder];
  }

  // Bind all the unbound root signature descriptors.
  for (uint32_t i = 0; i < pRootSignature->mDescriptorCount; ++i) {
    const DescriptorInfo *descriptorInfo = &pRootSignature->pDescriptors[i];
    const DescriptorData *descriptorData = &node.pDescriptorDataArray[i];

    if (!node.pBoundDescriptors[i]) {
      ShaderStage usedStagesMask = descriptorInfo->mDesc.used_stages;
      switch (descriptorInfo->mDesc.type) {
        case DESCRIPTOR_TYPE_RW_TEXTURE: {
          uint32_t textureCount = Math_MaxI32(1U, descriptorData->mCount);
          for (uint32_t j = 0; j < textureCount; j++) {
            if (!descriptorData->ppTextures[j] || !((Metal::Texture *) descriptorData->ppTextures[j])->mtlTexture) {
              LOGERRORF("RW Texture descriptor (%s) at array index (%u) is NULL", descriptorData->pName, j);
              return;
            }
            Metal::Texture *texture = (Metal::Texture *) descriptorData->ppTextures[j];

            if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
              [pCmd->mtlRenderEncoder
                  setVertexTexture:texture->pMtlUAVDescriptors[descriptorData->mUAVMipSlice]
                           atIndex:descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
              [pCmd->mtlRenderEncoder
                  setFragmentTexture:texture->pMtlUAVDescriptors[descriptorData->mUAVMipSlice]
                             atIndex:descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
              [pCmd->mtlComputeEncoder
                  setTexture:texture->pMtlUAVDescriptors[descriptorData->mUAVMipSlice]
                     atIndex:descriptorInfo->mDesc.reg + j];
            }
          }
          break;
        }
        case DESCRIPTOR_TYPE_TEXTURE: {
          uint32_t textureCount = Math_MaxI32(1U, descriptorData->mCount);
          for (uint32_t j = 0; j < textureCount; j++) {
            if (!descriptorData->ppTextures[j] || !((Metal::Texture *) descriptorData->ppTextures[j])->mtlTexture) {
              LOGERRORF("Texture descriptor (%s) at array index (%u) is NULL", descriptorData->pName, j);
              return;
            }
            Metal::Texture *texture = (Metal::Texture *) descriptorData->ppTextures[j];

            if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
              [pCmd->mtlRenderEncoder
                  setVertexTexture:texture->mtlTexture
                           atIndex:descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
              [pCmd->mtlRenderEncoder
                  setFragmentTexture:texture->mtlTexture
                             atIndex:descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
              [pCmd->mtlComputeEncoder
                  setTexture:texture->mtlTexture
                     atIndex:descriptorInfo->mDesc.reg + j];
            }
          }
          break;
        }
        case DESCRIPTOR_TYPE_SAMPLER: {
          uint32_t samplerCount = Math_MaxI32(1U, descriptorData->mCount);
          for (uint32_t j = 0; j < samplerCount; j++) {
            if (!descriptorData->ppSamplers[j]
                || !((Metal::Sampler *) descriptorData->ppSamplers[j])->mtlSamplerState) {
              LOGERRORF("Texture descriptor (%s) at array index (%u) is NULL", descriptorData->pName, j);
              return;
            }
            Metal::Sampler *sampler = (Metal::Sampler *) descriptorData->ppSamplers[j];

            if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
              [pCmd->mtlRenderEncoder
                  setVertexSamplerState:sampler->mtlSamplerState
                                atIndex:descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
              [pCmd->mtlRenderEncoder
                  setFragmentSamplerState:sampler->mtlSamplerState
                                  atIndex:descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
              [pCmd->mtlComputeEncoder
                  setSamplerState:sampler->mtlSamplerState
                          atIndex:descriptorInfo->mDesc.reg + j];
            }
          }
          break;
        }
        case DESCRIPTOR_TYPE_ROOT_CONSTANT:
          if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
            [pCmd->mtlRenderEncoder setVertexBytes:descriptorData->pRootConstant
                                            length:descriptorInfo->mDesc.size
                                           atIndex:descriptorInfo->mDesc.reg];
          }
          if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
            [pCmd->mtlRenderEncoder setFragmentBytes:descriptorData->pRootConstant
                                              length:descriptorInfo->mDesc.size
                                             atIndex:descriptorInfo->mDesc.reg];
          }
          if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
            [pCmd->mtlComputeEncoder setBytes:descriptorData->pRootConstant
                                       length:descriptorInfo->mDesc.size
                                      atIndex:descriptorInfo->mDesc.reg];
          }
          break;
        case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case DESCRIPTOR_TYPE_RW_BUFFER:
        case DESCRIPTOR_TYPE_BUFFER: {
          // If we're trying to bind a buffer with an mCount > 1, it means we're binding many descriptors into an argument buffer.
          if (descriptorData->mCount > 1) {
            BindArgumentBuffer(pCmd, node, descriptorInfo, descriptorData);
          } else {
            Metal::Buffer *buffer = (Metal::Buffer *) descriptorData->ppBuffers[0];
            size_t offset = (descriptorData->ppBuffers[0]->mPositionInHeap +
                (descriptorData->pOffsets ? descriptorData->pOffsets[0] : 0));

            if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
              [pCmd->mtlRenderEncoder setVertexBuffer:buffer->mtlBuffer
                                               offset:offset
                                              atIndex:descriptorInfo->mDesc.reg];
            }
            if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
              [pCmd->mtlRenderEncoder setFragmentBuffer:buffer->mtlBuffer
                                                 offset:offset
                                                atIndex:descriptorInfo->mDesc.reg];
            }
            if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
              [pCmd->mtlComputeEncoder setBuffer:buffer->mtlBuffer
                                          offset:offset
                                         atIndex:descriptorInfo->mDesc.reg];
            }
          }
          break;
        }
        default: break;
      }
      node.pBoundDescriptors[i] = true;
    }
  }

  // We need to bind static samplers manually since Metal API has no concept of static samplers
  if (!node.mBoundStaticSamplers) {
    node.mBoundStaticSamplers = true;

    for (uint32_t i = 0; i < pRootSignature->mStaticSamplerCount; ++i) {
      ShaderStage usedStagesMask = pRootSignature->pStaticSamplerStages[i];
      Metal::Sampler *pSampler = (Metal::Sampler *) pRootSignature->ppStaticSamplers[i];
      uint32_t reg = pRootSignature->pStaticSamplerSlots[i];
      if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
        [pCmd->mtlRenderEncoder setVertexSamplerState:pSampler->mtlSamplerState atIndex:reg];
      }
      if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
        [pCmd->mtlRenderEncoder setFragmentSamplerState:pSampler->mtlSamplerState atIndex:reg];
      }
      if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
        [pCmd->mtlComputeEncoder setSamplerState:pSampler->mtlSamplerState atIndex:reg];
      }
    }
  }
}

void CmdBindLocalDescriptors(Cmd *pCmd,
                             DescriptorBinder *pDescriptorBinder,
                             RootSignature *pRootSignature,
                             uint32_t numDescriptors,
                             DescriptorData *pDescParams) {
  ASSERT(pCmd);
  ASSERT(pRootSignature);

  Renderer *pRenderer = (Renderer *) pCmd->pRenderer;

  ResetBoundResources(pRenderer, pDescriptorBinder, pRootSignature);
  DescriptorBinderNode& node = pDescriptorBinder->mRootSignatureNodes[pRootSignature];

  // Compare the currently bound root signature with the root signature of the descriptor manager
  // If these values dont match, we must bind the root signature of the descriptor manager
  // If the values match, no op is required

  // Loop through input params to check for new data
  for (uint32_t paramIdx = 0; paramIdx < numDescriptors; ++paramIdx) {
    const DescriptorData *pParam = &pDescParams[paramIdx];
    ASSERT(pParam);
    if (!pParam->pName) {
      LOGERRORF("Name of Descriptor at index (%u) is NULL", paramIdx);
      return;
    }

    uint32_t hash = tinystl::hash(pParam->pName);
    uint32_t descIndex = -1;
    const DescriptorInfo *pDesc = GetDescriptor(pRootSignature, pParam->pName, &descIndex);
    if (!pDesc) {
      continue;
    }

    const uint32_t arrayCount = Math_MaxU32(1U, pParam->mCount);

    // Replace the default DescriptorData by the new data pased into this function.
    node.pDescriptorDataArray[descIndex].pName = pParam->pName;
    node.pDescriptorDataArray[descIndex].mCount = arrayCount;
    node.pDescriptorDataArray[descIndex].pOffsets = pParam->pOffsets;
    switch (pDesc->mDesc.type) {
      case DESCRIPTOR_TYPE_RW_TEXTURE:
      case DESCRIPTOR_TYPE_TEXTURE:
        if (!pParam->ppTextures) {
          LOGERRORF("Texture descriptor (%s) is NULL", pParam->pName);
          return;
        }
        node.pDescriptorDataArray[descIndex].ppTextures = pParam->ppTextures;
        break;
      case DESCRIPTOR_TYPE_SAMPLER:
        if (!pParam->ppSamplers) {
          LOGERRORF("Sampler descriptor (%s) is NULL", pParam->pName);
          return;
        }
        node.pDescriptorDataArray[descIndex].ppSamplers = pParam->ppSamplers;
        break;
      case DESCRIPTOR_TYPE_ROOT_CONSTANT:
        if (!pParam->pRootConstant) {
          LOGERRORF("RootConstant array (%s) is NULL", pParam->pName);
          return;
        }
        node.pDescriptorDataArray[descIndex].pRootConstant = pParam->pRootConstant;
        break;
      case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      case DESCRIPTOR_TYPE_RW_BUFFER:
      case DESCRIPTOR_TYPE_BUFFER:
        if (!pParam->ppBuffers) {
          LOGERRORF("Buffer descriptor (%s) is NULL", pParam->pName);
          return;
        }
        node.pDescriptorDataArray[descIndex].ppBuffers = pParam->ppBuffers;

        // In case we're binding an argument buffer, signal that we need to re-encode the resources into the buffer.
        if (arrayCount > 1 && node.mArgumentBuffers.find(hash).node) {
          node.mArgumentBuffers[hash].second = true;
        }

        break;
      default: break;
    }

    // Mark this descriptor as unbound, so it's values are updated.
    node.pBoundDescriptors[descIndex] = false;
  }

  // Bind all the unbound root signature descriptors.
  for (uint32_t i = 0; i < pRootSignature->mDescriptorCount; ++i) {
    const DescriptorInfo *descriptorInfo = &pRootSignature->pDescriptors[i];
    const DescriptorData *descriptorData = &node.pDescriptorDataArray[i];

    if (!node.pBoundDescriptors[i]) {
      ShaderStage usedStagesMask = descriptorInfo->mDesc.used_stages;
      switch (descriptorInfo->mDesc.type) {
        case DESCRIPTOR_TYPE_RW_TEXTURE: {
          uint32_t textureCount = Math_MaxU32(1U, descriptorData->mCount);

          for (uint32_t j = 0; j < textureCount; j++) {
            if (!descriptorData->ppTextures[j] || !((Texture *) descriptorData->ppTextures[j])->mtlTexture) {
              LOGERRORF("RW Texture descriptor (%s) at array index (%u) is NULL", descriptorData->pName, j);
              return;
            }
            Metal::Texture *texture = (Metal::Texture *) descriptorData->ppTextures[j];

            if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
              [pCmd->mtlRenderEncoder setVertexTexture:texture->pMtlUAVDescriptors[descriptorData->mUAVMipSlice] atIndex:
                  descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
              [pCmd->mtlRenderEncoder setFragmentTexture:texture->pMtlUAVDescriptors[descriptorData->mUAVMipSlice] atIndex:
                  descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
              [pCmd->mtlComputeEncoder setTexture:texture->pMtlUAVDescriptors[descriptorData->mUAVMipSlice] atIndex:
                  descriptorInfo->mDesc.reg + j];
            }
          }
          break;
        }
        case DESCRIPTOR_TYPE_TEXTURE: {
          uint32_t textureCount = Math_MaxU32(1U, descriptorData->mCount);
          for (uint32_t j = 0; j < textureCount; j++) {
            if (!descriptorData->ppTextures[j] || !((Metal::Texture *) descriptorData->ppTextures[j])->mtlTexture) {
              LOGERRORF("Texture descriptor (%s) at array index (%u) is NULL", descriptorData->pName, j);
              return;
            }
            Metal::Texture *texture = (Metal::Texture *) descriptorData->ppTextures[j];

            if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
              [pCmd->mtlRenderEncoder setVertexTexture:texture->mtlTexture atIndex:
                  descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
              [pCmd->mtlRenderEncoder setFragmentTexture:texture->mtlTexture atIndex:
                  descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
              [pCmd->mtlComputeEncoder setTexture:texture->mtlTexture atIndex:
                  descriptorInfo->mDesc.reg + j];
            }
          }
          break;
        }
        case DESCRIPTOR_TYPE_SAMPLER: {
          uint32_t samplerCount = Math_MaxI32(1U, descriptorData->mCount);
          for (uint32_t j = 0; j < samplerCount; j++) {
            if (!descriptorData->ppSamplers[j]
                || !((Metal::Sampler *) descriptorData->ppSamplers[j])->mtlSamplerState) {
              LOGERRORF("Texture descriptor (%s) at array index (%u) is NULL", descriptorData->pName, j);
              return;
            }

            Metal::Sampler *sampler = (Metal::Sampler *) descriptorData->ppSamplers[j];

            if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
              [pCmd->mtlRenderEncoder setVertexSamplerState:sampler->mtlSamplerState atIndex:
                  descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
              [pCmd->mtlRenderEncoder setFragmentSamplerState:sampler->mtlSamplerState atIndex:
                  descriptorInfo->mDesc.reg + j];
            }
            if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
              [pCmd->mtlComputeEncoder setSamplerState:sampler->mtlSamplerState atIndex:
                  descriptorInfo->mDesc.reg + j];
            }
          }
          break;
        }
        case DESCRIPTOR_TYPE_ROOT_CONSTANT:
          if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
            [pCmd->mtlRenderEncoder setVertexBytes:descriptorData->pRootConstant length:descriptorInfo->mDesc.size atIndex:descriptorInfo->mDesc.reg];
          }
          if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
            [pCmd->mtlRenderEncoder setFragmentBytes:descriptorData->pRootConstant length:descriptorInfo->mDesc.size atIndex:descriptorInfo->mDesc.reg];
          }
          if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
            [pCmd->mtlComputeEncoder setBytes:descriptorData->pRootConstant length:descriptorInfo->mDesc.size atIndex:descriptorInfo->mDesc.reg];
          }
          break;
        case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case DESCRIPTOR_TYPE_RW_BUFFER:
        case DESCRIPTOR_TYPE_BUFFER: {
          // If we're trying to bind a buffer with an mCount > 1, it means we're binding many descriptors into an argument buffer.
          if (descriptorData->mCount > 1) {
            BindArgumentBuffer(pCmd, node, descriptorInfo, descriptorData);
          } else {
            Metal::Buffer *buffer = (Metal::Buffer *) descriptorData->ppBuffers[0];
            size_t offset = buffer->mPositionInHeap + (descriptorData->pOffsets ? descriptorData->pOffsets[0] : 0);

            if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
              [pCmd->mtlRenderEncoder
                  setVertexBuffer:buffer->mtlBuffer
                           offset:offset
                          atIndex:descriptorInfo->mDesc.reg];
            }
            if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {

              [pCmd->mtlRenderEncoder
                  setFragmentBuffer:buffer->mtlBuffer
                             offset:offset
                            atIndex:descriptorInfo->mDesc.reg];
            }
            if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
              [pCmd->mtlComputeEncoder
                  setBuffer:buffer->mtlBuffer
                     offset:offset
                    atIndex:descriptorInfo->mDesc.reg];
            }
          }
          break;
        }
        default: break;
      }
      node.pBoundDescriptors[i] = true;
    }
  }

  // We need to bind static samplers manually since Metal API has no concept of static samplers
  if (!node.mBoundStaticSamplers) {
    node.mBoundStaticSamplers = true;

    for (uint32_t i = 0; i < pRootSignature->mStaticSamplerCount; ++i) {
      ShaderStage usedStagesMask = pRootSignature->pStaticSamplerStages[i];
      Sampler *pSampler = pRootSignature->ppStaticSamplers[i];
      uint32_t reg = pRootSignature->pStaticSamplerSlots[i];
      if ((usedStagesMask & SHADER_STAGE_VERT) != 0) {
        [pCmd->mtlRenderEncoder
            setVertexSamplerState:pSampler->mtlSamplerState
                          atIndex:reg];
      }
      if ((usedStagesMask & SHADER_STAGE_FRAG) != 0) {
        [pCmd->mtlRenderEncoder
            setFragmentSamplerState:pSampler->mtlSamplerState
                            atIndex:reg];
      }
      if ((usedStagesMask & SHADER_STAGE_COMP) != 0) {
        [pCmd->mtlComputeEncoder
            setSamplerState:pSampler->mtlSamplerState
                    atIndex:reg];
      }
    }
  }
}

void AddDescriptorBinder(
    Renderer *pRenderer,
    uint32_t gpuIndex,
    uint32_t numDescriptorDescs,
    const DescriptorBinderDesc *pDescs,
    DescriptorBinder **ppDescriptorBinder) {
  // TODO Deano
  DescriptorBinder *descriptorBinder = new DescriptorBinder();

  for (uint32_t idesc = 0; idesc < numDescriptorDescs; idesc++) {
    const DescriptorBinderDesc *pDesc = pDescs + idesc;
    RootSignature *pRootSignature = (Metal::RootSignature*) pDesc->pRootSignature;

    tinystl::unordered_map<const RootSignature *, DescriptorBinderNode>::const_iterator
        it = descriptorBinder->mRootSignatureNodes.find(pRootSignature);
    if (it != descriptorBinder->mRootSignatureNodes.end()) {
      continue;
    }  // we only need to store data per unique root signature. It is safe to skip repeated ones in Metal renderer.

    DescriptorBinderNode node = {};

    // Allocate enough memory to hold all the necessary data for all the descriptors of this rootSignature.
    node.pDescriptorDataArray =
        (DescriptorData *) calloc(pRootSignature->mDescriptorCount, sizeof(DescriptorData));
    node.pBoundDescriptors = (bool *) calloc(pRootSignature->mDescriptorCount, sizeof(bool));

    // Fill all the descriptors in the rootSignature with their default values.
    for (uint32_t i = 0; i < pRootSignature->mDescriptorCount; ++i) {
      DescriptorInfo *descriptorInfo = &pRootSignature->pDescriptors[i];

      // Create a DescriptorData structure for a default resource.
      node.pDescriptorDataArray[i].pName = "";
      node.pDescriptorDataArray[i].mCount = 1;
      node.pDescriptorDataArray[i].pOffsets = NULL;

      // Metal requires that the bound textures match the texture type present in the shader.
      Texture **ppDefaultTexture = nil;
      if (descriptorInfo->mDesc.type == DESCRIPTOR_TYPE_RW_TEXTURE
          || descriptorInfo->mDesc.type == DESCRIPTOR_TYPE_TEXTURE) {
        switch ((MTLTextureType) descriptorInfo->mDesc.backend.mtlTextureType) {
          case MTLTextureType1D: ppDefaultTexture = &pRenderer->pDefault1DTexture;
            break;
          case MTLTextureType1DArray: ppDefaultTexture = &pRenderer->pDefault1DTextureArray;
            break;
          case MTLTextureType2D: ppDefaultTexture = &pRenderer->pDefault2DTexture;
            break;
          case MTLTextureType2DArray: ppDefaultTexture = &pRenderer->pDefault2DTextureArray;
            break;
          case MTLTextureType3D: ppDefaultTexture = &pRenderer->pDefault3DTexture;
            break;
          case MTLTextureTypeCube: ppDefaultTexture = &pRenderer->pDefaultCubeTexture;
            break;
          case MTLTextureTypeCubeArray: ppDefaultTexture = &pRenderer->pDefaultCubeTextureArray;
            break;
          default: break;
        }
      }

      // Point to the appropiate default resource depending of the type of descriptor.
      switch (descriptorInfo->mDesc.type) {
        case DESCRIPTOR_TYPE_RW_TEXTURE:
        case DESCRIPTOR_TYPE_TEXTURE: node.pDescriptorDataArray[i].ppTextures = (TheForge_Texture**)ppDefaultTexture;
          break;
        case DESCRIPTOR_TYPE_SAMPLER: node.pDescriptorDataArray[i].ppSamplers = (TheForge_Sampler**)&pRenderer->pDefaultSampler;
          break;
        case DESCRIPTOR_TYPE_ROOT_CONSTANT:
          // Default root constants can be bound the same way buffers are.
          node.pDescriptorDataArray[i].pRootConstant = &pRenderer->pDefaultBuffer;
          break;
        case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case DESCRIPTOR_TYPE_RW_BUFFER:
        case DESCRIPTOR_TYPE_BUFFER: {
          node.pDescriptorDataArray[i].ppBuffers = (TheForge_Buffer**)&pRenderer->pDefaultBuffer;
          break;
          default: break;
        }
      }
    }
    descriptorBinder->mRootSignatureNodes.insert({pRootSignature, node});
  }
  *ppDescriptorBinder = descriptorBinder;
}

void RemoveDescriptorBinder(Renderer *pRenderer, DescriptorBinder *pDescriptorBinder) {
  for (tinystl::unordered_hash_node<const RootSignature *, DescriptorBinderNode>
        & node : pDescriptorBinder->mRootSignatureNodes) {
    node.second.mArgumentBuffers.clear();
    free(node.second.pDescriptorDataArray);
    free(node.second.pBoundDescriptors);
  }
  delete(pDescriptorBinder);
}

} } // end namespace TheForge::Metal