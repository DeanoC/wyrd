#pragma once
#ifndef WYRD_THEFORGE_RENDERER_STRUCTS_H
#define WYRD_THEFORGE_RENDERER_STRUCTS_H

#include "core/core.h"
#include "theforge/renderer_enums.h"
#include "theforge/renderer_descs.h"
#include "stb/stb_dict.h"

typedef struct TheForge_IndirectDrawArguments {
  uint32_t mVertexCount;
  uint32_t mInstanceCount;
  uint32_t mStartVertex;
  uint32_t mStartInstance;
} TheForge_IndirectDrawArguments;

typedef struct TheForge_IndirectDrawIndexArguments {
  uint32_t mIndexCount;
  uint32_t mInstanceCount;
  uint32_t mStartIndex;
  uint32_t mVertexOffset;
  uint32_t mStartInstance;
} TheForge_IndirectDrawIndexArguments;

typedef struct TheForge_IndirectDispatchArguments {
  uint32_t mGroupCountX;
  uint32_t mGroupCountY;
  uint32_t mGroupCountZ;
} TheForge_IndirectDispatchArguments;

typedef struct TheForge_BufferBarrier {
  struct TheForge_Buffer *pBuffer;
  TheForge_ResourceStateFlags mNewState;
  bool mSplit;
} TheForge_BufferBarrier;

typedef struct TheForge_TextureBarrier {
  struct TheForge_Texture *pTexture;
  TheForge_ResourceStateFlags mNewState;
  bool mSplit;
} TheForge_TextureBarrier;

typedef struct TheForge_ReadRange {
  uint64_t mOffset;
  uint64_t mSize;
} TheForge_ReadRange;

typedef struct TheForge_QueryHeap {
  TheForge_QueryHeapDesc mDesc;
} TheForge_QueryHeap;

typedef struct TheForge_Buffer {
  /// A unique id used for hashing buffers during resource binding
  uint64_t mBufferId;
  /// Position of dynamic buffer memory in the mapped resource
  uint64_t mPositionInHeap;
  /// CPU address of the mapped buffer (appliacable to buffers created in CPU accessible heaps (CPU, CPU_TO_GPU, GPU_TO_CPU)
  void *pCpuMappedAddress;
  /// Buffer creation info
  TheForge_BufferDesc mDesc;
  /// Current state of the buffer
  TheForge_ResourceStateFlags mCurrentState;
  /// State of the buffer before mCurrentState (used for state tracking during a split barrier)
  TheForge_ResourceStateFlags mPreviousState;
} TheForge_Buffer;

typedef struct TheForge_Texture {
  /// A unique id used for hashing textures during resource binding
  uint64_t mTextureId;
  /// Texture creation info
  TheForge_TextureDesc mDesc;    //88
  /// Size of the texture (in bytes)
  uint64_t mTextureSize;
  /// Current state of the texture
  TheForge_ResourceStateFlags mCurrentState;
  /// State of the texture before mCurrentState (used for state tracking during a split barrier)
  TheForge_ResourceStateFlags mPreviousState;
  /// This value will be false if the underlying resource is not owned by the texture (swapchain textures,...)
  bool mOwnsImage;
} TheForge_Texture;

typedef struct TheForge_RenderTarget {
  TheForge_RenderTargetDesc mDesc;
  TheForge_Texture *pTexture;

} TheForge_RenderTarget;

typedef struct TheForge_Sampler {
  /// A unique id used for hashing samplers during resource binding
  uint64_t mSamplerId;
} TheForge_Sampler;

#include "theforge/shader_reflection.h"

/// Data structure holding the layout for a descriptor
typedef struct TheForge_DescriptorInfo {
  /// Binding information generated from the shader reflection
  TheForge_ShaderResourceDesc mDesc;
  /// Index in the descriptor set
  uint32_t mIndexInParent;
  /// Update frequency of this descriptor
  TheForge_DescriptorUpdateFrequency mUpdateFrquency;
  uint32_t mHandleIndex;
} TheForge_DescriptorInfo;

typedef struct TheForge_RootSignature {
  /// Number of descriptors declared in the root signature layout
  uint32_t mDescriptorCount;
  /// Array of all descriptors declared in the root signature layout
  TheForge_DescriptorInfo *pDescriptors;
  TheForge_PipelineType mPipelineType;

  /// Api specific binding manager
  stb_ptrmap pDescriptorManagerMap;

  /// Translates hash of descriptor name to descriptor index
  stb_udict32 pDescriptorNameToIndexMap;

} TheForge_RootSignature;

typedef struct TheForge_DescriptorData {
  /// User can either set name of descriptor or index (index in pRootSignature->pDescriptors array)
  /// Name of descriptor
  const char *pName;
  union {
    struct {
      /// Offset to bind the buffer descriptor
      uint64_t *pOffsets;
      uint64_t *pSizes;
    };
    uint32_t mUAVMipSlice;
  };
  /// Array of resources containing descriptor handles or constant to be used in ring buffer memory - DescriptorRange can hold only one resource type array
  union {
    /// Array of texture descriptors (srv and uav textures)
    TheForge_Texture **ppTextures;
    /// Array of sampler descriptors
    TheForge_Sampler **ppSamplers;
    /// Array of buffer descriptors (srv, uav and cbv buffers)
    TheForge_Buffer **ppBuffers;
    /// Constant data in system memory to be bound as root / push constant
    void *pRootConstant;
  };
  /// Number of resources in the descriptor(applies to array of textures, buffers,...)
  uint32_t mCount;
} TheForge_DescriptorData;

typedef struct TheForge_CmdPool {
  struct TheForge_Queue *pQueue;
  TheForge_CmdPoolDesc mCmdPoolDesc;
} TheForge_CmdPool;

typedef struct TheForge_Cmd {
  struct TheForge_Renderer *pRenderer;
  TheForge_CmdPool *pCmdPool;

  const TheForge_RootSignature *pBoundRootSignature;
  uint32_t *pBoundColorFormats;
  uint32_t mBoundDepthStencilFormat;
  uint32_t mBoundRenderTargetCount;
  TheForge_SampleCount mBoundSampleCount;
  uint32_t mBoundSampleQuality;
  uint32_t mBoundWidth;
  uint32_t mBoundHeight;
  uint32_t mNodeIndex;
  uint64_t mRenderPassHash;
} TheForge_Cmd;

typedef struct TheForge_Fence {
} TheForge_Fence;

typedef struct TheForge_Semaphore {
} TheForge_Semaphore;

typedef struct TheForge_Queue {
  struct TheForge_Renderer *pRenderer;
  TheForge_QueueDesc mQueueDesc;
} TheForge_Queue;

typedef struct TheForge_Shader {
  TheForge_ShaderStage mStages;
  TheForge_PipelineReflection mReflection;
} TheForge_Shader;

typedef struct TheForge_BlendState {
} TheForge_BlendState;

typedef struct TheForge_DepthState {
} TheForge_DepthState;

typedef struct TheForge_RasterizerState {
} TheForge_RasterizerState;

typedef struct TheForge_Pipeline {
  union {
    TheForge_GraphicsPipelineDesc mGraphics;
    TheForge_ComputePipelineDesc mCompute;
    TheForge_RaytracingPipelineDesc mRaytracing;
  };
  TheForge_PipelineType mType;
} TheForge_Pipeline;

typedef struct TheForge_GPUVendorPreset {
  char mVendorId[TheForge_MAX_GPU_VENDOR_STRING_LENGTH];
  char mModelId[TheForge_MAX_GPU_VENDOR_STRING_LENGTH];
  char mRevisionId[TheForge_MAX_GPU_VENDOR_STRING_LENGTH];    // OPtional as not all gpu's have that. Default is : 0x00
  TheForge_GPUPresetLevel mPresetLevel;
  char mGpuName[TheForge_MAX_GPU_VENDOR_STRING_LENGTH];    //If GPU Name is missing then value will be empty string
} TheForge_GPUVendorPreset;

typedef enum TheForge_DefaultResourceAlignment {
  RESOURCE_BUFFER_ALIGNMENT = 4U,
} TheForge_DefaultResourceAlignment;

typedef struct TheForge_GPUSettings {
  uint32_t mUniformBufferAlignment;
  uint32_t mUploadBufferTextureAlignment;
  uint32_t mUploadBufferTextureRowAlignment;
  uint32_t mMaxVertexInputBindings;
  uint32_t mMaxRootSignatureDWORDS;
  uint32_t mWaveLaneCount;
  TheForge_GPUVendorPreset mGpuVendorPreset;
  bool mMultiDrawIndirect;
  bool mROVsSupported;
} TheForge_GPUSettings;

typedef struct TheForge_ResourceAllocator TheForge_ResourceAllocator;

typedef struct TheForge_Renderer {
  char *pName;
  TheForge_RendererDesc mSettings;
  uint32_t mNumOfGPUs;
  TheForge_GPUSettings *pActiveGpuSettings;
  TheForge_GPUSettings mGpuSettings[TheForge_MAX_GPUS];
  TheForge_ResourceAllocator*   pResourceAllocator;

  // Default states used if user does not specify them in pipeline creation
  TheForge_BlendState *pDefaultBlendState;
  TheForge_DepthState *pDefaultDepthState;
  TheForge_RasterizerState *pDefaultRasterizerState;
} TheForge_Renderer;

typedef struct TheForge_CommandSignature {
  TheForge_CommandSignatureDesc mDesc;
  uint32_t mIndirectArgDescCounts;
  uint32_t mDrawCommandStride;
} TheForge_CommandSignature;

#endif //WYRD_THEFORGE_RENDERER_STRUCTS_H
