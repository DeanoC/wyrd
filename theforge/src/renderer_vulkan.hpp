#pragma once
#ifndef WYRD_THEFORGE_RENDERER_VULKAN_HPP
#define WYRD_THEFORGE_RENDERER_VULKAN_HPP

#include "core/core.h"
#include "theforge/renderer.hpp"

namespace TheForge {

struct QueryHeapVulkan : public QueryHeap {
  VkQueryPool pVkQueryPool;
};
struct BufferVulkan : public Buffer {
  /// Native handle of the underlying resource
  VkBuffer pVkBuffer;
  /// Buffer view
  VkBufferView pVkStorageTexelView;
  VkBufferView pVkUniformTexelView;
  /// Contains resource allocation info such as parent heap, offset in heap
  struct VmaAllocation_T *pVkAllocation;
  /// Description for creating the descriptor for this buffer (applicable to BUFFER_USAGE_UNIFORM, BUFFER_USAGE_STORAGE_SRV, BUFFER_USAGE_STORAGE_UAV)
  VkDescriptorBufferInfo mVkBufferInfo;
};

struct TextureVulkan : public Texture {
  /// Opaque handle used by shaders for doing read/write operations on the texture
  VkImageView pVkSRVDescriptor;
  /// Opaque handle used by shaders for doing read/write operations on the texture
  VkImageView *pVkUAVDescriptors;
  /// Native handle of the underlying resource
  VkImage pVkImage;
  /// Device memory handle
  VkDeviceMemory pVkMemory;
  /// Contains resource allocation info such as parent heap, offset in heap
  struct VmaAllocation_T *pVkAllocation;
  /// Flags specifying which aspects (COLOR,DEPTH,STENCIL) are included in the pVkImageView
  VkImageAspectFlags mVkAspectMask;
};

struct RenderVulkan : public RenderTarget {
  VkImageView *pVkDescriptors;
};

struct SamplerVulkan : public Sampler {
  /// Native handle of the underlying resource
  VkSampler pVkSampler;
  /// Description for creating the descriptor for this sampler
  VkDescriptorImageInfo mVkSamplerView;
};

struct DescriptorInfoVulkan : public DescriptorInfo {
  VkDescriptorType mVkType;
  VkShaderStageFlags mVkStages;
  uint32_t mDynamicUniformIndex;
};

struct RootSignatureDescVulkan : public RootSignatureDesc {
  const char **ppDynamicUniformBufferNames;
  uint32_t mDynamicUniformBufferCount;
};
struct RootSignatureVulkan : public RootSignature {
  VkDescriptorSetLayout mVkDescriptorSetLayouts[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t *pVkDescriptorIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t mVkDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t mVkDynamicDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
  uint32_t mVkCumulativeDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
  VkPushConstantRange *pVkPushConstantRanges;
  uint32_t mVkPushConstantCount;

  VkPipelineLayout pPipelineLayout;
};

struct CmdPoolVulkan : public CmdPool {
  VkCommandPool pVkCmdPool;
};

struct CmdVulkan : public Cmd {
  VkCommandBuffer pVkCmdBuf;
  VkRenderPass pVkActiveRenderPass;

  VkImageMemoryBarrier pBatchImageMemoryBarriers[MAX_BATCH_BARRIERS];
  VkBufferMemoryBarrier pBatchBufferMemoryBarriers[MAX_BATCH_BARRIERS];
  struct DescriptorStoreHeap *pDescriptorPool;
  uint32_t mBatchImageMemoryBarrierCount;
  uint32_t mBatchBufferMemoryBarrierCount;
};
struct FenceVulkan : public Forge {
  VkFence pVkFence;
  bool mSubmitted;
};

struct SemaphoreVulkan : public Semaphore {
  VkSemaphore pVkSemaphore;
  uint32_t mCurrentNodeIndex;
  bool mSignaled;
};

struct QueueVulkan : public Queue {
  VkQueue pVkQueue;
  uint32_t mVkQueueFamilyIndex;
  uint32_t mVkQueueIndex;
};

struct ShaderVulkan : public Shader {
  VkShaderModule *pShaderModules;
};

struct BlendStateVulkan : public BlendState {
  VkPipelineColorBlendAttachmentState RTBlendStates[MAX_RENDER_TARGET_ATTACHMENTS];
  VkBool32 LogicOpEnable;
  VkLogicOp LogicOp;
};

struct DepthStateVulkan : public DepthState {
  VkBool32 DepthTestEnable;
  VkBool32 DepthWriteEnable;
  VkCompareOp DepthCompareOp;
  VkBool32 DepthBoundsTestEnable;
  VkBool32 StencilTestEnable;
  VkStencilOpState Front;
  VkStencilOpState Back;
  float MinDepthBounds;
  float MaxDepthBounds;
};

struct RasterizerStateVulkan : public RasterizerState {
  VkBool32 DepthClampEnable;
  VkPolygonMode PolygonMode;
  VkCullModeFlags CullMode;
  VkFrontFace FrontFace;
  VkBool32 DepthBiasEnable;
  float DepthBiasConstantFactor;
  float DepthBiasClamp;
  float DepthBiasSlopeFactor;
  float LineWidth;
};

struct PipelineVulkan : public Pipeline {
  VkPipeline pVkPipeline;
};

struct SwapChainVulkan : public SwapChain {
  /// Present queue if one exists (queuePresent will use this queue if the hardware has a dedicated present queue)
  VkQueue pPresentQueue;
  VkSwapchainKHR pSwapChain;
  VkSurfaceKHR pVkSurface;
  VkImage *ppVkSwapChainImages;
};

struct RendererVulkan : public Renderer {
  VkInstance pVkInstance;
  VkPhysicalDevice pVkGPUs[MAX_GPUS];
  VkPhysicalDevice pVkActiveGPU;
  VkPhysicalDeviceProperties2 mVkGpuProperties[MAX_GPUS];
  VkPhysicalDeviceMemoryProperties mVkGpuMemoryProperties[MAX_GPUS];
  VkPhysicalDeviceFeatures mVkGpuFeatures[MAX_GPUS];
  uint32_t mVkQueueFamilyPropertyCount[MAX_GPUS];
  VkQueueFamilyProperties *mVkQueueFamilyProperties[MAX_GPUS];
  uint32_t mActiveGPUIndex;
  VkPhysicalDeviceMemoryProperties *pVkActiveGpuMemoryProperties;
  VkPhysicalDeviceProperties2 *pVkActiveGPUProperties;
  VkDevice pVkDevice;
#ifdef USE_DEBUG_UTILS_EXTENSION
  VkDebugUtilsMessengerEXT pVkDebugUtilsMessenger;
#endif
  VkDebugReportCallbackEXT pVkDebugReport;
  tinystl::vector<const char *> mInstanceLayers;
  uint32_t mVkUsedQueueCount[MAX_GPUS][16];
  uint32_t mVkLinkedNodeCount;

  Texture *pDefaultTexture1DSRV;
  Texture *pDefaultTexture1DUAV;
  Texture *pDefaultTexture1DArraySRV;
  Texture *pDefaultTexture1DArrayUAV;
  Texture *pDefaultTexture2DSRV;
  Texture *pDefaultTexture2DUAV;
  Texture *pDefaultTexture2DArraySRV;
  Texture *pDefaultTexture2DArrayUAV;
  Texture *pDefaultTexture3DSRV;
  Texture *pDefaultTexture3DUAV;
  Texture *pDefaultTextureCubeSRV;
  Buffer *pDefaultBuffer;
  Sampler *pDefaultSampler;

  struct VmaAllocator_T *pVmaAllocator;
  struct DescriptorStoreHeap *pDescriptorPool;

  // These are the extensions that we have loaded
  const char *gVkInstanceExtensions[MAX_INSTANCE_EXTENSIONS];
  // These are the extensions that we have loaded
  const char *gVkDeviceExtensions[MAX_DEVICE_EXTENSIONS];

};

struct CommandSignatureVulkan : public CommandSignature {
  IndirectArgumentType mDrawType;
};

}
#endif //WYRD_THEFORGE_RENDERER_VULKAN_HPP
