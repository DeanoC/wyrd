#pragma once
#ifndef WYRD_THEFORGE_RENDERER_STRUCTS_HPP
#define WYRD_THEFORGE_RENDERER_STRUCTS_HPP

namespace TheForge {

struct BufferBarrier : public TheForge_BufferBarrier {
};

struct TextureBarrier : public TheForge_TextureBarrier {
};

struct ReadRange : public TheForge_ReadRange {
};

struct QueryDesc : public TheForge_QueryDesc {
};

struct QueryHeap : public TheForge_QueryHeap {
};

struct BufferDesc : public TheForge_BufferDesc {
};

struct Buffer : public TheForge_Buffer {
};

struct ClearValue : public TheForge_ClearValue {
};

struct TextureDesc : public TheForge_TextureDesc {
};

struct Texture : public TheForge_Texture {
};

struct RenderTargetDesc : public TheForge_RenderTargetDesc {
};

struct RenderTarget : public TheForge_RenderTarget {
};

struct LoadActionsDesc : public TheForge_LoadActionsDesc {
};

struct SamplerDesc : public TheForge_SamplerDesc {
};

struct Sampler : public TheForge_Sampler {
};

struct DescriptorInfo : public TheForge_DescriptorInfo {
};

struct RootSignatureDesc : public TheForge_RootSignatureDesc {
};

struct RootSignature : public TheForge_RootSignature {
};

struct DescriptorData : public TheForge_DescriptorData {
};

struct CmdPoolDesc : public TheForge_CmdPoolDesc {
};

struct CmdPool : public TheForge_CmdPool {
};

struct Cmd : public TheForge_Cmd {
};

struct QueueDesc : public TheForge_QueueDesc {
};

struct Fence : public TheForge_Fence {
};

struct Semaphore : public TheForge_Semaphore {
};

struct Queue : public TheForge_Queue {
};

struct ShaderMacro : public TheForge_ShaderMacro {
};

struct RendererShaderDefinesDesc : public TheForge_RendererShaderDefinesDesc {
};

struct ShaderStageDesc : public TheForge_ShaderStageDesc {
};

struct ShaderDesc : public TheForge_ShaderDesc {
};

struct BinaryShaderStageDesc : public TheForge_BinaryShaderStageDesc {
};

struct BinaryShaderDesc : public TheForge_BinaryShaderDesc {
};

struct Shader : public TheForge_Shader {
};

struct BlendStateDesc : public TheForge_BlendStateDesc {
};

struct BlendState : public TheForge_BlendState {
};

struct DepthStateDesc : public TheForge_DepthStateDesc {
};

struct DepthState : public TheForge_DepthState {
};

struct RasterizerStateDesc : public TheForge_RasterizerStateDesc {
};

struct RasterizerState : public TheForge_RasterizerState {
};

struct VertexAttrib : public TheForge_VertexAttrib {
};

struct VertexLayout : public TheForge_VertexLayout {
};

struct GraphicsPipelineDesc : public TheForge_GraphicsPipelineDesc {
};

struct ComputePipelineDesc : public TheForge_ComputePipelineDesc {
};

struct Pipeline : public TheForge_Pipeline {
};

struct SubresourceDataDesc : public TheForge_SubresourceDataDesc {
};

struct SwapChainDesc : public TheForge_SwapChainDesc {
};

struct SwapChain : public TheForge_SwapChain {
};

struct RendererDesc : public TheForge_RendererDesc {
};

struct GPUSettings : public TheForge_GPUSettings {
};

struct Renderer : public TheForge_Renderer {
};

struct IndirectArgumentDescriptor : public TheForge_IndirectArgumentDescriptor {
};

struct CommandSignatureDesc : public TheForge_CommandSignatureDesc {
};

struct CommandSignature : public TheForge_CommandSignature {
};

struct IndirectDrawArguments : public TheForge_IndirectDrawArguments {
};

struct IndirectDrawIndexArguments : public TheForge_IndirectDrawIndexArguments {
};

struct IndirectDispatchArguments : public TheForge_IndirectDispatchArguments {
};

}

#endif //WYRD_THEFORGE_RENDERER_STRUCTS_HPP
