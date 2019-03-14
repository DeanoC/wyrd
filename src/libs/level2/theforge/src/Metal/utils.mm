#import <simd/simd.h>
#import <MetalKit/MetalKit.h>

#include "core/core.h"
#include "math/math.h"
#include "os/thread.hpp"
#include "utils.hpp"
#include "../Image/image.h"
#include "renderer.hpp"

namespace TheForge { namespace Metal { namespace Util {
static MTLPixelFormat const gMtlFormatTranslator[] = {
    MTLPixelFormatInvalid,

    MTLPixelFormatR8Unorm,
    MTLPixelFormatRG8Unorm,
    MTLPixelFormatInvalid, //RGB8 not directly supported
    MTLPixelFormatRGBA8Unorm,

    MTLPixelFormatR16Unorm,
    MTLPixelFormatRG16Unorm,
    MTLPixelFormatInvalid, //RGB16 not directly supported
    MTLPixelFormatRGBA16Unorm,

    MTLPixelFormatR8Snorm,
    MTLPixelFormatRG8Snorm,
    MTLPixelFormatInvalid, //RGB8S not directly supported
    MTLPixelFormatRGBA8Snorm,

    MTLPixelFormatR16Snorm,
    MTLPixelFormatRG16Snorm,
    MTLPixelFormatInvalid, //RGB16S not directly supported
    MTLPixelFormatRGBA16Snorm,

    MTLPixelFormatR16Float,
    MTLPixelFormatRG16Float,
    MTLPixelFormatInvalid, //RGB16F not directly supported
    MTLPixelFormatRGBA16Float,

    MTLPixelFormatR32Float,
    MTLPixelFormatRG32Float,
    MTLPixelFormatInvalid, //RGB32F not directly supported
    MTLPixelFormatRGBA32Float,

    MTLPixelFormatR16Sint,
    MTLPixelFormatRG16Sint,
    MTLPixelFormatInvalid, //RGB16I not directly supported
    MTLPixelFormatRGBA16Sint,

    MTLPixelFormatR32Sint,
    MTLPixelFormatRG32Sint,
    MTLPixelFormatInvalid, //RGG32I not directly supported
    MTLPixelFormatRGBA32Sint,

    MTLPixelFormatR16Uint,
    MTLPixelFormatRG16Uint,
    MTLPixelFormatInvalid, //RGB16UI not directly supported
    MTLPixelFormatRGBA16Uint,

    MTLPixelFormatR32Uint,
    MTLPixelFormatRG32Uint,
    MTLPixelFormatInvalid, //RGB32UI not directly supported
    MTLPixelFormatRGBA32Uint,

    MTLPixelFormatInvalid, //RGBE8 not directly supported
    MTLPixelFormatRGB9E5Float,
    MTLPixelFormatRG11B10Float,
    MTLPixelFormatInvalid, //B5G6R5 not directly supported
    MTLPixelFormatInvalid, //RGBA4 not directly supported
    MTLPixelFormatRGB10A2Unorm,

#ifndef TARGET_IOS
    MTLPixelFormatDepth16Unorm,
    MTLPixelFormatDepth24Unorm_Stencil8,
    MTLPixelFormatDepth24Unorm_Stencil8,
#else
// Only 32-bit depth formats are supported on iOS.
    MTLPixelFormatDepth32Float,
    MTLPixelFormatDepth32Float,
    MTLPixelFormatDepth32Float,
#endif
    MTLPixelFormatDepth32Float,

#ifndef TARGET_IOS
    MTLPixelFormatBC1_RGBA,
    MTLPixelFormatBC2_RGBA,
    MTLPixelFormatBC3_RGBA,
    MTLPixelFormatBC4_RUnorm,
    MTLPixelFormatBC5_RGUnorm,

    // PVR formats
    MTLPixelFormatInvalid, // PVR_2BPP = 56,
    MTLPixelFormatInvalid, // PVR_2BPPA = 57,
    MTLPixelFormatInvalid, // PVR_4BPP = 58,
    MTLPixelFormatInvalid, // PVR_4BPPA = 59,
#else
MTLPixelFormatInvalid,
    MTLPixelFormatInvalid,
    MTLPixelFormatInvalid,
    MTLPixelFormatInvalid,
    MTLPixelFormatInvalid,

    // PVR formats
    MTLPixelFormatPVRTC_RGB_2BPP, // PVR_2BPP = 56,
    MTLPixelFormatPVRTC_RGBA_2BPP, // PVR_2BPPA = 57,
    MTLPixelFormatPVRTC_RGB_4BPP, // PVR_4BPP = 58,
    MTLPixelFormatPVRTC_RGBA_4BPP, // PVR_4BPPA = 59,
#endif

    MTLPixelFormatInvalid, // INTZ = 60,	// Nvidia hack. Supported on all DX10+ HW
    // Compressed mobile forms
    MTLPixelFormatInvalid, // ETC1 = 65,	//  RGB
    MTLPixelFormatInvalid, // ATC = 66, //  RGB
    MTLPixelFormatInvalid, // ATCA = 67,	//  RGBA, explicit alpha
    MTLPixelFormatInvalid, // ATCI = 68,	//  RGBA, interpolated alpha
    MTLPixelFormatInvalid, // RAWZ = 69, //depth only, Nvidia (requires recombination of data) //FIX IT: PS3 as well?
    MTLPixelFormatInvalid, // DF16 = 70, //depth only, Intel/AMD
    MTLPixelFormatInvalid, // STENCILONLY = 71, // stencil ony usage
    MTLPixelFormatInvalid, // GNF_BC1 = 72,
    MTLPixelFormatInvalid, // GNF_BC2 = 73,
    MTLPixelFormatInvalid, // GNF_BC3 = 74,
    MTLPixelFormatInvalid, // GNF_BC4 = 75,
    MTLPixelFormatInvalid, // GNF_BC5 = 76,
    MTLPixelFormatInvalid, // GNF_BC6 = 77,
    MTLPixelFormatInvalid, // GNF_BC7 = 78,
    // Reveser Form
    MTLPixelFormatBGRA8Unorm, // BGRA8 = 79,
    // Extend for DXGI
    MTLPixelFormatInvalid, // X8D24PAX32 = 80,
    MTLPixelFormatStencil8,// S8 = 81,
    MTLPixelFormatInvalid, // D16S8 = 82,
    MTLPixelFormatDepth32Float_Stencil8, // D32S8 = 83,

#ifndef TARGET_IOS
    // PVR formats
    MTLPixelFormatInvalid, // PVR_2BPP_SRGB = 84,
    MTLPixelFormatInvalid, // PVR_2BPPA_SRGB = 85,
    MTLPixelFormatInvalid, // PVR_4BPP_SRGB = 86,
    MTLPixelFormatInvalid, // PVR_4BPPA_SRGB = 87,
#else
// PVR formats
    MTLPixelFormatPVRTC_RGB_2BPP_sRGB, // PVR_2BPP_SRGB = 84,
    MTLPixelFormatPVRTC_RGBA_2BPP_sRGB, // PVR_2BPPA_SRGB = 85,
    MTLPixelFormatPVRTC_RGB_4BPP_sRGB, // PVR_4BPP_SRGB = 86,
    MTLPixelFormatPVRTC_RGBA_4BPP_sRGB, // PVR_4BPPA_SRGB = 87,
#endif
};


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
  MTLPixelFormat result = MTLPixelFormatInvalid;

  if (format >= sizeof(gMtlFormatTranslator) / sizeof(gMtlFormatTranslator[0])) {
    LOGERROR("Failed to Map from ConfettilFileFromat to MTLPixelFormat, should add map method in gMtlFormatTranslator");
  } else {
    result = gMtlFormatTranslator[format];
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
