#pragma once

#ifndef WYRD_THEFORGE_RENDERER_METAL_HPP
#define WYRD_THEFORGE_RENDERER_METAL_HPP
#include "core/core.h"
#include "theforge/renderer.hpp"

namespace TheForge {
struct BufferMetal : public Buffer {
  /// Contains resource allocation info such as parent heap, offset in heap
  struct ResourceAllocation *pMtlAllocation;
  /// Native handle of the underlying resource
  id <MTLBuffer> mtlBuffer;
};

struct TextureMetal : public Texture {
  /// Contains resource allocation info such as parent heap, offset in heap
  struct ResourceAllocation *pMtlAllocation;
  /// Native handle of the underlying resource
  id <MTLTexture> mtlTexture;
  id <MTLTexture> __strong *pMtlUAVDescriptors;
  MTLPixelFormat mtlPixelFormat;
  bool mIsCompressed;
};

struct RenderTargetMetal : public RenderTarget {
  // A separate texture is needed for stencil rendering on iOS.
  Texture *pStencil;
};
struct SamplerMetal : public Sampler {
  /// Native handle of the underlying resource
  id <MTLSamplerState> mtlSamplerState;
};

struct RootSignatureMetal : public RootSignature {
  Sampler **ppStaticSamplers;
  uint32_t *pStaticSamplerSlots;
  TheForge_ShaderStage *pStaticSamplerStages;
  uint32_t mStaticSamplerCount;
};

struct CmdMetal : public Cmd {
  id <MTLCommandBuffer> mtlCommandBuffer;
  id <MTLFence> mtlEncoderFence;    // Used to sync different types of encoders recording in the same Cmd.
  id <MTLRenderCommandEncoder> mtlRenderEncoder;
  id <MTLComputeCommandEncoder> mtlComputeEncoder;
  id <MTLBlitCommandEncoder> mtlBlitEncoder;
  MTLRenderPassDescriptor *pRenderPassDesc;
  Shader *pShader;
  Buffer *selectedIndexBuffer;
  uint64_t mSelectedIndexBufferOffset;
  MTLPrimitiveType selectedPrimitiveType;
  bool mRenderPassActive;
};
struct FenceMetal : public Fence {
  dispatch_semaphore_t pMtlSemaphore;
  bool mSubmitted;
};
struct SemaphoreMetal : public Semaphore {
  dispatch_semaphore_t pMtlSemaphore;
};

struct QueueMetal : public Queue {
  id <MTLCommandQueue> mtlCommandQueue;
  dispatch_semaphore_t pMtlSemaphore;
};
struct ShaderMetal : public Shader {
  id <MTLFunction> mtlVertexShader;
  id <MTLFunction> mtlFragmentShader;
  id <MTLFunction> mtlComputeShader;
  uint32_t mNumThreadsPerGroup[3];
};
struct BlendStateMetal : public BlendState {
  struct BlendStateData {
    MTLBlendFactor srcFactor;
    MTLBlendFactor destFactor;
    MTLBlendFactor srcAlphaFactor;
    MTLBlendFactor destAlphaFactor;
    MTLBlendOperation blendMode;
    MTLBlendOperation blendAlphaMode;
  };
  BlendStateData blendStatePerRenderTarget[TheForge_MAX_RENDER_TARGET_ATTACHMENTS];
  bool alphaToCoverage;
};
struct DepthStateMetal : public DepthState {
  id <MTLDepthStencilState> mtlDepthState;
};

struct RasterizerStateMetal : public RasterizerState {
  MTLCullMode cullMode;
  MTLTriangleFillMode fillMode;
  float depthBiasSlopeFactor;
  float depthBias;
  bool scissorEnable;
  bool multisampleEnable;
  MTLWinding frontFace;
};
struct PipelineMetal : public Pipeline {
  TheForge_RenderTarget *pRenderPasspRenderTarget;
  TheForge_Shader *pShader;
  id <MTLRenderPipelineState> mtlRenderPipelineState;
  id <MTLComputePipelineState> mtlComputePipelineState;
  MTLCullMode mCullMode;
};

struct SwapChainMetal : public SwapChain {
  MTKView *pMTKView;
  id <CAMetalDrawable> mMTKDrawable;
  id <MTLCommandBuffer> presentCommandBuffer;
};

struct RendererMetal : public Renderer {
  id <MTLDevice> pDevice;
  struct ResourceAllocator *pResourceAllocator;
};

struct CommandSignatureMetal : public CommandSignature {
  TheForge_IndirectArgumentType mDrawType;
};

}

#endif //WYRD_THEFORGE_RENDERER_METAL_HPP
