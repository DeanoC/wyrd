#pragma once

#ifndef WYRD_THEFORGE_RENDERER_D3D12_HPP
#define WYRD_THEFORGE_RENDERER_D3D12_HPP

#include "core/core.h"
#include "theforge/renderer.hpp"

namespace TheForge {

struct QueryHeapD3D12 : public QueryHeap {
  ID3D12QueryHeap *pDxQueryHeap;
};
struct BufferD3D12 : public Buffer {
  /// GPU Address
  D3D12_GPU_VIRTUAL_ADDRESS mDxGpuAddress;
  /// Descriptor handle of the CBV in a CPU visible descriptor heap (applicable to BUFFER_USAGE_UNIFORM)
  D3D12_CPU_DESCRIPTOR_HANDLE mDxCbvHandle;
  /// Descriptor handle of the SRV in a CPU visible descriptor heap (applicable to BUFFER_USAGE_STORAGE_SRV)
  D3D12_CPU_DESCRIPTOR_HANDLE mDxSrvHandle;
  /// Descriptor handle of the UAV in a CPU visible descriptor heap (applicable to BUFFER_USAGE_STORAGE_UAV)
  D3D12_CPU_DESCRIPTOR_HANDLE mDxUavHandle;
  /// Native handle of the underlying resource
  ID3D12Resource *pDxResource;
  /// Contains resource allocation info such as parent heap, offset in heap
  struct ResourceAllocation *pDxAllocation;

  DXGI_FORMAT mDxIndexFormat;
}
struct TextureD3D12 : public Texture {
  /// Descriptor handle of the SRV in a CPU visible descriptor heap (applicable to TEXTURE_USAGE_SAMPLED_IMAGE)
  D3D12_CPU_DESCRIPTOR_HANDLE mDxSRVDescriptor;
  D3D12_CPU_DESCRIPTOR_HANDLE *pDxUAVDescriptors;
  /// Native handle of the underlying resource
  ID3D12Resource *pDxResource;
  /// Contains resource allocation info such as parent heap, offset in heap
  struct ResourceAllocation *pDxAllocation;
};

struct RenderTargetD3D12 : public RenderTarget {
  D3D12_CPU_DESCRIPTOR_HANDLE *pDxDescriptors;
};

struct SamplerD3D12 : public Sampler {
  /// Description for creating the Sampler descriptor for ths sampler
  D3D12_SAMPLER_DESC mDxSamplerDesc;
  /// Descriptor handle of the Sampler in a CPU visible descriptor heap
  D3D12_CPU_DESCRIPTOR_HANDLE mDxSamplerHandle;
};

struct DescriptorInfoD3D12 : public DescriptorInfo {
  D3D12_ROOT_PARAMETER_TYPE mDxType;
};

struct RootSignatureD3D12 : public RootSignature {
  uint32_t mDxViewDescriptorTableRootIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t mDxSamplerDescriptorTableRootIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t *pDxViewDescriptorIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t mDxViewDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t mDxCumulativeViewDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t *pDxSamplerDescriptorIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t mDxSamplerDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t mDxCumulativeSamplerDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t *pDxRootDescriptorRootIndices;
  uint32_t *pDxRootConstantRootIndices;
  uint32_t mDxRootDescriptorCount;
  uint32_t mDxRootConstantCount;
  ID3D12RootSignature *pDxRootSignature;
  ID3DBlob *pDxSerializedRootSignatureString;
};
struct CmdPoolD3D12 : public CmdPool {
  // Temporarily move to Cmd struct until we get the command allocator pool logic working
  //ID3D12CommandAllocator*	   pDxCmdAlloc;
};

struct CmdD3D12 : public Cmd {
  // For now each command list will have its own allocator until we get the command allocator pool logic working
  ID3D12CommandAllocator *pDxCmdAlloc;
  ID3D12GraphicsCommandList *pDxCmdList;
  D3D12_RESOURCE_BARRIER pBatchBarriers[MAX_BATCH_BARRIERS];
  // Small ring buffer to be used for copying root constant data in case the root constant was converted to root cbv
  struct UniformRingBuffer *pRootConstantRingBuffer;
  D3D12_CPU_DESCRIPTOR_HANDLE mViewCpuHandle;
  D3D12_GPU_DESCRIPTOR_HANDLE mViewGpuHandle;
  uint64_t mViewPosition;
  D3D12_CPU_DESCRIPTOR_HANDLE mSamplerCpuHandle;
  D3D12_GPU_DESCRIPTOR_HANDLE mSamplerGpuHandle;
  uint64_t mSamplerPosition;
  D3D12_CPU_DESCRIPTOR_HANDLE mTransientCBVs;
  uint64_t mTransientCBVPosition;
  uint32_t mBatchBarrierCount;
};

struct FenceD3D12 : public Forge {
  ID3D12Fence *pDxFence;
  HANDLE pDxWaitIdleFenceEvent;
  uint64 mFenceValue;
};

struct SemaphoreD3D12 : public Semaphore {
  // DirectX12 does not have a concept of semaphores
  // All synchronization is done using fences
  // Simlate semaphore signal and wait using DirectX12 fences

  // Semaphores used in DirectX12 only in queueSubmit
  // queueSubmit -> How the semaphores work in DirectX12

  // pp_wait_semaphores -> queue->Wait is manually called on each fence in this
  // array before calling ExecuteCommandLists to make the fence work like a wait semaphore

  // pp_signal_semaphores -> Manually call queue->Signal on each fence in this array after
  // calling ExecuteCommandLists and increment the underlying fence value

  // queuePresent does not use the wait semaphore since the swapchain Present function
  // already does the synchronization in this case
};

struct QueueD3D12 : public Queue {
  ID3D12CommandQueue *pDxQueue;
  Fence *pQueueFence;
};
struct ShaderD3D12 : public Shader {
  ID3DBlob **pShaderBlobs;
};
struct BlendStateD3D12 : public BlendState {
  D3D12_BLEND_DESC mDxBlendDesc;

};
struct DepthStateD3D12 : public DepthState {
  D3D12_DEPTH_STENCIL_DESC mDxDepthStencilDesc;
};
struct RasterizerStateD3D12 : public RasterizerState {
  D3D12_RASTERIZER_DESC mDxRasterizerDesc;
};
struct PipelineD3D12 : public Pipeline {
  ID3D12PipelineState *pDxPipelineState;
  D3D_PRIMITIVE_TOPOLOGY mDxPrimitiveTopology;
};

struct SwapChainD3D12 : public SwapChain {
#if defined(_DURANGO)
  IDXGISwapChain1* pDxSwapChain;
    UINT             mDxSyncInterval;
    ID3D12Resource** ppDxSwapChainResources;
    uint32_t         mImageIndex;
    uint32_t         mFlags;
#elif defined(DIRECT3D12)
  /// Use IDXGISwapChain3 for now since IDXGISwapChain4
    /// isn't supported by older devices.
    IDXGISwapChain3* pDxSwapChain;
    /// Sync interval to specify how interval for vsync
    UINT             mDxSyncInterval;
    ID3D12Resource** ppDxSwapChainResources;
    uint32_t         mImageIndex;
    uint32_t         mFlags;
#endif
};

struct RendererDescD3D12 : public RendererDesc {
};

struct RendererD3D12 : public Renderer {
#if defined(_DURANGO)
  IDXGIFactory2* pDXGIFactory;
    IDXGIAdapter*  pDxGPUs[MAX_GPUS];
    IDXGIAdapter*  pDxActiveGPU;
    ID3D12Device*  pDxDevice;
    EsramManager*  pESRAMManager;
#if defined(_DEBUG) || defined(PROFILE)
    ID3D12Debug* pDXDebug;
#endif
    // Default NULL Descriptors for binding at empty descriptor slots to make sure all descriptors are bound at submit
    D3D12_CPU_DESCRIPTOR_HANDLE mSrvNullDescriptor;
    D3D12_CPU_DESCRIPTOR_HANDLE mUavNullDescriptor;
    D3D12_CPU_DESCRIPTOR_HANDLE mCbvNullDescriptor;
    D3D12_CPU_DESCRIPTOR_HANDLE mSamplerNullDescriptor;

    // API specific descriptor heap and memory allocator
    struct DescriptorStoreHeap* pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    struct DescriptorStoreHeap* pCbvSrvUavHeap[MAX_GPUS];
    struct DescriptorStoreHeap* pSamplerHeap[MAX_GPUS];
    struct ResourceAllocator*   pResourceAllocator;
    uint32_t                    mDxLinkedNodeCount;
#elif defined(DIRECT3D12)
  IDXGIFactory5* pDXGIFactory;
    IDXGIAdapter3* pDxGPUs[MAX_GPUS];
    IDXGIAdapter3* pDxActiveGPU;
    ID3D12Device*  pDxDevice;
#if defined(_DEBUG)
    ID3D12Debug*   pDXDebug;
#endif
    // Default NULL Descriptors for binding at empty descriptor slots to make sure all descriptors are bound at submit
    D3D12_CPU_DESCRIPTOR_HANDLE mSrvNullDescriptor;
    D3D12_CPU_DESCRIPTOR_HANDLE mUavNullDescriptor;
    D3D12_CPU_DESCRIPTOR_HANDLE mCbvNullDescriptor;
    D3D12_CPU_DESCRIPTOR_HANDLE mSamplerNullDescriptor;

    // API specific descriptor heap and memory allocator
    struct DescriptorStoreHeap* pCPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    struct DescriptorStoreHeap* pCbvSrvUavHeap[MAX_GPUS];
    struct DescriptorStoreHeap* pSamplerHeap[MAX_GPUS];
    struct ResourceAllocator*   pResourceAllocator;
    uint32_t                    mDxLinkedNodeCount;
#endif
};

struct CommandSignatureD3D12 : public CommandSignature {
  ID3D12CommandSignature *pDxCommandSignautre;
};

#endif //WYRD_THEFORGE_RENDERER_D3D12_HPP
