#pragma once
#ifndef WYRD_THEFORGE_RENDERER_STRUCTS_HPP
#define WYRD_THEFORGE_RENDERER_STRUCTS_HPP

#include "core/core.h"
#include "theforge/renderer_structs.h"

namespace TheForge {

using IndirectDrawArguments = TheForge_IndirectDrawArguments;
using IndirectDrawIndexArguments = TheForge_IndirectDrawIndexArguments;
using IndirectDispatchArguments = TheForge_IndirectDispatchArguments;
using BufferBarrier = TheForge_BufferBarrier;
using TextureBarrier = TheForge_TextureBarrier;
using ReadRange = TheForge_ReadRange;
using QueryDesc = TheForge_QueryDesc;
using QueryHeap = TheForge_QueryHeap;
using Buffer = TheForge_Buffer;
using Texture = TheForge_Texture;
using RenderTarget = TheForge_RenderTarget;
using Sampler = TheForge_Sampler;
using DescriptorInfo = TheForge_DescriptorInfo;
using RootSignature = TheForge_RootSignature;
using DescriptorInfo = TheForge_DescriptorInfo;
using DescriptorData = TheForge_DescriptorData;
using CmdPool = TheForge_CmdPool;
using Cmd = TheForge_Cmd;
using Fence = TheForge_Fence;
using Semaphore = TheForge_Semaphore;
using Queue = TheForge_Queue;
using Shader = TheForge_Shader;
using BlendState = TheForge_BlendState;
using DepthState = TheForge_DepthState;
using RasterizerState = TheForge_RasterizerState;
using Pipeline = TheForge_Pipeline;
using SwapChain = TheForge_SwapChain;
using GPUVendorPreset = TheForge_GPUVendorPreset;
using DefaultResourceAlignment = TheForge_DefaultResourceAlignment;
using GPUSettings = TheForge_GPUSettings;
using Renderer = TheForge_Renderer;
using CommandSignature = TheForge_CommandSignature;
using ResourceAllocator = TheForge_ResourceAllocator;

}

#endif //WYRD_THEFORGE_RENDERER_STRUCTS_HPP
