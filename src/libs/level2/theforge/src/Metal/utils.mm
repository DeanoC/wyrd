#import <simd/simd.h>
#import <MetalKit/MetalKit.h>

#include "core/core.h"
#include "math/math.h"
#include "os/thread.hpp"
#include "utils.hpp"
#include "../Image/image.h"
#include "renderer.hpp"

namespace TheForge { namespace Metal { namespace Util {
static MTLPixelFormat PixelFormatConvertor(ImageFormat const fmt) {
  switch(fmt) {
    case TheForge_IF_NONE: return MTLPixelFormatInvalid;

    case TheForge_IF_R8:    return MTLPixelFormatR8Unorm;
    case TheForge_IF_RG8:   return MTLPixelFormatRG8Unorm;
    case TheForge_IF_RGB8:  return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA8: return MTLPixelFormatRGBA8Unorm;
    case TheForge_IF_R16: return MTLPixelFormatR16Unorm;
    case TheForge_IF_RG16: return MTLPixelFormatRG16Unorm;
    case TheForge_IF_RGB16: return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA16: return MTLPixelFormatRGBA16Unorm;
    case TheForge_IF_R8S: return MTLPixelFormatR8Snorm;
    case TheForge_IF_RG8S: return MTLPixelFormatRG8Snorm;
    case TheForge_IF_RGB8S: return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA8S: return MTLPixelFormatRGBA8Snorm;
    case TheForge_IF_R16S:    return MTLPixelFormatR16Snorm;
    case TheForge_IF_RG16S:   return MTLPixelFormatRG16Snorm;
    case TheForge_IF_RGB16S:  return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA16S: return MTLPixelFormatRGBA16Snorm;
    case TheForge_IF_R16F:    return MTLPixelFormatR16Float;
    case TheForge_IF_RG16F:   return MTLPixelFormatRG16Float;
    case TheForge_IF_RGB16F:  return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA16F: return MTLPixelFormatRGBA16Float;
    case TheForge_IF_R32F:    return MTLPixelFormatR32Float;
    case TheForge_IF_RG32F:   return MTLPixelFormatRG32Float;
    case TheForge_IF_RGB32F:  return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA32F: return MTLPixelFormatRGBA32Float;
    case TheForge_IF_R16I:    return MTLPixelFormatR16Sint;
    case TheForge_IF_RG16I:   return MTLPixelFormatRG16Sint;
    case TheForge_IF_RGB16I:  return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA16I: return MTLPixelFormatRGBA16Sint;
    case TheForge_IF_R32I:    return MTLPixelFormatR32Sint;
    case TheForge_IF_RG32I:   return MTLPixelFormatRG32Sint;
    case TheForge_IF_RGB32I:  return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA32I: return MTLPixelFormatRGBA32Sint;
    case TheForge_IF_R16UI:   return MTLPixelFormatR16Uint;
    case TheForge_IF_RG16UI:  return MTLPixelFormatRG16Uint;
    case TheForge_IF_RGB16UI: return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA16UI:return MTLPixelFormatRGBA16Uint;
    case TheForge_IF_R32UI:   return MTLPixelFormatR32Uint;
    case TheForge_IF_RG32UI:  return MTLPixelFormatRG32Uint;
    case TheForge_IF_RGB32UI: return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA32UI:return MTLPixelFormatRGBA32Uint;


    case TheForge_IF_RGBE8: return MTLPixelFormatInvalid;
    case TheForge_IF_RGB9E5: return MTLPixelFormatRGB9E5Float;
    case TheForge_IF_RG11B10F: return MTLPixelFormatRG11B10Float;
    case TheForge_IF_RGB565: return MTLPixelFormatInvalid;
    case TheForge_IF_RGBA4: return MTLPixelFormatInvalid;
    case TheForge_IF_RGB10A2: return MTLPixelFormatRGB10A2Unorm;

    case TheForge_IF_D32F: return MTLPixelFormatDepth32Float;
#ifndef TARGET_IOS
    case TheForge_IF_D16: return MTLPixelFormatDepth16Unorm;
    case TheForge_IF_D24: return MTLPixelFormatDepth24Unorm_Stencil8;
    case TheForge_IF_D24S8: return MTLPixelFormatDepth24Unorm_Stencil8;
    case TheForge_IF_DXT1: return MTLPixelFormatBC1_RGBA;
    case TheForge_IF_DXT3: return MTLPixelFormatBC2_RGBA;
    case TheForge_IF_DXT5: return MTLPixelFormatBC3_RGBA;
    case TheForge_IF_ATI1N: return MTLPixelFormatBC4_RUnorm;
    case TheForge_IF_ATI2N: return MTLPixelFormatBC5_RGUnorm;
    case TheForge_IF_PVR_2BPP: return MTLPixelFormatInvalid;
    case TheForge_IF_PVR_2BPPA: return MTLPixelFormatInvalid;
    case TheForge_IF_PVR_4BPP: return MTLPixelFormatInvalid;
    case TheForge_IF_PVR_4BPPA: return MTLPixelFormatInvalid;
#else
    case TheForge_IF_D16: return MTLPixelFormatInvalid;
    case TheForge_IF_D24: return MTLPixelFormatInvalid;
    case TheForge_IF_D24S8: return MTLPixelFormatInvalid;
    case TheForge_IF_DXT1: return MTLPixelFormatInvalid;
    case TheForge_IF_DXT3: return MTLPixelFormatInvalid;
    case TheForge_IF_DXT5: return MTLPixelFormatInvalid;
    case TheForge_IF_ATI1N: return MTLPixelFormatInvalid;
    case TheForge_IF_ATI2N: return MTLPixelFormatInvalid;
    case TheForge_IF_PVR_2BPP: return MTLPixelFormatPVRTC_RGB_2BPP;
    case TheForge_IF_PVR_2BPPA: return MTLPixelFormatPVRTC_RGBA_2BPP;
    case TheForge_IF_PVR_4BPP: return MTLPixelFormatPVRTC_RGB_4BPP;
    case TheForge_IF_PVR_4BPPA: return MTLPixelFormatPVRTC_RGBA_4BPP;
#endif

    case TheForge_IF_ETC1: return MTLPixelFormatInvalid;
    case TheForge_IF_ATC : return MTLPixelFormatInvalid;
    case TheForge_IF_ATCA: return MTLPixelFormatInvalid;
    case TheForge_IF_ATCI: return MTLPixelFormatInvalid;
    case TheForge_IF_GNF_BC1: return MTLPixelFormatInvalid;
    case TheForge_IF_GNF_BC2: return MTLPixelFormatInvalid;
    case TheForge_IF_GNF_BC3: return MTLPixelFormatInvalid;
    case TheForge_IF_GNF_BC4: return MTLPixelFormatInvalid;
    case TheForge_IF_GNF_BC5: return MTLPixelFormatInvalid;
    case TheForge_IF_GNF_BC6: return MTLPixelFormatInvalid;
    case TheForge_IF_GNF_BC7: return MTLPixelFormatInvalid;
    case TheForge_IF_BGRA8: return MTLPixelFormatBGRA8Unorm;
    case TheForge_IF_X8D24PAX32: return MTLPixelFormatInvalid;
    case TheForge_IF_S8: return MTLPixelFormatStencil8;
    case TheForge_IF_D16S8: return MTLPixelFormatInvalid;
    case TheForge_IF_D32S8: return MTLPixelFormatDepth32Float_Stencil8;
#ifndef TARGET_IOS
    case TheForge_IF_PVR_2BPP_SRGB: return MTLPixelFormatInvalid;
    case TheForge_IF_PVR_2BPPA_SRGB: return MTLPixelFormatInvalid;
    case TheForge_IF_PVR_4BPP_SRGB: return MTLPixelFormatInvalid;
    case TheForge_IF_PVR_4BPPA_SRGB: return MTLPixelFormatInvalid;
#else
    // PVR formats
    case TheForge_IF_PVR_2BPP_SRGB: return MTLPixelFormatPVRTC_RGB_2BPP_sRGB;
    case TheForge_IF_PVR_2BPPA_SRGB: return MTLPixelFormatPVRTC_RGBA_2BPP_sRGB;
    case TheForge_IF_PVR_4BPP_SRGB: return MTLPixelFormatPVRTC_RGB_4BPP_sRGB;
    case TheForge_IF_PVR_4BPPA_SRGB: return MTLPixelFormatPVRTC_RGBA_4BPP_sRGB;
#endif

    default:
      ASSERT(false);
      return MTLPixelFormatInvalid;
  }
}

uint32_t CalculateVertexLayoutStride(const VertexLayout *pVertexLayout) {
  ASSERT(pVertexLayout);

  uint32_t result = 0;
  for (uint32_t i = 0; i < pVertexLayout->mAttribCount; ++i) {
    result += CalculateImageFormatStride(pVertexLayout->mAttribs[i].mFormat);
  }
  return result;
}

bool IsImageFormatSupported(ImageFormat format) {
  bool result = false;
  switch (format) {
    // 1 channel
    case R8: result = true;
      break;
    case R16: result = true;
      break;
    case R16F: result = true;
      break;
    case R32UI: result = true;
      break;
    case R32F: result = true;
      break;
    case D32S8: result = true;
      break;
      // 2 channel
    case RG8: result = true;
      break;
    case RG16: result = true;
      break;
    case RG16F: result = true;
      break;
    case RG32UI: result = true;
      break;
    case RG32F: result = true;
      break;
      // 3 channel
    case RGB8: result = true;
      break;
    case RGB16: result = true;
      break;
    case RGB16F: result = true;
      break;
    case RGB32UI: result = true;
      break;
    case RGB32F: result = true;
      break;
      // 4 channel
    case BGRA8: result = true;
      break;
    case RGBA16: result = true;
      break;
    case RGBA16F: result = true;
      break;
    case RGBA32UI: result = true;
      break;
    case RGBA32F: result = true;
      break;

    default: result = false;
      break;
  }
  return result;
}

uint64_t PthreadToUint64(const pthread_t& value) {
  uint64_t threadId = 0;
  memcpy(&threadId, &value, sizeof(value));
  return threadId;
}

MTLPixelFormat ToMtlPixelFormat(const ImageFormat format, const bool& srgb) {
  MTLPixelFormat result = PixelFormatConvertor(format);

  if (srgb) {
    if (result == MTLPixelFormatRGBA8Unorm) {
      result = MTLPixelFormatRGBA8Unorm_sRGB;
    } else if (result == MTLPixelFormatBGRA8Unorm) {
      result = MTLPixelFormatBGRA8Unorm_sRGB;
    }
#ifndef TARGET_IOS
    else if (result == MTLPixelFormatBC1_RGBA) {
      result = MTLPixelFormatBC1_RGBA_sRGB;
    } else if (result == MTLPixelFormatBC2_RGBA) {
      result = MTLPixelFormatBC2_RGBA_sRGB;
    } else if (result == MTLPixelFormatBC3_RGBA) {
      result = MTLPixelFormatBC3_RGBA_sRGB;
    } else if (result == MTLPixelFormatBC7_RGBAUnorm) {
      result = MTLPixelFormatBC7_RGBAUnorm_sRGB;
    }
#endif
  }

  return result;
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

MTLVertexFormat ToMtlVertexFormat(const ImageFormat format) {
  switch (format) {
    case RG8: return MTLVertexFormatUChar2Normalized;
    case RGB8: return MTLVertexFormatUChar3Normalized;
    case RGBA8: return MTLVertexFormatUChar4Normalized;

    case RG8S: return MTLVertexFormatChar2Normalized;
    case RGB8S: return MTLVertexFormatChar3Normalized;
    case RGBA8S: return MTLVertexFormatChar4Normalized;

    case RG16: return MTLVertexFormatUShort2Normalized;
    case RGB16: return MTLVertexFormatUShort3Normalized;
    case RGBA16: return MTLVertexFormatUShort4Normalized;

    case RG16S: return MTLVertexFormatShort2Normalized;
    case RGB16S: return MTLVertexFormatShort3Normalized;
    case RGBA16S: return MTLVertexFormatShort4Normalized;

    case RG16I: return MTLVertexFormatShort2;
    case RGB16I: return MTLVertexFormatShort3;
    case RGBA16I: return MTLVertexFormatShort4;

    case RG16UI: return MTLVertexFormatUShort2;
    case RGB16UI: return MTLVertexFormatUShort3;
    case RGBA16UI: return MTLVertexFormatUShort4;

    case RG16F: return MTLVertexFormatHalf2;
    case RGB16F: return MTLVertexFormatHalf3;
    case RGBA16F: return MTLVertexFormatHalf4;

    case R32F: return MTLVertexFormatFloat;
    case RG32F: return MTLVertexFormatFloat2;
    case RGB32F: return MTLVertexFormatFloat3;
    case RGBA32F: return MTLVertexFormatFloat4;

    case R32I: return MTLVertexFormatInt;
    case RG32I: return MTLVertexFormatInt2;
    case RGB32I: return MTLVertexFormatInt3;
    case RGBA32I: return MTLVertexFormatInt4;

    case R32UI: return MTLVertexFormatUInt;
    case RG32UI: return MTLVertexFormatUInt2;
    case RGB32UI: return MTLVertexFormatUInt3;
    case RGBA32UI: return MTLVertexFormatUInt4;

    case RGB10A2: return MTLVertexFormatUInt1010102Normalized;
    default: break;
  }
  LOGERRORF("Unknown vertex format: %d", format);
  return MTLVertexFormatInvalid;
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

  Shader* pShader = (Shader*) pShader;

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
      AddBuffer((Renderer*)pCmd->pRenderer, &bufferDesc, &argumentBuffer);

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
          [argumentEncoder setSamplerState:((Sampler*)descData->ppSamplers[i])->mtlSamplerState
                                   atIndex:i];
          break;
        case DESCRIPTOR_TYPE_BUFFER:
          [pCmd->mtlRenderEncoder useResource:((Buffer*)descData->ppBuffers[i])->mtlBuffer
                                        usage:(MTLResourceUsageRead | MTLResourceUsageSample)];
          [argumentEncoder setBuffer:((Buffer*)descData->ppBuffers[i])->mtlBuffer
                              offset:(descData->ppBuffers[i]->mPositionInHeap
                                  + (descData->pOffsets ? descData->pOffsets[i] : 0))
                             atIndex:i];
          break;
        case DESCRIPTOR_TYPE_TEXTURE:[pCmd->mtlRenderEncoder useResource:((Texture*)descData->ppTextures[i])->mtlTexture usage:MTLResourceUsageRead];
          [argumentEncoder setTexture:((Texture*)descData->ppTextures[i])->mtlTexture
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
