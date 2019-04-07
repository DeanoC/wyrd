#import <simd/simd.h>
#import <MetalKit/MetalKit.h>

#include "core/core.h"
#include "math/math.h"
#include "os/thread.hpp"
#include "utils.hpp"
#include "image/image.h"
#include "renderer.hpp"

namespace TheForge { namespace Metal { namespace Util {
static MTLPixelFormat PixelFormatConvertor(Image_Format const fmt) {
  switch (fmt) {
    default: return MTLPixelFormatInvalid;
//  case Image_Format_A8 : return MTLPixelFormatA8Unorm;
    case Image_Format_R8_UNORM: return MTLPixelFormatR8Unorm;
    case Image_Format_R8_SNORM: return MTLPixelFormatR8Snorm;
    case Image_Format_R8_UINT: return MTLPixelFormatR8Uint;
    case Image_Format_R8_SINT: return MTLPixelFormatR8Sint;
    case Image_Format_R16_UNORM: return MTLPixelFormatR16Unorm;
    case Image_Format_R16_SNORM: return MTLPixelFormatR16Snorm;
    case Image_Format_R16_UINT: return MTLPixelFormatR16Uint;
    case Image_Format_R16_SINT: return MTLPixelFormatR16Sint;
    case Image_Format_R16_SFLOAT: return MTLPixelFormatR16Float;
    case Image_Format_R8G8_UNORM: return MTLPixelFormatRG8Unorm;
    case Image_Format_R8G8_SNORM: return MTLPixelFormatRG8Snorm;
    case Image_Format_R8G8_UINT: return MTLPixelFormatRG8Uint;
    case Image_Format_R8G8_SINT: return MTLPixelFormatRG8Sint;
    case Image_Format_R32_UINT: return MTLPixelFormatR32Uint;
    case Image_Format_R32_SINT: return MTLPixelFormatR32Sint;
    case Image_Format_R32_SFLOAT: return MTLPixelFormatR32Float;
    case Image_Format_R16G16_UNORM: return MTLPixelFormatRG16Unorm;
    case Image_Format_R16G16_SNORM: return MTLPixelFormatRG16Snorm;
    case Image_Format_R16G16_UINT: return MTLPixelFormatRG16Uint;
    case Image_Format_R16G16_SINT: return MTLPixelFormatRG16Sint;
    case Image_Format_R16G16_SFLOAT: return MTLPixelFormatRG16Float;
    case Image_Format_R8G8B8A8_UNORM: return MTLPixelFormatRGBA8Unorm;
    case Image_Format_R8G8B8A8_SRGB: return MTLPixelFormatRGBA8Unorm_sRGB;
    case Image_Format_R8G8B8A8_SNORM: return MTLPixelFormatRGBA8Snorm;
    case Image_Format_R8G8B8A8_UINT: return MTLPixelFormatRGBA8Uint;
    case Image_Format_R8G8B8A8_SINT: return MTLPixelFormatRGBA8Sint;
    case Image_Format_B8G8R8A8_UNORM: return MTLPixelFormatBGRA8Unorm;
    case Image_Format_B8G8R8A8_SRGB: return MTLPixelFormatBGRA8Unorm_sRGB;
    case Image_Format_A2R10G10B10_UNORM_PACK32: return MTLPixelFormatRGB10A2Unorm;
    case Image_Format_A2R10G10B10_UINT_PACK32: return MTLPixelFormatRGB10A2Uint;
    case Image_Format_B10G11R11_UFLOAT_PACK32: return MTLPixelFormatRG11B10Float;
    case Image_Format_E5B9G9R9_UFLOAT_PACK32: return MTLPixelFormatRGB9E5Float;
    case Image_Format_A2B10G10R10_UNORM_PACK32: return MTLPixelFormatBGR10A2Unorm;
//    case Image_Format_R8_UNORM: return MTLPixelFormatBGR10_XR;
//    case Image_Format_R8_SRGB: return MTLPixelFormatBGR10_XR_sRGB;
    case Image_Format_R32G32_UINT: return MTLPixelFormatRG32Uint;
    case Image_Format_R32G32_SINT: return MTLPixelFormatRG32Sint;
    case Image_Format_R32G32_SFLOAT: return MTLPixelFormatRG32Float;
    case Image_Format_R16G16B16A16_UNORM: return MTLPixelFormatRGBA16Unorm;
    case Image_Format_R16G16B16A16_SNORM: return MTLPixelFormatRGBA16Snorm;
    case Image_Format_R16G16B16A16_UINT: return MTLPixelFormatRGBA16Uint;
    case Image_Format_R16G16B16A16_SINT: return MTLPixelFormatRGBA16Sint;
    case Image_Format_R16G16B16A16_SFLOAT: return MTLPixelFormatRGBA16Float;
//    case Image_Format_ : return MTLPixelFormatBGRA10_XR;
//    case Image_Format_ : return MTLPixelFormatBGRA10_XR_sRGB;
    case Image_Format_R32G32B32A32_UINT: return MTLPixelFormatRGBA32Uint;
    case Image_Format_R32G32B32A32_SINT: return MTLPixelFormatRGBA32Sint;
    case Image_Format_R32G32B32A32_SFLOAT: return MTLPixelFormatRGBA32Float;
    case Image_Format_D32_SFLOAT : return MTLPixelFormatDepth32Float;
    case Image_Format_S8_UINT : return MTLPixelFormatStencil8;
    case Image_Format_D32_SFLOAT_S8_UINT : return MTLPixelFormatDepth32Float_Stencil8;
#ifndef TARGET_IOS
    case Image_Format_BC1_RGBA_UNORM_BLOCK: return MTLPixelFormatBC1_RGBA;
    case Image_Format_BC1_RGBA_SRGB_BLOCK: return MTLPixelFormatBC1_RGBA_sRGB;
    case Image_Format_BC2_UNORM_BLOCK: return MTLPixelFormatBC2_RGBA;
    case Image_Format_BC2_SRGB_BLOCK: return MTLPixelFormatBC2_RGBA_sRGB;
    case Image_Format_BC3_UNORM_BLOCK: return MTLPixelFormatBC3_RGBA;
    case Image_Format_BC3_SRGB_BLOCK : return MTLPixelFormatBC3_RGBA_sRGB;
    case Image_Format_BC4_UNORM_BLOCK : return MTLPixelFormatBC4_RUnorm;
    case Image_Format_BC4_SNORM_BLOCK : return MTLPixelFormatBC4_RSnorm;
    case Image_Format_BC5_UNORM_BLOCK : return MTLPixelFormatBC5_RGUnorm;
    case Image_Format_BC5_SNORM_BLOCK : return MTLPixelFormatBC5_RGSnorm;
    case Image_Format_BC6H_SFLOAT_BLOCK : return MTLPixelFormatBC6H_RGBFloat;
    case Image_Format_BC6H_UFLOAT_BLOCK : return MTLPixelFormatBC6H_RGBUfloat;
    case Image_Format_BC7_UNORM_BLOCK : return MTLPixelFormatBC7_RGBAUnorm;
    case Image_Format_BC7_SRGB_BLOCK : return MTLPixelFormatBC7_RGBAUnorm_sRGB;
    case Image_Format_D16_UNORM : return MTLPixelFormatDepth16Unorm;
    case Image_Format_D24_UNORM_S8_UINT : return MTLPixelFormatDepth24Unorm_Stencil8;
#else
    case Image_Format_R8_SRGB: return MTLPixelFormatR8Unorm_sRGB;
    case Image_Format_R8G8_SRGB: return MTLPixelFormatRG8Unorm_sRGB;
    case Image_Format_B5G6R5_UNORM_PACK16: return MTLPixelFormatB5G6R5Unorm;
    case Image_Format_A1R5G5B5_UNORM_PACK16: return MTLPixelFormatA1BGR5Unorm;
    case Image_Format_R4G4B4A4_UNORM_PACK16: return MTLPixelFormatABGR4Unorm;
    case Image_Format_B5G5R5A1_UNORM_PACK16: return MTLPixelFormatBGR5A1Unorm;
    case Image_Format_PVR_2BPP_BLOCK : return MTLPixelFormatPVRTC_RGB_2BPP;
    case Image_Format_PVR_2BPP_SRGB_BLOCK : return MTLPixelFormatPVRTC_RGB_2BPP_sRGB;
    case Image_Format_PVR_4BPP_BLOCK : return MTLPixelFormatPVRTC_RGB_4BPP;
    case Image_Format_PVR_4BPP_SRGB_BLOCK : return MTLPixelFormatPVRTC_RGB_4BPP_sRGB;

//    case Image_Format_PV : return MTLPixelFormatPVRTC_RGBA_2BPP;
//    case Image_Format_ : return MTLPixelFormatPVRTC_RGBA_2BPP_sRGB;
//    case Image_Format_ : return MTLPixelFormatPVRTC_RGBA_4BPP;
//    case Image_Format_ : return MTLPixelFormatPVRTC_RGBA_4BPP_sRGB;
/*
    case Image_Format_ : return MTLPixelFormatEAC_R11Unorm;
    case Image_Format_ : return MTLPixelFormatEAC_R11Snorm;
    case Image_Format_ : return MTLPixelFormatEAC_RG11Unorm;
    case Image_Format_ : return MTLPixelFormatEAC_RG11Snorm;
    case Image_Format_ : return MTLPixelFormatEAC_RGBA8;
    case Image_Format_ : return MTLPixelFormatEAC_RGBA8_sRGB;
    case Image_Format_ : return MTLPixelFormatETC2_RGB8;
    case Image_Format_ : return MTLPixelFormatETC2_RGB8_sRGB;
    case Image_Format_ : return MTLPixelFormatETC2_RGB8A1;
    case Image_Format_ : return MTLPixelFormatETC2_RGB8A1_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_4x4_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_5x4_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_5x5_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_6x5_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_6x6_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_8x5_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_8x6_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_8x8_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_10x5_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_10x6_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_10x8_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_10x10_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_12x10_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_12x12_sRGB;
    case Image_Format_ : return MTLPixelFormatASTC_4x4_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_5x4_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_5x5_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_6x5_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_6x6_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_8x5_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_8x6_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_8x8_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_10x5_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_10x6_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_10x8_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_10x10_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_12x10_LDR;
    case Image_Format_ : return MTLPixelFormatASTC_12x12_LDR;*/
#endif
//    case Image_Format_ : return MTLPixelFormatGBGR422;
//    case Image_Format_ : return MTLPixelFormatBGRG422;
//    case Image_Format_X : return MTLPixelFormatX32_Stencil8;
//    case Image_Format_X : return MTLPixelFormatX24_Stencil8;
  }
}

uint32_t CalculateVertexLayoutStride(const VertexLayout *pVertexLayout) {
  ASSERT(pVertexLayout);

  uint32_t result = 0;
  for (uint32_t i = 0; i < pVertexLayout->mAttribCount; ++i) {
    result += Image_Format_BitWidth(pVertexLayout->mAttribs[i].mFormat) / 8;
  }
  return result;
}

bool IsImageFormatSupported(Image_Format format) {
  return PixelFormatConvertor(format) != MTLPixelFormatInvalid;
}

uint64_t PthreadToUint64(const pthread_t& value) {
  uint64_t threadId = 0;
  memcpy(&threadId, &value, sizeof(value));
  return threadId;
}

MTLPixelFormat ToMtlPixelFormat(const Image_Format format) {
  return PixelFormatConvertor(format);
}

bool IsMtlDepthPixelFormat(const MTLPixelFormat& format) {
  return format == MTLPixelFormatDepth32Float || format == MTLPixelFormatDepth32Float_Stencil8
#ifndef TARGET_IOS
      || format == MTLPixelFormatDepth16Unorm || format == MTLPixelFormatDepth24Unorm_Stencil8
#endif
      ;
}

bool IsMtlCompressedPixelFormat(const MTLPixelFormat& format) {
#ifndef TARGET_IOS
  return format == MTLPixelFormatBC1_RGBA || format == MTLPixelFormatBC1_RGBA_sRGB || format == MTLPixelFormatBC2_RGBA
      ||
          format == MTLPixelFormatBC2_RGBA_sRGB || format == MTLPixelFormatBC3_RGBA
      || format == MTLPixelFormatBC3_RGBA_sRGB ||
      format == MTLPixelFormatBC4_RUnorm || format == MTLPixelFormatBC4_RSnorm || format == MTLPixelFormatBC5_RGUnorm ||
      format == MTLPixelFormatBC5_RGSnorm || format == MTLPixelFormatBC6H_RGBFloat
      || format == MTLPixelFormatBC6H_RGBUfloat ||
      format == MTLPixelFormatBC7_RGBAUnorm || format == MTLPixelFormatBC7_RGBAUnorm_sRGB;
#else
  // Note: BC texture formats are not supported on iOS.
    return 	format == MTLPixelFormatPVRTC_RGB_2BPP ||
            format == MTLPixelFormatPVRTC_RGB_2BPP_sRGB ||
            format == MTLPixelFormatPVRTC_RGB_4BPP ||
            format == MTLPixelFormatPVRTC_RGB_4BPP_sRGB ||
            format == MTLPixelFormatPVRTC_RGBA_2BPP ||
            format == MTLPixelFormatPVRTC_RGBA_2BPP_sRGB ||
            format == MTLPixelFormatPVRTC_RGBA_4BPP ||
            format == MTLPixelFormatPVRTC_RGBA_4BPP_sRGB;
#endif
}

MTLVertexFormat ToMtlVertexFormat(const Image_Format format) {
  switch (format) {
    case Image_Format_R8_UNORM: return MTLVertexFormatUCharNormalized;
    case Image_Format_R8G8_UNORM: return MTLVertexFormatUChar2Normalized;
    case Image_Format_R8G8B8_UNORM: return MTLVertexFormatUChar3Normalized;
    case Image_Format_R8G8B8A8_UNORM: return MTLVertexFormatUChar4Normalized;

    case Image_Format_R8_SNORM: return MTLVertexFormatCharNormalized;
    case Image_Format_R8G8_SNORM: return MTLVertexFormatChar2Normalized;
    case Image_Format_R8G8B8_SNORM: return MTLVertexFormatChar3Normalized;
    case Image_Format_R8G8B8A8_SNORM: return MTLVertexFormatChar4Normalized;

    case Image_Format_R8_UINT: return MTLVertexFormatUChar;
    case Image_Format_R8G8_UINT: return MTLVertexFormatUChar2;
    case Image_Format_R8G8B8_UINT: return MTLVertexFormatUChar3;
    case Image_Format_R8G8B8A8_UINT: return MTLVertexFormatUChar4;

    case Image_Format_R8_SINT: return MTLVertexFormatChar;
    case Image_Format_R8G8_SINT: return MTLVertexFormatChar2;
    case Image_Format_R8G8B8_SINT: return MTLVertexFormatChar3;
    case Image_Format_R8G8B8A8_SINT: return MTLVertexFormatChar4;

    case Image_Format_R16_UNORM: return MTLVertexFormatUShortNormalized;
    case Image_Format_R16G16_UNORM: return MTLVertexFormatUShort2Normalized;
    case Image_Format_R16G16B16_UNORM: return MTLVertexFormatUShort3Normalized;
    case Image_Format_R16G16B16A16_UNORM: return MTLVertexFormatUShort4Normalized;

    case Image_Format_R16_SNORM: return MTLVertexFormatShortNormalized;
    case Image_Format_R16G16_SNORM: return MTLVertexFormatShort2Normalized;
    case Image_Format_R16G16B16_SNORM: return MTLVertexFormatShort3Normalized;
    case Image_Format_R16G16B16A16_SNORM: return MTLVertexFormatShort4Normalized;

    case Image_Format_R16_UINT: return MTLVertexFormatUShort;
    case Image_Format_R16G16_UINT: return MTLVertexFormatUShort2;
    case Image_Format_R16G16B16_UINT: return MTLVertexFormatUShort3;
    case Image_Format_R16G16B16A16_UINT: return MTLVertexFormatUShort4;

    case Image_Format_R16_SINT: return MTLVertexFormatShort;
    case Image_Format_R16G16_SINT: return MTLVertexFormatShort2;
    case Image_Format_R16G16B16_SINT: return MTLVertexFormatShort3;
    case Image_Format_R16G16B16A16_SINT: return MTLVertexFormatShort4;

    case Image_Format_R32_UINT: return MTLVertexFormatUInt;
    case Image_Format_R32G32_UINT: return MTLVertexFormatUInt2;
    case Image_Format_R32G32B32_UINT: return MTLVertexFormatUInt3;
    case Image_Format_R32G32B32A32_UINT: return MTLVertexFormatUInt4;

    case Image_Format_R32_SINT: return MTLVertexFormatInt;
    case Image_Format_R32G32_SINT: return MTLVertexFormatInt2;
    case Image_Format_R32G32B32_SINT: return MTLVertexFormatInt3;
    case Image_Format_R32G32B32A32_SINT: return MTLVertexFormatInt4;

    case Image_Format_R16_SFLOAT: return MTLVertexFormatHalf;
    case Image_Format_R16G16_SFLOAT: return MTLVertexFormatHalf2;
    case Image_Format_R16G16B16_SFLOAT: return MTLVertexFormatHalf3;
    case Image_Format_R16G16B16A16_SFLOAT: return MTLVertexFormatHalf4;

    case Image_Format_R32_SFLOAT: return MTLVertexFormatFloat;
    case Image_Format_R32G32_SFLOAT: return MTLVertexFormatFloat2;
    case Image_Format_R32G32B32_SFLOAT: return MTLVertexFormatFloat3;
    case Image_Format_R32G32B32A32_SFLOAT: return MTLVertexFormatFloat4;

    case Image_Format_A2B10G10R10_UNORM_PACK32: return MTLVertexFormatUInt1010102Normalized;
    default: return MTLVertexFormatInvalid;
  }
}

MTLLoadAction ToMtlLoadAction(const LoadActionType& loadActionType) {
  if (loadActionType == LOAD_ACTION_DONTCARE) {
    return MTLLoadActionDontCare;
  } else if (loadActionType == LOAD_ACTION_LOAD) {
    return MTLLoadActionLoad;
  } else {
    return MTLLoadActionClear;
  }
}

void BindArgumentBuffer(Cmd *pCmd,
                        DescriptorManager *pManager,
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
    DescriptorManager::ArgumentBuffers::iterator jt = pManager->mArgumentBuffers.find(hash);

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

      pManager->mArgumentBuffers[hash] = {argumentBuffer, true};
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

    pManager->mArgumentBuffers[hash].second = false;
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

void EndCurrentEncoders(Cmd *pCmd) {
  if (pCmd->mtlRenderEncoder != nil) {
    [pCmd->mtlRenderEncoder endEncoding];
    pCmd->mtlRenderEncoder = nil;
  }
  if (pCmd->mtlComputeEncoder != nil) {
    [pCmd->mtlComputeEncoder endEncoding];
    pCmd->mtlComputeEncoder = nil;
  }
  if (pCmd->mtlBlitEncoder != nil) {
    [pCmd->mtlBlitEncoder endEncoding];
    pCmd->mtlBlitEncoder = nil;
  }
}

bool SyncEncoders(Cmd *pCmd, const CmdPoolType& newEncoderType) {
  if (newEncoderType != CMD_POOL_DIRECT && pCmd->mtlRenderEncoder != nil) {
    [pCmd->mtlRenderEncoder updateFence:pCmd->mtlEncoderFence afterStages:MTLRenderStageFragment];
    return true;
  }
  if (newEncoderType != CMD_POOL_COMPUTE && pCmd->mtlComputeEncoder != nil) {
    [pCmd->mtlComputeEncoder updateFence:pCmd->mtlEncoderFence];
    return true;
  }
  if (newEncoderType != CMD_POOL_COPY && pCmd->mtlBlitEncoder != nil) {
    [pCmd->mtlBlitEncoder updateFence:pCmd->mtlEncoderFence];
    return true;
  }
  return false;
}

bool IsCompatibleTextureView(const MTLTextureType& textureType, const MTLTextureType& subviewTye) {
  switch (textureType) {
    case MTLTextureType1D:
      if (subviewTye != MTLTextureType1D) {
        return false;
      }
      return true;
    case MTLTextureType2D:
      if (subviewTye != MTLTextureType2D && subviewTye != MTLTextureType2DArray) {
        return false;
      }
      return true;
    case MTLTextureType2DArray:
    case MTLTextureTypeCube:
    case MTLTextureTypeCubeArray:
      if (subviewTye != MTLTextureType2D && subviewTye != MTLTextureType2DArray && subviewTye != MTLTextureTypeCube &&
          subviewTye != MTLTextureTypeCubeArray) {
        return false;
      }
      return true;
    case MTLTextureType3D:
      if (subviewTye != MTLTextureType3D) {
        return false;
      }
      return true;
    default: return false;
  }

  return false;
}

}}} // end namespace TheForge::Metal::Util

// Proxy log callback
void InternalLog(LogType type, const char *msg, const char *component) {
  switch (type) {
    case LOG_TYPE_INFO: LOGINFOF("%s ( %s )", component, msg);
      break;
    case LOG_TYPE_WARN: LOGWARNINGF("%s ( %s )", component, msg);
      break;
    case LOG_TYPE_DEBUG: LOGDEBUGF("%s ( %s )", component, msg);
      break;
    case LOG_TYPE_ERROR: LOGERRORF("%s ( %s )", component, msg);
      break;
    default: break;
  }
}
