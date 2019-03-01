
#include "core/core.h"
#include "core/logger.h"
#include "tinystl/vector.h"
#include "tinystl/string.h"

#if !defined(__APPLE__) && !defined(TARGET_OS_MAC)
#error "MacOs is needed!"
#endif
#import <simd/simd.h>
#import <MetalKit/MetalKit.h>

#include "renderer_metal.hpp"
#include "memory_allocator.hpp"

namespace TheForge {

// Size and properties for the MTLResourceHeaps allocated for each resource type.
struct AllocatorHeapPropertiesMetal {
  uint64_t mBlockSize;
  MTLStorageMode mStorageMode;
  MTLCPUCacheMode mCPUCacheMode;
  const char *pName;
};

static const AllocatorHeapPropertiesMetal gHeapProperties[(int) AllocatorMemoryType::NUM_TYPES] = {
    /// Default Buffer
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Default Buffers Heap",
    },
    // Upload Buffer
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModeShared,
        MTLCPUCacheModeWriteCombined,
        "Upload Buffers Heap",
    },
    // Readback Buffer
    {
        RESOURCE_DEFAULT_SMALL_HEAP_BLOCK_SIZE,
        MTLStorageModeShared,
        MTLCPUCacheModeDefaultCache,
        "Readback Buffers Heap",
    },
    /// Texture Small
    {
        RESOURCE_DEFAULT_SMALL_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Small Textures Heap",
    },
    /// Texture Default
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Default Textures Heap",
    },
    /// Texture MSAA
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "MSAA Textures Heap",
    },
    /// RTV DSV
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "RenderTargets Heap",
    },
    /// RTV DSV MSAA
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "MSAA RenderTargets Heap",
    },
    /// RTV DSV Shared
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Shared RenderTargets Heap",
    },
    /// RTV DSV Shared MSAA
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Shared MSAA RenderTargets Heap",
    },
    /// RTV DSV Shared Adapter
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Shared Adapter RenderTargets Heap",
    },
    /// RTV DSV Shared Adapter MSAA
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Shared Adapter MSAA RenderTargets Heap",
    },
    /// Default UAV Buffer
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Default UAV Buffers Heap",
    },
    /// Upload UAV Buffer
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModeShared,
        MTLCPUCacheModeWriteCombined,
        "Upload UAV Buffers Heap",
    },
    /// Readback UAV Buffer
    {
        RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
        MTLStorageModeShared,
        MTLCPUCacheModeDefaultCache,
        "Readback UAV Buffers Heap",
    },
};

struct BufferCreateInfoMetal : public BufferCreateInfo {
  const uint64_t mSize;
  //const uint64_t	mAlignment;
};

struct TextureCreateInfoMetal : public TextureCreateInfo {
  MTLTextureDescriptor *pDesc;
  const bool mIsRT;
  const bool mIsMS;
};

long CreateBuffer(
    ResourceAllocator *allocator,
    const BufferCreateInfoMetal *pCreateInfo,
    const AllocatorMemoryRequirements *pMemoryRequirements,
    BufferMetal *pBuffer) {
  ASSERT(allocator && pCreateInfo && pMemoryRequirements && pBuffer);
  RESOURCE_DEBUG_LOG("resourceAllocCreateBuffer");
  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  AllocatorSuballocationType suballocType = RESOURCE_SUBALLOCATION_TYPE_BUFFER;

  // For GPU buffers, use special memory type
  // For CPU mapped UAV / SRV buffers, just use suballocation strategy
  if (((pBuffer->mDesc.mDescriptors & DESCRIPTOR_TYPE_RW_BUFFER)
      || (pBuffer->mDesc.mDescriptors & DESCRIPTOR_TYPE_BUFFER)) &&
      pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_GPU_ONLY) {
        suballocType = RESOURCE_SUBALLOCATION_TYPE_BUFFER_SRV_UAV;
  }

  // Get the proper resource options for the buffer usage.
  MTLResourceOptions mtlResourceOptions = 0;
  switch (pMemoryRequirements->usage) {
    case RESOURCE_MEMORY_USAGE_GPU_ONLY: mtlResourceOptions = MTLResourceStorageModePrivate;
      break;
    case RESOURCE_MEMORY_USAGE_CPU_ONLY:
      mtlResourceOptions = MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeShared;
      break;
    case RESOURCE_MEMORY_USAGE_CPU_TO_GPU:
      mtlResourceOptions = MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeShared;
      break;
    case RESOURCE_MEMORY_USAGE_GPU_TO_CPU:
      mtlResourceOptions = MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeShared;
      break;
    default: assert(!"Unknown buffer usage type");
      break;
  }

  // Get the proper size and alignment for the buffer's resource options.
  AllocationInfo info;
  info.mSizeAlign =
      [allocator->m_Device heapBufferSizeAndAlignWithLength:pCreateInfo->mSize options:mtlResourceOptions];
  info.mIsRT = false;
  info.mIsMS = false;

  // Allocate memory using allocator (either from a previously created heap, or from a new one).
  bool res = allocator->AllocateMemory(info, *pMemoryRequirements, suballocType, &pBuffer->pMtlAllocation);
  if (res) {
    if (pBuffer->pMtlAllocation->GetType() == ResourceAllocation::ALLOCATION_TYPE_BLOCK) {
      pBuffer->mtlBuffer =
          [pBuffer->pMtlAllocation->GetMemory() newBufferWithLength:pCreateInfo->mSize options:mtlResourceOptions];
      pBuffer->mtlBuffer.label = @"Placed Buffer";
      assert(pBuffer->mtlBuffer);

      if (pMemoryRequirements->flags & RESOURCE_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT) {
        if (pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_GPU_ONLY) {
          LOGWARNINGF(
              "Cannot map memory not visible on CPU. Use a readback buffer instead for reading the memory to a cpu visible "
              "buffer");
        } else {
          pBuffer->pMtlAllocation->GetBlock()->m_pMappedData = pBuffer->mtlBuffer.contents;
        }
      }
    } else {
      pBuffer->mtlBuffer = [allocator->m_Device newBufferWithLength:pCreateInfo->mSize options:mtlResourceOptions];
      pBuffer->mtlBuffer.label = @"Owned Buffer";
      assert(pBuffer->mtlBuffer);

      if (pMemoryRequirements->flags & RESOURCE_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT &&
          pMemoryRequirements->usage != RESOURCE_MEMORY_USAGE_GPU_ONLY) {
        pBuffer->pMtlAllocation->GetOwnAllocation()->m_pMappedData = pBuffer->mtlBuffer.contents;
      }
    }

    // Bind buffer with memory.
    if (pBuffer->pMtlAllocation) {
      // All steps succeeded.
      ResourceAllocationInfo allocInfo = {};
      allocator->GetAllocationInfo(pBuffer->pMtlAllocation, &allocInfo);
      pBuffer->pCpuMappedAddress = allocInfo.pMappedData;
      return true;
    }

    // If we failed to create a Metal allocation, free the temp memory and exit.
    allocator->FreeMemory(pBuffer->pMtlAllocation);
    return res;
  }

  // Exit (not properly allocated resource).
  return res;
}

void destroyBuffer(ResourceAllocator *allocator, Buffer *pBuffer) {
  if (pBuffer->mtlBuffer != nil) {
    ASSERT(allocator);
    RESOURCE_DEBUG_LOG("resourceAllocDestroyBuffer");

    RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

    if (!pBuffer->pMtlAllocation->GetResource()) {
      pBuffer->mtlBuffer = nil;
    }

    allocator->FreeMemory(pBuffer->pMtlAllocation);
  }
}

long createTexture(
    ResourceAllocator *allocator,
    const TextureCreateInfo *pCreateInfo,
    const AllocatorMemoryRequirements *pMemoryRequirements,
    Texture *pTexture) {
  ASSERT(allocator && pCreateInfo->pDesc && pMemoryRequirements && pTexture);

  RESOURCE_DEBUG_LOG("resourceAllocCreateImage");

  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  AllocatorSuballocationType suballocType;
  if (!resourceAllocFindSuballocType(pCreateInfo->pDesc, &suballocType)) {
    return false;
  }

  // Allocate memory using allocator.
  AllocationInfo info;
  info.mSizeAlign = [allocator->m_Device heapTextureSizeAndAlignWithDescriptor:pCreateInfo->pDesc];
  info.mIsRT = pCreateInfo->mIsRT;
  info.mIsMS = pCreateInfo->mIsMS;

  bool res = allocator->AllocateMemory(info, *pMemoryRequirements, suballocType, &pTexture->pMtlAllocation);
  if (res) {
    if (pTexture->pMtlAllocation->GetType() == ResourceAllocation::ALLOCATION_TYPE_BLOCK) {
      pTexture->mtlTexture = [pTexture->pMtlAllocation->GetMemory() newTextureWithDescriptor:pCreateInfo->pDesc];
      assert(pTexture->mtlTexture);
      pTexture->mtlTexture.label = @"Placed Texture";
    } else {
      pTexture->mtlTexture = [allocator->m_Device newTextureWithDescriptor:pCreateInfo->pDesc];
      assert(pTexture->mtlTexture);
      pTexture->mtlTexture.label = @"Owned Texture";
    }

    // Bind texture with memory.
    if (pTexture->pMtlAllocation) {
      // All steps succeeded.
      ResourceAllocationInfo allocInfo = {};
      allocator->GetAllocationInfo(pTexture->pMtlAllocation, &allocInfo);
      return true;
    }

    pTexture->mtlTexture = nil;
    allocator->FreeMemory(pTexture->pMtlAllocation);
    return res;
  }
  return res;
}

void destroyTexture(ResourceAllocator *allocator, Texture *pTexture) {
  if (pTexture->mtlTexture != nil) {
    ASSERT(allocator);

    RESOURCE_DEBUG_LOG("resourceAllocDestroyImage");

    RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

    pTexture->mtlTexture = nil;

    allocator->FreeMemory(pTexture->pMtlAllocation);
  }
}

}

uint32_t queryThreadExecutionWidth(TheForge::RendererMetal *pRenderer) {
  if (!pRenderer) {
    return 0;
  }

  NSError *error = nil;
  NSString *defaultComputeShader =
      @"#include <metal_stdlib>\n"
      "using namespace metal;\n"
      "kernel void simplest(texture2d<float, access::write> output [[texture(0)]],uint2 gid [[thread_position_in_grid]])\n"
      "{output.write(float4(0, 0, 0, 1), gid);}";

  // Load all the shader files with a .metal file extension in the project
  id <MTLLibrary>
      defaultLibrary = [pRenderer->pDevice newLibraryWithSource:defaultComputeShader options:nil error:&error];

  if (error != nil) {
    LOGWARNINGF("Could not create library for simple compute shader: %s", [[error localizedDescription] UTF8String]);
    return 0;
  }

  // Load the kernel function from the library
  id <MTLFunction> kernelFunction = [defaultLibrary newFunctionWithName:@"simplest"];

  // Create a compute pipeline state
  id <MTLComputePipelineState>
      computePipelineState = [pRenderer->pDevice newComputePipelineStateWithFunction:kernelFunction error:&error];
  if (error != nil) {
    LOGWARNINGF("Could not create compute pipeline state for simple compute shader: %s",
                [[error localizedDescription] UTF8String]);
    return 0;
  }

  return (uint32_t) computePipelineState.threadExecutionWidth;
}

EXTERN_C void TheForgeInitRenderer(const char *appName,
                                   const TheForge_RendererDesc *settings,
                                   TheForge_Renderer **ppRenderer) {
  using namespace TheForge;
  auto pRenderer = (RendererMetal *) malloc(sizeof(RendererMetal));
  ASSERT(pRenderer);
  if (pRenderer == nullptr) { return; }

  pRenderer->pName = (char *) malloc(strlen(appName) + 1);
  memcpy(pRenderer->pName, appName, strlen(appName));

  // Copy settings
  memcpy(&(pRenderer->mSettings), settings, sizeof(*settings));
  pRenderer->mSettings.mApi = TheForge_RENDERER_API_METAL;

  // Initialize the Metal bits
  {
    // Get the systems default device.
    pRenderer->pDevice = MTLCreateSystemDefaultDevice();

    //get gpu vendor and model id.
    GPUVendorPreset gpuVendor;
    gpuVendor.mPresetLevel = GPU_PRESET_LOW;

#ifndef TARGET_IOS
    tinystl::string outModelId;
//    retrieveSystemProfilerInformation(outModelId);
//    displayGraphicsInfo(pRenderer->pDevice.registryID, outModelId, gpuVendor);
    tinystl::string mDeviceName = [pRenderer->pDevice.name UTF8String];
    strncpy(gpuVendor.mGpuName, mDeviceName.c_str(), MAX_GPU_VENDOR_STRING_LENGTH);
    LOGINFOF("Current Gpu Name: %s", gpuVendor.mGpuName);
    LOGINFOF("Current Gpu Vendor ID: %s", gpuVendor.mVendorId);
    LOGINFOF("Current Gpu Model ID: %s", gpuVendor.mModelId);
#else
    strncpy(gpuVendor.mVendorId, "Apple", MAX_GPU_VENDOR_STRING_LENGTH);
        strncpy(gpuVendor.mModelId, "iOS", MAX_GPU_VENDOR_STRING_LENGTH);

#endif
    // Set the default GPU settings.
    pRenderer->mNumOfGPUs = 1;
    pRenderer->mGpuSettings[0].mUniformBufferAlignment = 256;
    pRenderer->mGpuSettings[0].mUploadBufferTextureAlignment = 16;
    pRenderer->mGpuSettings[0].mUploadBufferTextureRowAlignment = 1;
    pRenderer->mGpuSettings[0].mMaxVertexInputBindings =
        MAX_VERTEX_BINDINGS;    // there are no special vertex buffers for input in Metal, only regular buffers
    pRenderer->mGpuSettings[0].mMultiDrawIndirect =
        false;    // multi draw indirect is not supported on Metal: only single draw indirect
    pRenderer->mGpuSettings[0].mGpuVendorPreset = gpuVendor;
    pRenderer->pActiveGpuSettings = &pRenderer->mGpuSettings[0];
    pRenderer->mGpuSettings[0].mROVsSupported = [pRenderer->pDevice areRasterOrderGroupsSupported];
    pRenderer->mGpuSettings[0].mWaveLaneCount = queryThreadExecutionWidth(pRenderer);

#ifndef TARGET_IOS
//    setGPUPresetLevel(pRenderer);
    //exit app if gpu being used has an office preset.
    if (pRenderer->pActiveGpuSettings->mGpuVendorPreset.mPresetLevel < GPU_PRESET_LOW) {
      ASSERT(pRenderer->pActiveGpuSettings->mGpuVendorPreset.mPresetLevel >= GPU_PRESET_LOW);

      //remove allocated name
      free(pRenderer->pName);
      pRenderer->pName = nullptr;
      //set device to null
      pRenderer->pDevice = nil;
      //remove allocated renderer
      free(pRenderer);
      pRenderer = nullptr;

      LOGERROR("Selected GPU has an office Preset in gpu.cfg");
      LOGERROR("Office Preset is not supported by the Forge");

      ppRenderer = nullptr;

#ifdef AUTOMATED_TESTING
      //exit with success return code not to show failure on Jenkins
            exit(0);
#endif
      return;
    }
#endif
    // Create a resource allocator.
    AllocatorCreateInfo info = {0};
    info.device = pRenderer->pDevice;
    //info.physicalDevice = pRenderer->pActiveGPU;
    createAllocator(&info, &pRenderer->pResourceAllocator);

    // Create default resources.
    create_default_resources(pRenderer);

    // Renderer is good! Assign it to result!
    *(ppRenderer) = pRenderer;
  }
}

void removeRenderer(Renderer *pRenderer) {
  ASSERT(pRenderer);
  SAFE_FREE(pRenderer->pName);
  destroyAllocator(pRenderer->pResourceAllocator);
  pRenderer->pDevice = nil;
  SAFE_FREE(pRenderer);
}
