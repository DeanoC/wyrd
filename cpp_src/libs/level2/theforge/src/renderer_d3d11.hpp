#pragma once

#ifndef WYRD_THEFORGE_RENDERER_D3D11_HPP
#define WYRD_THEFORGE_RENDERER_D3D11_HPP
#include "core/core.h"
#include "theforge/renderer.hpp"

namespace TheForge {

struct QueryHeapD3D11 : public QueryHeap {
  ID3D11Query **ppDxQueries;
};

struct BufferD3D11 : public Buffer {
  ID3D11Buffer *pDxResource;
  ID3D11ShaderResourceView *pDxSrvHandle;
  ID3D11UnorderedAccessView *pDxUavHandle;
  DXGI_FORMAT mDxIndexFormat;
};

struct TextureD3D11 : public Texture {
  ID3D11Resource *pDxResource;
  ID3D11ShaderResourceView *pDxSRVDescriptor;
  ID3D11UnorderedAccessView **pDxUAVDescriptors;
};

struct RenderTargetD3D11 : public RenderTarget {
  union {
    /// Resources
    ID3D11RenderTargetView **pDxRtvDescriptors;
    ID3D11DepthStencilView **pDxDsvDescriptors;
  };
};

struct SamplerD3D11 : public Sampler {
  /// Native handle of the underlying resource
  ID3D11SamplerState *pSamplerState;
};

struct RootSignatureD3D11 : public RootSignature {
  ID3D11SamplerState **ppStaticSamplers;
  uint32_t *pStaticSamplerSlots;
  ShaderStage *pStaticSamplerStages;
  uint32_t mStaticSamplerCount;
};

struct CmdD3D11 : public Cmd {
  uint8_t *pDescriptorStructPool;
  uint8_t *pDescriptorNamePool;
  uint8_t *pDescriptorResourcesPool;
  uint64_t mDescriptorStructPoolOffset;
  uint64_t mDescriptorNamePoolOffset;
  uint64_t mDescriptorResourcePoolOffset;
  Buffer *pRootConstantBuffer;
  Buffer *pTransientConstantBuffer;
};

struct ShaderD3D11 : public Shader {
  ID3D11VertexShader *pDxVertexShader;
  ID3D11PixelShader *pDxPixelShader;
  ID3D11GeometryShader *pDxGeometryShader;
  ID3D11DomainShader *pDxDomainShader;
  ID3D11HullShader *pDxHullShader;
  ID3D11ComputeShader *pDxComputeShader;
  ID3DBlob *pDxInputSignature;
};

struct BlendStateD3D11 : public BlendState {
  ID3D11BlendState *pBlendState;
};

struct DepthStateD3D11 : public DepthState {
  ID3D11DepthStencilState *pDxDepthStencilState;
};

struct RasterizerStateD3D11 : public RasterizerState {
  ID3D11RasterizerState *pDxRasterizerState;
};

struct PipelineD3D11 : public Pipeline {
  ID3D11VertexShader *pDxVertexShader;
  ID3D11PixelShader *pDxPixelShader;
  ID3D11GeometryShader *pDxGeometryShader;
  ID3D11DomainShader *pDxDomainShader;
  ID3D11HullShader *pDxHullShader;
  ID3D11ComputeShader *pDxComputeShader;
  ID3D11InputLayout *pDxInputLayout;
  D3D_PRIMITIVE_TOPOLOGY mDxPrimitiveTopology;
};

struct SwapChainD3D11 : public SwapChain {
  /// Use IDXGISwapChain3 for now since IDXGISwapChain4
  /// isn't supported by older devices.
  IDXGISwapChain *pDxSwapChain;
  /// Sync interval to specify how interval for vsync
  UINT mDxSyncInterval;
  ID3D11Resource **ppDxSwapChainResources;
  uint32_t mImageIndex;
  uint32_t mFlags;

};

struct RendererD3D11 : public Renderer {
  IDXGIFactory1 *pDXGIFactory;
  IDXGIAdapter1 *pDxGPUs[MAX_GPUS];
  IDXGIAdapter1 *pDxActiveGPU;
  ID3D11Device *pDxDevice;
  ID3D11DeviceContext *pDxContext;
};

}
#endif //WYRD_THEFORGE_RENDERER_D3D11_HPP
