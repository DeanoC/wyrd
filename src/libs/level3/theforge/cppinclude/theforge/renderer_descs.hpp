#pragma once
#ifndef WYRD_THEFORGE_RENDERER_DESCS_HPP
#define WYRD_THEFORGE_RENDERER_DESCS_HPP

#include "core/core.h"
#include "theforge/renderer_descs.h"

namespace TheForge {

using BufferDesc = TheForge_BufferDesc;
using ClearValue = TheForge_ClearValue;
using RenderTargetDesc = TheForge_RenderTargetDesc;
using TextureDesc = TheForge_TextureDesc;
using LoadActionsDesc = TheForge_LoadActionsDesc;
using SamplerDesc = TheForge_SamplerDesc;
using RootSignatureDesc = TheForge_RootSignatureDesc;
using CmdPoolDesc = TheForge_CmdPoolDesc;
using QueueDesc = TheForge_QueueDesc;

using ShaderResource = TheForge_ShaderResourceDesc;
using BinaryShaderStageDesc = TheForge_BinaryShaderStageDesc;
using BinaryShaderDesc = TheForge_BinaryShaderDesc;
using BlendStateDesc = TheForge_BlendStateDesc;
using DepthStateDesc = TheForge_DepthStateDesc;
using RasterizerStateDesc = TheForge_RasterizerStateDesc;
using VertexAttrib = TheForge_VertexAttrib;
using VertexLayout = TheForge_VertexLayout;
using GraphicsPipelineDesc = TheForge_GraphicsPipelineDesc;
using ComputePipelineDesc = TheForge_ComputePipelineDesc;
using RaytracingPipelineDesc = TheForge_RaytracingPipelineDesc;
using PipelineDesc = TheForge_PipelineDesc;
using SubresourceDataDesc = TheForge_SubresourceDataDesc;
using RendererDescVulkan = TheForge_RendererDescVulkan;
using RendererDescD3D12 = TheForge_RendererDescD3D12;
using RendererDesc = TheForge_RendererDesc;
using IndirectArgumentDescriptor = TheForge_IndirectArgumentDescriptor;
using CommandSignatureDesc = TheForge_CommandSignatureDesc;
using QueryHeapDesc = TheForge_QueryHeapDesc;
}

#endif //WYRD_THEFORGE_RENDERER_DESCS_HPP
