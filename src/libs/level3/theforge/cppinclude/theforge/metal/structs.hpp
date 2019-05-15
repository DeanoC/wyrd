#pragma once
#ifndef WYRD_THEFORGE_METAL_STRUCTS_HPP
#define WYRD_THEFORGE_METAL_STRUCTS_HPP

#import <simd/simd.h>
#import <MetalKit/MetalKit.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>

#include "core/core.h"
#include "tinystl/string.h"
#include "tinystl/unordered_map.h"
#include "theforge/renderer.hpp"
#include "theforge/raytracer.hpp"

namespace TheForge { namespace Metal {

struct BufferBarrier : public TheForge::BufferBarrier {
};

struct TextureBarrier : public TheForge::TextureBarrier {
};

struct QueryHeap : public TheForge::QueryHeap {
};

struct Buffer : public TheForge::Buffer {
  /// Contains resource allocation info such as parent heap, offset in heap
  struct ResourceAllocation *pMtlAllocation;
  /// Native handle of the underlying resource
  id <MTLBuffer> mtlBuffer;
};

struct Texture : public TheForge::Texture {
  /// Contains resource allocation info such as parent heap, offset in heap
  struct ResourceAllocation *pMtlAllocation;
  /// Native handle of the underlying resource
  id <MTLTexture> mtlTexture;
  id <MTLTexture> __strong *pMtlUAVDescriptors;
  MTLPixelFormat mtlPixelFormat;
  bool mIsCompressed;
};

struct RenderTarget : public TheForge::RenderTarget {
#if defined(TARGET_IOS)
  // A separate texture is needed for stencil rendering on iOS.
  Texture* pStencil;
#endif
};

struct Sampler : public TheForge::Sampler {
  /// Native handle of the underlying resource
  id <MTLSamplerState> mtlSamplerState;
};

struct RootSignature : public TheForge::RootSignature {
  Sampler **ppStaticSamplers;
  uint32_t *pStaticSamplerSlots;
  ShaderStage *pStaticSamplerStages;
  uint32_t mStaticSamplerCount;
};

struct CmdPool : public TheForge::CmdPool {
};

struct Cmd : public TheForge::Cmd {
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

struct Fence : public TheForge::Fence {
  dispatch_semaphore_t pMtlSemaphore;
  bool mSubmitted;
};

struct Semaphore : public TheForge::Semaphore {
  dispatch_semaphore_t pMtlSemaphore;
};

struct Queue : public TheForge::Queue {
  id <MTLCommandQueue> mtlCommandQueue;
};

struct Shader : public TheForge::Shader {
  id <MTLFunction> mtlVertexShader;
  id <MTLFunction> mtlFragmentShader;
  id <MTLFunction> mtlComputeShader;
  tinystl::string mtlVertexShaderEntryPoint;
  tinystl::string mtlFragmentShaderEntryPoint;
  tinystl::string mtlComputeShaderEntryPoint;
  uint32_t mNumThreadsPerGroup[3];
};

struct BlendState : public TheForge::BlendState {
  struct BlendStateData {
    MTLBlendFactor srcFactor;
    MTLBlendFactor destFactor;
    MTLBlendFactor srcAlphaFactor;
    MTLBlendFactor destAlphaFactor;
    MTLBlendOperation blendMode;
    MTLBlendOperation blendAlphaMode;
  };
  BlendStateData blendStatePerRenderTarget[MAX_RENDER_TARGET_ATTACHMENTS];
  bool alphaToCoverage;
};

struct DepthState : public TheForge::DepthState {
  id <MTLDepthStencilState> mtlDepthState;
};

struct RasterizerState : public TheForge::RasterizerState {
  MTLCullMode cullMode;
  MTLTriangleFillMode fillMode;
  float depthBiasSlopeFactor;
  float depthBias;
  bool scissorEnable;
  bool multisampleEnable;
  MTLWinding frontFace;
};
typedef struct RaytracingPipeline RaytracingPipeline;

struct Pipeline : public TheForge::Pipeline {
  RenderTarget *pRenderPassRenderTarget;

  Shader *pShader;
  id <MTLRenderPipelineState> mtlRenderPipelineState;
  id <MTLComputePipelineState> mtlComputePipelineState;
  RaytracingPipeline *pRaytracingPipeline;
  MTLCullMode mCullMode;
};


struct Renderer : public TheForge::Renderer {
  id <MTLDevice> pDevice;
  ResourceAllocator *pResourceAllocator;

  // defaults are const after construction, so can be accessed lock free
  Texture *pDefault1DTexture;
  Texture *pDefault1DTextureArray;
  Texture *pDefault2DTexture;
  Texture *pDefault2DTextureArray;
  Texture *pDefault3DTexture;
  Texture *pDefaultCubeTexture;
  Texture *pDefaultCubeTextureArray;
  Buffer *pDefaultBuffer;
  Sampler *pDefaultSampler;
  BlendState *pDefaultBlendState;
  DepthState *pDefaultDepthState;
  RasterizerState *pDefaultRasterizerState;

  uint64_t mBufferIds = 0;
  uint64_t mTextureIds = 0;
  uint64_t mSamplerIds = 0;

};

struct CommandSignature : public TheForge::CommandSignature {
  IndirectArgumentType mDrawType;
};

struct Raytracing : public TheForge::Raytracing {
  MPSRayIntersector* pIntersector;
};

}} // end namespace TheForge::Metal

#endif //WYRD_METAL_STRUCTS_HPP
