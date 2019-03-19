#pragma once
#ifndef WYRD_THEFORGE_RENDERER_DESC_H
#define WYRD_THEFORGE_RENDERER_DESC_H

#include "core/core.h"
#include "guishell/window.h"
#include "theforge/image_enums.h"
#include "theforge/renderer_enums.h"

/// Data structure holding necessary info to create a Buffer
typedef struct TheForge_BufferDesc {
  /// Size of the buffer (in bytes)
  uint64_t mSize;
  /// Decides which memory heap buffer will use (default, upload, readback)
  TheForge_ResourceMemoryUsage mMemoryUsage;
  /// Creation flags of the buffer
  TheForge_BufferCreationFlags mFlags;
  /// What state will the buffer get created in
  TheForge_ResourceStateFlags mStartState;
  /// Specifies whether the buffer will have 32 bit or 16 bit indices (applicable to BUFFER_USAGE_INDEX)
  TheForge_IndexType mIndexType;
  /// Vertex stride of the buffer (applicable to BUFFER_USAGE_VERTEX)
  uint32_t mVertexStride;
  /// Index of the first element accessible by the SRV/UAV (applicable to BUFFER_USAGE_STORAGE_SRV, BUFFER_USAGE_STORAGE_UAV)
  uint64_t mFirstElement;
  /// Number of elements in the buffer (applicable to BUFFER_USAGE_STORAGE_SRV, BUFFER_USAGE_STORAGE_UAV)
  uint64_t mElementCount;
  /// Size of each element (in bytes) in the buffer (applicable to BUFFER_USAGE_STORAGE_SRV, BUFFER_USAGE_STORAGE_UAV)
  uint64_t mStructStride;
  /// Set this to specify a counter buffer for this buffer (applicable to BUFFER_USAGE_STORAGE_SRV, BUFFER_USAGE_STORAGE_UAV)
  struct TheForge_Buffer *pCounterBuffer;
  /// Format of the buffer (applicable to typed storage buffers (Buffer<T>)
  TheForge_ImageFormat mFormat;
  /// Flags specifying the suitable usage of this buffer (Uniform buffer, Vertex Buffer, Index Buffer,...)
  TheForge_DescriptorTypeFlags mDescriptors;
  /// Debug name used in gpu profile
  const char *pDebugName;
  uint32_t *pSharedNodeIndices;
  uint32_t mNodeIndex;
  uint32_t mSharedNodeIndexCount;
} TheForge_BufferDesc;

typedef struct TheForge_ClearValue {
  struct {
    float r;
    float g;
    float b;
    float a;
  };
  struct {
    float depth;
    uint32_t stencil;
  };
} TheForge_ClearValue;

typedef struct TheForge_RenderTargetDesc {
  /// Texture creation flags (decides memory allocation strategy, sharing access,...)
  TheForge_TextureCreationFlags mFlags;
  /// Width
  uint32_t mWidth;
  /// Height
  uint32_t mHeight;
  /// Depth (Should be 1 if not a mType is not TEXTURE_TYPE_3D)
  uint32_t mDepth;
  /// Texture array size (Should be 1 if texture is not a texture array or cubemap)
  uint32_t mArraySize;
  /// Number of mip levels
  uint32_t mMipLevels;
  /// MSAA
  TheForge_SampleCount mSampleCount;
  /// Internal image format
  TheForge_ImageFormat mFormat;
  /// Optimized clear value (recommended to use this same value when clearing the rendertarget)
  TheForge_ClearValue mClearValue;
  /// The image quality level. The higher the quality, the lower the performance. The valid range is between zero and the value appropriate for mSampleCount
  uint32_t mSampleQuality;
  /// Descriptor creation
  TheForge_DescriptorTypeFlags mDescriptors;
  const void *pNativeHandle;
  /// Debug name used in gpu profile
  const char *pDebugName;
  /// GPU indices to share this texture
  uint32_t *pSharedNodeIndices;
  /// Number of GPUs to share this texture
  uint32_t mSharedNodeIndexCount;
  /// GPU which will own this texture
  uint32_t mNodeIndex;
  /// Set whether rendertarget is srgb
  bool mSrgb;
} TheForge_RenderTargetDesc;

/// Data structure holding necessary info to create a Texture
typedef struct TheForge_TextureDesc {
  /// Texture creation flags (decides memory allocation strategy, sharing access,...)
  TheForge_TextureCreationFlags mFlags;
  /// Width
  uint32_t mWidth;
  /// Height
  uint32_t mHeight;
  /// Depth (Should be 1 if not a mType is not TEXTURE_TYPE_3D)
  uint32_t mDepth;
  /// Texture array size (Should be 1 if texture is not a texture array or cubemap)
  uint32_t mArraySize;
  /// Number of mip levels
  uint32_t mMipLevels;
  /// Number of multisamples per pixel (currently Textures created with mUsage TEXTURE_USAGE_SAMPLED_IMAGE only support SAMPLE_COUNT_1)
  TheForge_SampleCount mSampleCount;
  /// The image quality level. The higher the quality, the lower the performance. The valid range is between zero and the value appropriate for mSampleCount
  uint32_t mSampleQuality;
  /// Internal image format
  TheForge_ImageFormat mFormat;
  /// Optimized clear value (recommended to use this same value when clearing the rendertarget)
  TheForge_ClearValue mClearValue;
  /// What state will the texture get created in
  TheForge_ResourceStateFlags mStartState;
  /// Descriptor creation
  TheForge_DescriptorTypeFlags mDescriptors;
  /// Pointer to native texture handle if the texture does not own underlying resource
  const void *pNativeHandle;
  /// Debug name used in gpu profile
  const char *pDebugName;
  /// GPU indices to share this texture
  uint32_t *pSharedNodeIndices;
  /// Number of GPUs to share this texture
  uint32_t mSharedNodeIndexCount;
  /// GPU which will own this texture
  uint32_t mNodeIndex;
  /// Set whether texture is srgb
  bool mSrgb;
  /// Is the texture CPU accessible (applicable on hardware supporting CPU mapped textures (UMA))
  bool mHostVisible;
} TheForge_TextureDesc;

typedef struct TheForge_LoadActionsDesc {
  TheForge_ClearValue mClearColorValues[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  TheForge_LoadActionType mLoadActionsColor[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  TheForge_ClearValue mClearDepth;
  TheForge_LoadActionType mLoadActionDepth;
  TheForge_LoadActionType mLoadActionStencil;
} TheForge_LoadActionsDesc;

typedef struct TheForge_SamplerDesc {
  TheForge_FilterType mMinFilter;
  TheForge_FilterType mMagFilter;
  TheForge_MipMapMode mMipMapMode;
  TheForge_AddressMode mAddressU;
  TheForge_AddressMode mAddressV;
  TheForge_AddressMode mAddressW;
  float mMipLosBias;
  float mMaxAnisotropy;
  TheForge_CompareMode mCompareFunc;
} TheForge_SamplerDesc;

typedef struct TheForge_RootSignatureDesc {
  struct TheForge_Shader **ppShaders;
  uint32_t mShaderCount;
  uint32_t mMaxBindlessTextures;
  const char **ppStaticSamplerNames;
  struct TheForge_Sampler **ppStaticSamplers;
  uint32_t mStaticSamplerCount;
  TheForge_RootSignatureType mSignatureType;
  struct TheForge_ShaderResource *pRaytracingShaderResources;
  uint32_t pRaytracingResourcesCount;

  // Vulkan only at the moment
  const char **ppDynamicUniformBufferNames;
  uint32_t mDynamicUniformBufferCount;

} TheForge_RootSignatureDesc;

typedef struct TheForge_CmdPoolDesc {
  TheForge_CmdPoolType mCmdPoolType;
} TheForge_CmdPoolDesc;

typedef struct TheForge_QueueDesc {
  TheForge_QueueFlags mFlag;
  TheForge_QueuePriority mPriority;
  TheForge_CmdPoolType mType;
  uint32_t mNodeIndex;
} TheForge_QueueDesc;

typedef struct TheForge_ShaderMacro {
  char const *definition;
  char const *value;
} TheForge_ShaderMacro;

typedef struct TheForge_RendererShaderDefinesDesc {
  TheForge_ShaderMacro *rendererShaderDefines;
  uint32_t rendererShaderDefinesCnt;
} TheForge_RendererShaderDefinesDesc;

typedef struct TheForge_ShaderStageDesc {
  char const *mName;
  char const *mCode;
  char const *mEntryPoint;
  size_t mNumMacros;
  TheForge_ShaderMacro const *mMacros;
} TheForge_ShaderStageDesc;

typedef struct TheForge_ShaderDesc {
  TheForge_ShaderStage mStages;
  TheForge_ShaderStageDesc mVert;
  TheForge_ShaderStageDesc mFrag;
  TheForge_ShaderStageDesc mGeom;
  TheForge_ShaderStageDesc mHull;
  TheForge_ShaderStageDesc mDomain;
  TheForge_ShaderStageDesc mComp;
} TheForge_ShaderDesc;

typedef struct TheForge_BinaryShaderStageDesc {
  /// Byte code array
  char *pByteCode;
  uint32_t mByteCodeSize;
  // Shader source is needed for reflection
  char const *mSource;
  /// Entry point is needed for Metal
  char const *mEntryPoint;
} TheForge_BinaryShaderStageDesc;

typedef struct TheForge_BinaryShaderDesc {
  TheForge_ShaderStage mStages;
  TheForge_BinaryShaderStageDesc mVert;
  TheForge_BinaryShaderStageDesc mFrag;
  TheForge_BinaryShaderStageDesc mGeom;
  TheForge_BinaryShaderStageDesc mHull;
  TheForge_BinaryShaderStageDesc mDomain;
  TheForge_BinaryShaderStageDesc mComp;
} TheForge_BinaryShaderDesc;

typedef struct TheForge_BlendStateDesc {
  /// Source blend factor per render target.
  TheForge_BlendConstant mSrcFactors[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  /// Destination blend factor per render target.
  TheForge_BlendConstant mDstFactors[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  /// Source alpha blend factor per render target.
  TheForge_BlendConstant mSrcAlphaFactors[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  /// Destination alpha blend factor per render target.
  TheForge_BlendConstant mDstAlphaFactors[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  /// Blend mode per render target.
  TheForge_BlendMode mBlendModes[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  /// Alpha blend mode per render target.
  TheForge_BlendMode mBlendAlphaModes[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  /// Write mask per render target.
  int32_t mMasks[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  /// Mask that identifies the render targets affected by the blend state.
  TheForge_BlendStateTargetsFlags mRenderTargetMask;
  /// Set whether alpha to coverage should be enabled.
  bool mAlphaToCoverage;
  /// Set whether each render target has an unique blend function. When false the blend function in slot 0 will be used for all render targets.
  bool mIndependentBlend;
} TheForge_BlendStateDesc;

typedef struct TheForge_DepthStateDesc {
  bool mDepthTest;
  bool mDepthWrite;
  TheForge_CompareMode mDepthFunc;// = TheForge_CMP_LEQUAL;
  bool mStencilTest;
  uint8_t mStencilReadMask;
  uint8_t mStencilWriteMask;
  TheForge_CompareMode mStencilFrontFunc;// = TheForge_CMP_ALWAYS;
  TheForge_StencilOp mStencilFrontFail;
  TheForge_StencilOp mDepthFrontFail;
  TheForge_StencilOp mStencilFrontPass;
  TheForge_CompareMode mStencilBackFunc;// = TheForge_CMP_ALWAYS;
  TheForge_StencilOp mStencilBackFail;
  TheForge_StencilOp mDepthBackFail;
  TheForge_StencilOp mStencilBackPass;
} TheForge_DepthStateDesc;

typedef struct TheForge_RasterizerStateDesc {
  TheForge_CullMode mCullMode;
  int32_t mDepthBias;
  float mSlopeScaledDepthBias;
  TheForge_FillMode mFillMode;
  bool mMultiSample;
  bool mScissor;
  TheForge_FrontFace mFrontFace;
} TheForge_RasterizerStateDesc;

typedef struct TheForge_VertexAttrib {
  TheForge_ShaderSemantic mSemantic;
  uint32_t mSemanticNameLength;
  char mSemanticName[TheForge_MAX_SEMANTIC_NAME_LENGTH];
  TheForge_ImageFormat mFormat;
  uint32_t mBinding;
  uint32_t mLocation;
  uint32_t mOffset;
  TheForge_VertexAttribRate mRate;

  uint32_t mSemanticType;
  uint32_t mSemanticIndex;
} TheForge_VertexAttrib;

typedef struct TheForge_VertexLayout {
  uint32_t mAttribCount;
  TheForge_VertexAttrib mAttribs[TheForge_MAX_VERTEX_ATTRIBS];
} TheForge_VertexLayout;

/************************************************************************/
// #pGlobalRootSignature - Root Signature used by all shaders in the ppShaders array
// #ppShaders - Array of all shaders which can be called during the raytracing operation
//	  This includes the ray generation shader, all miss, any hit, closest hit shaders
// #pHitGroups - Name of the hit groups which will tell the pipeline about which combination of hit shaders to use
// #mPayloadSize - Size of the payload struct for passing data to and from the shaders.
//	  Example - float4 payload sent by raygen shader which will be filled by miss shader as a skybox color
//				  or by hit shader as shaded color
// #mAttributeSize - Size of the intersection attribute. As long as user uses the default intersection shader
//	  this size is sizeof(float2) which represents the ZW of the barycentric co-ordinates of the intersection
/************************************************************************/
typedef struct TheForge_RaytracingPipelineDesc {
  struct Raytracing *pRaytracing;
  struct TheForge_RootSignature *pGlobalRootSignature;
  struct TheForge_Shader *pRayGenShader;
  struct TheForge_RootSignature *pRayGenRootSignature;
  struct TheForge_Shader **ppMissShaders;
  struct TheForge_RootSignature **ppMissRootSignatures;
  struct TheForge_RaytracingHitGroup *pHitGroups;
  struct TheForge_RootSignature *pEmptyRootSignature;
  unsigned mMissShaderCount;
  unsigned mHitGroupCount;
  // #TODO : Remove this after adding shader reflection for raytracing shaders
  unsigned mPayloadSize;
  // #TODO : Remove this after adding shader reflection for raytracing shaders
  unsigned mAttributeSize;
  unsigned mMaxTraceRecursionDepth;
  unsigned mMaxRaysCount;
} TheForge_RaytracingPipelineDesc;

typedef struct TheForge_GraphicsPipelineDesc {
  struct TheForge_Shader *pShaderProgram;
  struct TheForge_RootSignature *pRootSignature;
  struct TheForge_VertexLayout *pVertexLayout;
  struct TheForge_BlendState *pBlendState;
  struct TheForge_DepthState *pDepthState;
  struct TheForge_RasterizerState *pRasterizerState;
  TheForge_ImageFormat *pColorFormats;
  bool *pSrgbValues;
  uint32_t mRenderTargetCount;
  TheForge_SampleCount mSampleCount;
  uint32_t mSampleQuality;
  TheForge_ImageFormat mDepthStencilFormat;
  TheForge_PrimitiveTopology mPrimitiveTopo;
} TheForge_GraphicsPipelineDesc;

typedef struct TheForge_ComputePipelineDesc {
  struct TheForge_Shader *pShaderProgram;
  struct TheForge_RootSignature *pRootSignature;
} TheForge_ComputePipelineDesc;

typedef struct TheForge_PipelineDesc {
  TheForge_PipelineType mType;
  union {
    TheForge_ComputePipelineDesc mComputeDesc;
    TheForge_GraphicsPipelineDesc mGraphicsDesc;
    TheForge_RaytracingPipelineDesc mRaytracingDesc;
  };
} TheForge_PipelineDesc;

typedef struct TheForge_SubresourceDataDesc {
  uint32_t mArrayLayer;
  uint32_t mMipLevel;
  uint64_t mBufferOffset;
  uint32_t mWidth;
  uint32_t mHeight;
  uint32_t mDepth;

  uint32_t mRowPitch;
  uint32_t mSlicePitch;
  void *pData; // metal only allows direct access
} TheForge_SubresourceDataDesc;

typedef struct TheForge_SwapChainDesc {
  /// Window handle
  GuiShell_WindowDesc *pWindow;
  /// Queues which should be allowed to present
  struct TheForge_Queue **ppPresentQueues;
  /// Number of present queues
  uint32_t mPresentQueueCount;
  /// Number of backbuffers in this swapchain
  uint32_t mImageCount;
  /// Width of the swapchain
  uint32_t mWidth;
  /// Height of the swapchain
  uint32_t mHeight;
  /// Sample count
  TheForge_SampleCount mSampleCount;
  /// Sample quality (DirectX12 only)
  uint32_t mSampleQuality;
  /// Color format of the swapchain
  TheForge_ImageFormat mColorFormat;
  /// Clear value
  TheForge_ClearValue mColorClearValue;
  /// Set whether this swapchain using srgb color space
  bool mSrgb;
  /// Set whether swap chain will be presented using vsync
  bool mEnableVsync;
} TheForge_SwapChainDesc;

// backend specific renderer desc
typedef struct TheForge_RendererDescVulkan {
  int numInstanceLayers;
  int numInstanceExtensions;
  char const **mInstanceLayers;
  char const **mInstanceExtensions;
  char const **mDeviceExtensions;
//  PFN_vkDebugReportCallbackEXT pVkDebugFn;
} TheForge_RendererDescVulkan;

typedef struct TheForge_RendererDescD3D12 {
  TheForge_D3D_FEATURE_LEVEL mDxFeatureLevel;

} TheForge_RendererDescD3D12;

typedef struct TheForge_RendererDesc {
  TheForge_RendererApi mApi;
  TheForge_ShaderTarget mShaderTarget;
  TheForge_GpuMode mGpuMode;

  union {
    TheForge_RendererDescD3D12 mD3D12BackEndDesc;
    TheForge_RendererDescVulkan mVulkanBackEndDesc;
  };

} TheForge_RendererDesc;

// Indirect command sturcture define
typedef struct TheForge_IndirectArgumentDescriptor {
  TheForge_IndirectArgumentType mType;
  uint32_t mRootParameterIndex;
  uint32_t mCount;
  uint32_t mDivisor;

} TheForge_IndirectArgumentDescriptor;

typedef struct TheForge_CommandSignatureDesc {
  struct TheForge_CmdPool *pCmdPool;
  struct TheForge_RootSignature *pRootSignature;
  uint32_t mIndirectArgCount;
  TheForge_IndirectArgumentDescriptor *pArgDescs;
} TheForge_CommandSignatureDesc;

typedef struct TheForge_QueryHeapDesc {
  TheForge_QueryType mType;
  uint32_t mQueryCount;
  uint32_t mNodeIndex;
} TheForge_QueryHeapDesc;

typedef struct TheForge_QueryDesc {
  uint32_t mIndex;
} TheForge_QueryDesc;

#endif //WYRD_THEFORGE_RENDERER_DESC_H
