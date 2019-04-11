#import <simd/simd.h>
#import <MetalKit/MetalKit.h>
//#import <guishell/platform.hpp>

#include "core/core.h"
#include "math/math.h"
#include "os/thread.hpp"
#include "tinystl/unordered_map.h"
#include "theforge/metal/structs.hpp"
#include "memory_allocator.hpp"
#include "image/image.h"
#include "../dynamic_memory_allocator.hpp"
#include "theforge/metal/renderer.hpp"
#include "theforge/metal/utils.hpp"

#define MAX_FRAMES_IN_FLIGHT 3
static const int MaxDescriptorNamesPerRoot = 1024;

#define MAX_BUFFER_BINDINGS 31

namespace TheForge { namespace Metal {

namespace DefaultHeapSizes {
static const uint64_t LargeHeap = (64 * 1024 * 1024); // 64 MB
static const uint64_t SmallHeap = (16 * 1024 * 1024); // 16 MB
}
// clang-format off
static MTLBlendOperation const gMtlBlendOpTranslator[MAX_BLEND_MODES] = {
    MTLBlendOperationAdd,
    MTLBlendOperationSubtract,
    MTLBlendOperationReverseSubtract,
    MTLBlendOperationMin,
    MTLBlendOperationMax,
};

static MTLBlendFactor const gMtlBlendConstantTranslator[MAX_BLEND_CONSTANTS] = {
    MTLBlendFactorZero,
    MTLBlendFactorOne,
    MTLBlendFactorSourceColor,
    MTLBlendFactorOneMinusSourceColor,
    MTLBlendFactorDestinationColor,
    MTLBlendFactorOneMinusDestinationColor,
    MTLBlendFactorSourceAlpha,
    MTLBlendFactorOneMinusSourceAlpha,
    MTLBlendFactorDestinationAlpha,
    MTLBlendFactorOneMinusDestinationAlpha,
    MTLBlendFactorSourceAlphaSaturated,
    MTLBlendFactorBlendColor,
    MTLBlendFactorOneMinusBlendColor,
    //MTLBlendFactorBlendAlpha,
    //MTLBlendFactorOneMinusBlendAlpha,
    //MTLBlendFactorSource1Color,
    //MTLBlendFactorOneMinusSource1Color,
    //MTLBlendFactorSource1Alpha,
    //MTLBlendFactorOneMinusSource1Alpha,
};

static MTLCompareFunction const gMtlComparisonFunctionTranslator[MAX_COMPARE_MODES] = {
    MTLCompareFunctionNever,
    MTLCompareFunctionLess,
    MTLCompareFunctionEqual,
    MTLCompareFunctionLessEqual,
    MTLCompareFunctionGreater,
    MTLCompareFunctionNotEqual,
    MTLCompareFunctionGreaterEqual,
    MTLCompareFunctionAlways,
};

static MTLStencilOperation const gMtlStencilOpTranslator[MAX_STENCIL_OPS] = {
    MTLStencilOperationKeep,
    MTLStencilOperationZero,
    MTLStencilOperationReplace,
    MTLStencilOperationInvert,
    MTLStencilOperationIncrementWrap,
    MTLStencilOperationDecrementWrap,
    MTLStencilOperationIncrementClamp,
    MTLStencilOperationDecrementClamp,
};

static MTLCullMode const gMtlCullModeTranslator[MAX_CULL_MODES] = {
    MTLCullModeNone,
    MTLCullModeBack,
    MTLCullModeFront,
};

static MTLTriangleFillMode const gMtlFillModeTranslator[MAX_FILL_MODES] = {
    MTLTriangleFillModeFill,
    MTLTriangleFillModeLines,
};

static MTLSamplerAddressMode const gMtlAddressModeTranslator[] = {
    MTLSamplerAddressModeMirrorRepeat,
    MTLSamplerAddressModeRepeat,
    MTLSamplerAddressModeClampToEdge,
#ifndef TARGET_IOS
    MTLSamplerAddressModeClampToBorderColor,
#else
    MTLSamplerAddressModeClampToEdge,
#endif
};

AllocatorHeapProperties const gHeapProperties[RESOURCE_MEMORY_TYPE_NUM_TYPES] = {
    /// Default Buffer
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Default Buffers Heap",
    },
    // Upload Buffer
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModeShared,
        MTLCPUCacheModeWriteCombined,
        "Upload Buffers Heap",
    },
    // Readback Buffer
    {
        DefaultHeapSizes::SmallHeap,
        MTLStorageModeShared,
        MTLCPUCacheModeDefaultCache,
        "Readback Buffers Heap",
    },
    /// Texture Small
    {
        DefaultHeapSizes::SmallHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Small Textures Heap",
    },
    /// Texture Default
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Default Textures Heap",
    },
    /// Texture MSAA
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "MSAA Textures Heap",
    },
    /// RTV DSV
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "RenderTargets Heap",
    },
    /// RTV DSV MSAA
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "MSAA RenderTargets Heap",
    },
    /// RTV DSV Shared
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Shared RenderTargets Heap",
    },
    /// RTV DSV Shared MSAA
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Shared MSAA RenderTargets Heap",
    },
    /// RTV DSV Shared Adapter
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Shared Adapter RenderTargets Heap",
    },
    /// RTV DSV Shared Adapter MSAA
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Shared Adapter MSAA RenderTargets Heap",
    },
    /// Default UAV Buffer
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModePrivate,
        MTLCPUCacheModeDefaultCache,
        "Default UAV Buffers Heap",
    },
    /// Upload UAV Buffer
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModeShared,
        MTLCPUCacheModeWriteCombined,
        "Upload UAV Buffers Heap",
    },
    /// Readback UAV Buffer
    {
        DefaultHeapSizes::LargeHeap,
        MTLStorageModeShared,
        MTLCPUCacheModeDefaultCache,
        "Readback UAV Buffers Heap",
    },
};

// clang-format on

struct DescriptorManager;

/************************************************************************/
// Globals
/************************************************************************/

long CreateBuffer(
    ResourceAllocator *baseAllocator,
    const BufferCreateInfo *pCreateInfo,
    const AllocatorMemoryRequirements *pMemoryRequirements,
    Buffer *pBuffer) {

  MetalResourceAllocator *allocator = (MetalResourceAllocator *) baseAllocator;
  ASSERT(allocator && pCreateInfo && pMemoryRequirements && pBuffer);
  RESOURCE_DEBUG_LOG("resourceAllocCreateBuffer");
  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  AllocatorSuballocationType suballocType = RESOURCE_SUBALLOCATION_TYPE_BUFFER;

  // For GPU buffers, use special memory type
  // For CPU mapped UAV / SRV buffers, just use suballocation strategy
  if (((pBuffer->mDesc.mDescriptors & TheForge::DESCRIPTOR_TYPE_RW_BUFFER)
      || (pBuffer->mDesc.mDescriptors & TheForge::DESCRIPTOR_TYPE_BUFFER)) &&
      pMemoryRequirements->usage == TheForge::RESOURCE_MEMORY_USAGE_GPU_ONLY) {
    suballocType = RESOURCE_SUBALLOCATION_TYPE_BUFFER_SRV_UAV;
  }

  // Get the proper resource options for the buffer usage.
  MTLResourceOptions mtlResourceOptions;
  switch (pMemoryRequirements->usage) {
    case TheForge::RESOURCE_MEMORY_USAGE_GPU_ONLY: mtlResourceOptions = MTLResourceStorageModePrivate;
      break;
    case TheForge::RESOURCE_MEMORY_USAGE_CPU_ONLY:
      mtlResourceOptions = MTLResourceCPUCacheModeDefaultCache
          | MTLResourceStorageModeShared;
      break;
    case TheForge::RESOURCE_MEMORY_USAGE_CPU_TO_GPU:
      mtlResourceOptions = MTLResourceCPUCacheModeWriteCombined
          | MTLResourceStorageModeShared;
      break;
    case TheForge::RESOURCE_MEMORY_USAGE_GPU_TO_CPU:
      mtlResourceOptions = MTLResourceCPUCacheModeDefaultCache
          | MTLResourceStorageModeShared;
      break;
    default: assert(!"Unknown buffer usage type");
      break;
  }

  // Get the proper size and alignment for the buffer's resource options.
  AllocationInfo info;
  info.mSizeAlign = [allocator->m_Device
      heapBufferSizeAndAlignWithLength:pCreateInfo->mSize
                               options:mtlResourceOptions];
  info.mIsRT = false;
  info.mIsMS = false;

  // Allocate memory using allocator (either from a previously created heap, or from a new one).
  bool res = allocator->AllocateMemory(info, *pMemoryRequirements, suballocType, &pBuffer->pMtlAllocation);
  if (res) {
    if (pBuffer->pMtlAllocation->GetType() == ResourceAllocation::ALLOCATION_TYPE_BLOCK) {
      pBuffer->mtlBuffer = [pBuffer->pMtlAllocation->GetMemory()
          newBufferWithLength:
              pCreateInfo->mSize
                      options:
                          mtlResourceOptions];
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
      pBuffer->mtlBuffer = [allocator->m_Device
          newBufferWithLength:
              pCreateInfo->mSize
                      options:
                          mtlResourceOptions];
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

void DestroyBuffer(ResourceAllocator *baseAllocator, Buffer *pBuffer) {
  MetalResourceAllocator *allocator = (MetalResourceAllocator *) baseAllocator;
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

bool CreateTexture(
    ResourceAllocator *baseAllocator,
    const TextureCreateInfo *pCreateInfo,
    const AllocatorMemoryRequirements *pMemoryRequirements,
    Texture *pTexture) {
  MetalResourceAllocator *allocator = (MetalResourceAllocator *) baseAllocator;

  ASSERT(allocator && pCreateInfo->pDesc && pMemoryRequirements && pTexture);

  RESOURCE_DEBUG_LOG("resourceAllocCreateImage");

  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  AllocatorSuballocationType suballocType;
  if (!ResourceAllocFindSuballocType(pCreateInfo->pDesc, &suballocType)) {
    return false;
  }

  // Allocate memory using allocator.
  AllocationInfo info;
  info.mSizeAlign = [allocator->m_Device
      heapTextureSizeAndAlignWithDescriptor:
          pCreateInfo->pDesc];
  info.mIsRT = pCreateInfo->mIsRT;
  info.mIsMS = pCreateInfo->mIsMS;

  bool res = allocator->AllocateMemory(info, *pMemoryRequirements, suballocType, &pTexture->pMtlAllocation);
  if (res) {
    if (pTexture->pMtlAllocation->GetType() == ResourceAllocation::ALLOCATION_TYPE_BLOCK) {
      pTexture->mtlTexture = [pTexture->pMtlAllocation->GetMemory()
          newTextureWithDescriptor:
              pCreateInfo->pDesc];
      assert(pTexture->mtlTexture);
      pTexture->mtlTexture.label = @"Placed Texture";
    } else {
      pTexture->mtlTexture = [allocator->m_Device
          newTextureWithDescriptor:
              pCreateInfo->pDesc];
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

void DestroyTexture(ResourceAllocator *baseAllocator, Texture *pTexture) {
  MetalResourceAllocator *allocator = (MetalResourceAllocator *) baseAllocator;
  if (pTexture->mtlTexture != nil) {
    ASSERT(allocator);

    RESOURCE_DEBUG_LOG("resourceAllocDestroyImage");

    RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

    pTexture->mtlTexture = nil;

    allocator->FreeMemory(pTexture->pMtlAllocation);
  }

}
const DescriptorInfo *GetDescriptor(const RootSignature *pRootSignature, const char *pResName, uint32_t *pIndex) {
  uint32_t nameHash;

  int found = stb_udict32_get_flag(&pRootSignature->pDescriptorNameToIndexMap, stb_hash(pResName), &nameHash);
  if (found) {
    *pIndex = nameHash;
    return &pRootSignature->pDescriptors[nameHash];
  } else {
    LOGERRORF("Invalid descriptor param (%s)", pResName);
    return NULL;
  }
}

// Resource allocation statistics.
void CalculateMemoryStats(Renderer *pRenderer, char **stats) {
// TODO DEANO
  //  resourceAllocBuildStatsString(pRenderer->pResourceAllocator, stats, 0);
}
void FreeMemoryStats(Renderer *pRenderer, char *stats) {
// TODO DEANO
  //  resourceAllocFreeStatsString(pRenderer->pResourceAllocator,
//                               stats);
}

void ConsumeDynamicMemoryAllocator(
    DynamicMemoryAllocator *p_linear_allocator,
    uint64_t size,
    void **ppCpuAddress,
    uint64_t *pOffset,
    id <MTLBuffer> ppMtlBuffer) {

  TheForge::ConsumeDynamicMemoryAllocator(p_linear_allocator, size, ppCpuAddress, pOffset);

  if (ppMtlBuffer) {
    ppMtlBuffer = ((Buffer *) (p_linear_allocator->pBuffer))->mtlBuffer;
  }
}

void ConsumeDynamicMemoryAllocatorLockFree(
    DynamicMemoryAllocator *p_linear_allocator,
    uint64_t size,
    void **ppCpuAddress,
    uint64_t *pOffset,
    id <MTLBuffer> ppMtlBuffer) {
  TheForge::ConsumeDynamicMemoryAllocatorLockFree(p_linear_allocator, size, ppCpuAddress, pOffset);

  if (ppMtlBuffer) {
    ppMtlBuffer = ((Buffer *) (p_linear_allocator->pBuffer))->mtlBuffer;
  }
}

void CreateDefaultResources(Renderer *pRenderer) {
  TextureDesc texture1DDesc = {};
  texture1DDesc.mArraySize = 1;
  texture1DDesc.mDepth = 1;
  texture1DDesc.mFormat = Image_Format_R8_UNORM;
  texture1DDesc.mHeight = 1;
  texture1DDesc.mMipLevels = 1;
  texture1DDesc.mSampleCount = SAMPLE_COUNT_1;
  texture1DDesc.mStartState = RESOURCE_STATE_COMMON;
  texture1DDesc.mDescriptors = DESCRIPTOR_TYPE_TEXTURE | DESCRIPTOR_TYPE_RW_TEXTURE;
  texture1DDesc.mWidth = 2;
  texture1DDesc.mFlags = TEXTURE_CREATION_FLAG_OWN_MEMORY_BIT;
  AddTexture(pRenderer, &texture1DDesc, &pRenderer->pDefault1DTexture);

  TextureDesc texture1DArrayDesc = texture1DDesc;
  texture1DArrayDesc.mArraySize = 2;
  AddTexture(pRenderer, &texture1DArrayDesc, &pRenderer->pDefault1DTextureArray);

  TextureDesc texture2DDesc = texture1DDesc;
  texture2DDesc.mHeight = 2;
  AddTexture(pRenderer, &texture2DDesc, &pRenderer->pDefault2DTexture);

  TextureDesc texture2DArrayDesc = texture2DDesc;
  texture2DArrayDesc.mArraySize = 2;
  AddTexture(pRenderer, &texture2DArrayDesc, &pRenderer->pDefault2DTextureArray);

  TextureDesc texture3DDesc = texture2DDesc;
  texture3DDesc.mDepth = 2;
  AddTexture(pRenderer, &texture3DDesc, &pRenderer->pDefault3DTexture);

  TextureDesc textureCubeDesc = texture2DDesc;
  textureCubeDesc.mArraySize = 6;
  textureCubeDesc.mDescriptors |= DESCRIPTOR_TYPE_TEXTURE_CUBE;
  AddTexture(pRenderer, &textureCubeDesc, &pRenderer->pDefaultCubeTexture);

  TextureDesc textureCubeArrayDesc = textureCubeDesc;
  textureCubeArrayDesc.mArraySize *= 2;
#ifndef TARGET_IOS
  AddTexture(pRenderer, &textureCubeArrayDesc, &pRenderer->pDefaultCubeTextureArray);
#else
  if ([pRenderer->pDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily4_v1])
    AddTexture(pRenderer, &textureCubeArrayDesc, &pDefaultCubeTextureArray);
#endif

  BufferDesc bufferDesc = {};
  bufferDesc.mDescriptors = DESCRIPTOR_TYPE_BUFFER | DESCRIPTOR_TYPE_RW_BUFFER | DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  bufferDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
  bufferDesc.mStartState = RESOURCE_STATE_COMMON;
  bufferDesc.mSize = sizeof(uint32_t);
  bufferDesc.mFirstElement = 0;
  bufferDesc.mElementCount = 1;
  bufferDesc.mStructStride = sizeof(uint32_t);
  bufferDesc.mFlags = BUFFER_CREATION_FLAG_OWN_MEMORY_BIT;
  AddBuffer(pRenderer, &bufferDesc, &pRenderer->pDefaultBuffer);

  SamplerDesc samplerDesc = {};
  samplerDesc.mAddressU = ADDRESS_MODE_CLAMP_TO_BORDER;
  samplerDesc.mAddressV = ADDRESS_MODE_CLAMP_TO_BORDER;
  samplerDesc.mAddressW = ADDRESS_MODE_CLAMP_TO_BORDER;
  AddSampler(pRenderer, &samplerDesc, &pRenderer->pDefaultSampler);

  BlendStateDesc blendStateDesc = {};
  blendStateDesc.mDstAlphaFactors[0] = BC_ZERO;
  blendStateDesc.mDstFactors[0] = BC_ZERO;
  blendStateDesc.mSrcAlphaFactors[0] = BC_ONE;
  blendStateDesc.mSrcFactors[0] = BC_ONE;
  blendStateDesc.mMasks[0] = ALL;
  blendStateDesc.mRenderTargetMask = BLEND_STATE_TARGET_ALL;
  blendStateDesc.mIndependentBlend = false;
  AddBlendState(pRenderer, &blendStateDesc, &pRenderer->pDefaultBlendState);

  DepthStateDesc depthStateDesc = {};
  depthStateDesc.mDepthFunc = CMP_LEQUAL;
  depthStateDesc.mDepthTest = false;
  depthStateDesc.mDepthWrite = true;
  depthStateDesc.mStencilBackFunc = CMP_ALWAYS;
  depthStateDesc.mStencilFrontFunc = CMP_ALWAYS;
  depthStateDesc.mStencilReadMask = 0xFF;
  depthStateDesc.mStencilWriteMask = 0xFF;
  AddDepthState(pRenderer, &depthStateDesc, &pRenderer->pDefaultDepthState);

  RasterizerStateDesc rasterizerStateDesc = {};
  rasterizerStateDesc.mCullMode = CULL_MODE_BACK;
  AddRasterizerState(pRenderer, &rasterizerStateDesc, &pRenderer->pDefaultRasterizerState);
}

void DestroyDefaultResources(Renderer *pRenderer) {
  RemoveTexture(pRenderer, pRenderer->pDefault1DTexture);
  RemoveTexture(pRenderer, pRenderer->pDefault1DTextureArray);
  RemoveTexture(pRenderer, pRenderer->pDefault2DTexture);
  RemoveTexture(pRenderer, pRenderer->pDefault2DTextureArray);
  RemoveTexture(pRenderer, pRenderer->pDefault3DTexture);
  RemoveTexture(pRenderer, pRenderer->pDefaultCubeTexture);
  RemoveTexture(pRenderer, pRenderer->pDefaultCubeTextureArray);
  RemoveBuffer(pRenderer, pRenderer->pDefaultBuffer);
  RemoveSampler(pRenderer, pRenderer->pDefaultSampler);

  RemoveBlendState(pRenderer, pRenderer->pDefaultBlendState);
  RemoveDepthState(pRenderer, pRenderer->pDefaultDepthState);
  RemoveRasterizerState(pRenderer, pRenderer->pDefaultRasterizerState);
}


#ifndef TARGET_IOS
// Returns the CFDictionary that contains the system profiler data type described in inDataType.
CFDictionaryRef FindDictionaryForDataType(const CFArrayRef inArray, CFStringRef inDataType) {
  UInt8 i;
  CFDictionaryRef theDictionary;

  // Search the array of dictionaries for a CFDictionary that matches
  for (i = 0; i < CFArrayGetCount(inArray); i++) {
    theDictionary = (CFDictionaryRef) CFArrayGetValueAtIndex(inArray, i);

    // If the CFDictionary at this index has a key/value pair with the value equal to inDataType, retain and return it.
    if (CFDictionaryContainsValue(theDictionary, inDataType)) {
      return (theDictionary);
    }
  }

  return (NULL);
}

// Returns the CFArray of “item” dictionaries.
CFArrayRef GetItemsArrayFromDictionary(const CFDictionaryRef inDictionary) {
  CFArrayRef itemsArray;

  // Retrieve the CFDictionary that has a key/value pair with the key equal to “_items”.
  itemsArray = (CFArrayRef) CFDictionaryGetValue(inDictionary, CFSTR("_items"));
  if (itemsArray != NULL) {
    CFRetain(itemsArray);
  }

  return (itemsArray);
}
//Used to call system profiler to retrieve GPU information such as vendor id and model id
void RetrieveSystemProfilerInformation(tinystl::string& outVendorId) {
  FILE *sys_profile;
  size_t bytesRead = 0;
  char streamBuffer[1024 * 512];
  UInt8 i = 0;
  CFDataRef xmlData;
  CFDictionaryRef hwInfoDict;
  CFArrayRef itemsArray;
  CFIndex arrayCount;

  // popen will fork and invoke the system_profiler command and return a stream reference with its result data
  sys_profile = popen("system_profiler SPDisplaysDataType -xml", "r");
  // Read the stream into a memory buffer
  bytesRead = fread(streamBuffer, sizeof(char), sizeof(streamBuffer), sys_profile);
  // Close the stream
  pclose(sys_profile);
  if (bytesRead == 0) {
    LOGERROR("Couldn't read SPDisplaysData from system_profiler");
    outVendorId = tinystl::string("0x0000");
    return;
  }
  // Create a CFDataRef with the xml data
  xmlData = CFDataCreate(kCFAllocatorDefault, (UInt8 *) streamBuffer, bytesRead);
  // CFPropertyListCreateFromXMLData reads in the XML data and will parse it into a CFArrayRef for us.
  CFStringRef errorString;
  //read xml data
  CFArrayRef propertyArray =
      ((CFArrayRef) CFPropertyListCreateWithData(kCFAllocatorDefault,
                                                 xmlData,
                                                 kCFPropertyListImmutable,
                                                 NULL,
                                                 (CFErrorRef *) &errorString));

  // This will be the dictionary that contains all the Hardware information that system_profiler knows about.
  hwInfoDict = FindDictionaryForDataType(propertyArray, CFSTR("SPDisplaysDataType"));
  if (hwInfoDict != NULL) {
    itemsArray = GetItemsArrayFromDictionary(hwInfoDict);

    if (itemsArray != NULL) {
      // Find out how many items in this category – each one is a dictionary
      arrayCount = CFArrayGetCount(itemsArray);

      for (i = 0; i < arrayCount; i++) {
        CFMutableStringRef outputString;

        // Create a mutable CFStringRef with the dictionary value found with key “machine_name”
        // This is the machine_name of this macresources machine.
        // Here you can give any value in key tag,to get its corresponding content
        outputString = CFStringCreateMutableCopy(
            kCFAllocatorDefault, 0,
            (CFStringRef) CFDictionaryGetValue(
                (CFDictionaryRef) CFArrayGetValueAtIndex(itemsArray, i), CFSTR("spdisplays_device-id")));
        NSString *outNS = (__bridge NSString *) outputString;
        outVendorId = [outNS.lowercaseString UTF8String];
        //your code here
        //(you can append output string OR modify your function according to your need )
        CFRelease(outputString);
      }

      CFRelease(itemsArray);
    }
    hwInfoDict = nil;
  }
  CFRelease(xmlData);
  CFRelease(propertyArray);
}
//Used to go through the given registry ID for the select device.
//Multiple id's can be found so they get filtered using the inModel id that was taken
//from system profile
void DisplayGraphicsInfo(uint64_t regId, tinystl::string inModel, GPUVendorPreset& vendorVecOut) {
  // Get dictionary of all the PCI Devices
  //CFMutableDictionaryRef matchDict = IOServiceMatching("IOPCIDevice");
  CFMutableDictionaryRef matchDict = IORegistryEntryIDMatching(regId);
  // Create an iterator
  io_iterator_t iterator;

  if (IOServiceGetMatchingServices(kIOMasterPortDefault, matchDict, &iterator) == kIOReturnSuccess) {
    // Iterator for devices found
    io_registry_entry_t regEntry;

    while ((regEntry = IOIteratorNext(iterator))) {
      // Put this services object into a dictionary object.
      CFMutableDictionaryRef serviceDictionary;
      if (IORegistryEntryCreateCFProperties(regEntry, &serviceDictionary, kCFAllocatorDefault, kNilOptions)
          != kIOReturnSuccess) {
        // Service dictionary creation failed.
        IOObjectRelease(regEntry);
        continue;
      }
      NSString *ioPCIMatch = nil;
      //on macbook IOPCIPrimaryMatch is used
      if (CFDictionaryContainsKey(serviceDictionary, CFSTR("IOPCIPrimaryMatch"))) {
        ioPCIMatch = (NSString *) CFDictionaryGetValue(serviceDictionary, CFSTR("IOPCIPrimaryMatch"));
      } else {
        //on iMac IOPCIMatch is used
        ioPCIMatch = (NSString *) CFDictionaryGetValue(serviceDictionary, CFSTR("IOPCIMatch"));
      }

      if (ioPCIMatch) {
        //get list of vendors from PCI Match above
        //this is a reflection of the display kext
        NSArray *vendors = [ioPCIMatch componentsSeparatedByString:@" "];
        for (id vendor in vendors) {
          NSString *modelId = [vendor substringToIndex:6];
          NSString *vendorId = [vendor substringFromIndex:6];
          vendorId = [@"0x" stringByAppendingString:vendorId];
          tinystl::string modelIdString = [modelId.lowercaseString UTF8String];
          tinystl::string vendorIdString = [vendorId.lowercaseString UTF8String];
          //filter out unwated model id's
          if (modelIdString != inModel) {
            continue;
          }

          strncpy(vendorVecOut.mModelId, modelIdString.c_str(), MAX_GPU_VENDOR_STRING_LENGTH);
          strncpy(vendorVecOut.mVendorId, vendorIdString.c_str(), MAX_GPU_VENDOR_STRING_LENGTH);
          vendorVecOut.mPresetLevel = GPU_PRESET_LOW;
          break;
        }
      }

      // Release the dictionary
      CFRelease(serviceDictionary);
      // Release the serviceObject
      IOObjectRelease(regEntry);
    }
    // Release the iterator
    IOObjectRelease(iterator);
  }
}
#endif

uint32_t QueryThreadExecutionWidth(Renderer *pRenderer) {
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

void InitRenderer(const char *appName, const RendererDesc *settings, Renderer **ppRenderer) {

  Renderer *pRenderer = (Renderer *) calloc(1, sizeof(Renderer));
  ASSERT(pRenderer);

  pRenderer->pName = (char *) calloc(strlen(appName) + 1, sizeof(char));
  memcpy(pRenderer->pName, appName, strlen(appName));

  // Copy settings
  memcpy(&(pRenderer->mSettings), settings, sizeof(*settings));
  pRenderer->mSettings.mApi = RENDERER_API_METAL;

  // Initialize the Metal bits
  {
    // Get the systems default device.
    pRenderer->pDevice = MTLCreateSystemDefaultDevice();

    //get gpu vendor and model id.
    GPUVendorPreset gpuVendor;
    gpuVendor.mPresetLevel = GPU_PRESET_LOW;
#ifndef TARGET_IOS
    tinystl::string outModelId;
    RetrieveSystemProfilerInformation(outModelId);
    DisplayGraphicsInfo(pRenderer->pDevice.registryID, outModelId, gpuVendor);
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
    pRenderer->mGpuSettings[0].mWaveLaneCount = QueryThreadExecutionWidth(pRenderer);
#ifndef TARGET_IOS
//    setGPUPresetLevel(pRenderer);
    //exit app if gpu being used has an office preset.
    if (pRenderer->pActiveGpuSettings->mGpuVendorPreset.mPresetLevel < GPU_PRESET_LOW) {
      ASSERT(pRenderer->pActiveGpuSettings->mGpuVendorPreset.mPresetLevel >= GPU_PRESET_LOW);

      //remove allocated name
      free(pRenderer->pName);
      //set device to null
      pRenderer->pDevice = nil;
      //remove allocated renderer
      free(pRenderer);

      LOGERROR("Selected GPU has an office Preset in gpu.cfg");
      LOGERROR("Office Preset is not supported by the Forge");

      ppRenderer = NULL;
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
    CreateAllocator(&info, (MetalResourceAllocator **) &pRenderer->pResourceAllocator);

    // Create default resources.
    CreateDefaultResources(pRenderer);

    // Renderer is good! Assign it to result!
    *(ppRenderer) = pRenderer;
  }
}

void RemoveRenderer(Renderer *pRenderer) {
  ASSERT(pRenderer);
  free(pRenderer->pName);
  DestroyAllocator((MetalResourceAllocator *) pRenderer->pResourceAllocator);
  pRenderer->pDevice = nil;
  free(pRenderer);
}

void AddFence(Renderer *pRenderer, Fence **ppFence) {
  ASSERT(pRenderer);
  ASSERT(pRenderer->pDevice != nil);

  Fence *pFence = (Fence *) calloc(1, sizeof(Fence));
  ASSERT(pFence);

  pFence->pMtlSemaphore = dispatch_semaphore_create(0);
  pFence->mSubmitted = false;

  *ppFence = pFence;
}

void RemoveFence(Renderer *pRenderer, Fence *pFence) {
  ASSERT(pFence);
  pFence->pMtlSemaphore = nil;
  free(pFence);
}

void AddSemaphore(Renderer *pRenderer, Semaphore **ppSemaphore) {
  ASSERT(pRenderer);

  Semaphore *pSemaphore = (Semaphore *) calloc(1, sizeof(Semaphore));
  ASSERT(pSemaphore);

  pSemaphore->pMtlSemaphore = dispatch_semaphore_create(0);

  *ppSemaphore = pSemaphore;
}
void RemoveSemaphore(Renderer *pRenderer, Semaphore *pSemaphore) {
  ASSERT(pSemaphore);
  pSemaphore->pMtlSemaphore = nil;
  free(pSemaphore);
}

void AddQueue(Renderer *pRenderer, QueueDesc *pQDesc, Queue **ppQueue) {
  ASSERT(pQDesc);

  Queue *pQueue = (Queue *) calloc(1, sizeof(Queue));
  ASSERT(pQueue);

  pQueue->pRenderer = pRenderer;
  pQueue->mtlCommandQueue = [pRenderer->pDevice newCommandQueue];

  ASSERT(pQueue->mtlCommandQueue != nil);

  *ppQueue = pQueue;
}
void RemoveQueue(Renderer *pRenderer, Queue *pQueue) {
  ASSERT(pQueue);
  pQueue->mtlCommandQueue = nil;
  free(pQueue);
}

void AddCmdPool(Renderer *pRenderer, Queue *pQueue, bool transient, CmdPool **ppCmdPool) {
  ASSERT(pRenderer);
  ASSERT(pRenderer->pDevice != nil);

  CmdPool *pCmdPool = (CmdPool *) calloc(1, sizeof(*pCmdPool));
  ASSERT(pCmdPool);

  pCmdPool->mCmdPoolDesc = {pQueue->mQueueDesc.mType};
  pCmdPool->pQueue = pQueue;

  *ppCmdPool = pCmdPool;
}
void RemoveCmdPool(Renderer *pRenderer, CmdPool *pCmdPool) {
  ASSERT(pCmdPool);
  free(pCmdPool);
}

void AddCmd(CmdPool *pCmdPool, bool secondary, Cmd **ppCmd) {
  ASSERT(pCmdPool);
  Renderer *renderer = (Renderer *) pCmdPool->pQueue->pRenderer;
  ASSERT(renderer->pDevice != nil);

  Cmd *pCmd = (Cmd *) calloc(1, sizeof(*pCmd));
  ASSERT(pCmd);

  pCmd->pRenderer = pCmdPool->pQueue->pRenderer;
  pCmd->pCmdPool = pCmdPool;
  pCmd->mtlEncoderFence = [renderer->pDevice newFence];

  if (pCmdPool->mCmdPoolDesc.mCmdPoolType == CMD_POOL_DIRECT) {
    pCmd->pBoundColorFormats = (uint32_t *) calloc(MAX_RENDER_TARGET_ATTACHMENTS, sizeof(uint32_t));
  }

  *ppCmd = pCmd;
}
void RemoveCmd(CmdPool *pCmdPool, Cmd *pCmd) {
  ASSERT(pCmd);
  pCmd->mtlEncoderFence = nil;
  pCmd->mtlCommandBuffer = nil;

  if (pCmd->pBoundColorFormats) {
    free(pCmd->pBoundColorFormats);
  }

  free(pCmd);
}

void AddCmd_n(CmdPool *pCmdPool, bool secondary, uint32_t cmdCount, Cmd ***pppCmd) {
  ASSERT(pppCmd);

  Cmd **ppCmd = (Cmd **) calloc(cmdCount, sizeof(*ppCmd));
  ASSERT(ppCmd);

  for (uint32_t i = 0; i < cmdCount; ++i) {
    AddCmd(pCmdPool, secondary, &(ppCmd[i]));
  }

  *pppCmd = ppCmd;
}
void RemoveCmd_n(CmdPool *pCmdPool, uint32_t cmdCount, Cmd **ppCmd) {
  ASSERT(ppCmd);

  for (uint32_t i = 0; i < cmdCount; ++i) {
    RemoveCmd(pCmdPool, ppCmd[i]);
  }

  free(ppCmd);
}


void AddBuffer(Renderer *pRenderer, const BufferDesc *pDesc, Buffer **ppBuffer) {
  ASSERT(pRenderer);
  ASSERT(pDesc);
  ASSERT(pDesc->mSize > 0);
  ASSERT(pRenderer->pDevice != nil);

  Buffer *pBuffer = (Buffer *) calloc(1, sizeof(Buffer));
  ASSERT(pBuffer);
  pBuffer->mDesc = *pDesc;

  // Align the buffer size to multiples of the dynamic uniform buffer minimum size
  if (pBuffer->mDesc.mDescriptors & DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
    uint64_t minAlignment = pRenderer->pActiveGpuSettings->mUniformBufferAlignment;
    pBuffer->mDesc.mSize = round_up_64(pBuffer->mDesc.mSize, minAlignment);
  }

  //Use isLowPower to determine if running intel integrated gpu
  //There's currently an intel driver bug with placed resources so we need to create
  //new resources that are GPU only in their own memory space
  //0x8086 is intel vendor id
  if (strcmp(pRenderer->pActiveGpuSettings->mGpuVendorPreset.mVendorId, "0x8086") == 0 &&
      (ResourceMemoryUsage) pBuffer->mDesc.mMemoryUsage & RESOURCE_MEMORY_USAGE_GPU_ONLY) {
    pBuffer->mDesc.mFlags |= BUFFER_CREATION_FLAG_OWN_MEMORY_BIT;
  }

  // Get the proper memory requiremnets for the given buffer.
  AllocatorMemoryRequirements mem_reqs = {0};
  mem_reqs.usage = (ResourceMemoryUsage) pBuffer->mDesc.mMemoryUsage;
  mem_reqs.flags = 0;
  if (pBuffer->mDesc.mFlags & BUFFER_CREATION_FLAG_OWN_MEMORY_BIT) {
    mem_reqs.flags |= RESOURCE_MEMORY_REQUIREMENT_OWN_MEMORY_BIT;
  }
  if (pBuffer->mDesc.mFlags & BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT) {
    mem_reqs.flags |= RESOURCE_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT;
  }

  BufferCreateInfo alloc_info = {pBuffer->mDesc.mSize};
  long allocSuccess;
  allocSuccess = CreateBuffer(pRenderer->pResourceAllocator, &alloc_info, &mem_reqs, pBuffer);
  ASSERT(allocSuccess);

  pBuffer->mBufferId = (++pRenderer->mBufferIds << 8U) +
      Util::PthreadToUint64(Os::Thread::GetCurrentThreadID());
  pBuffer->mCurrentState = pBuffer->mDesc.mStartState;

  // If buffer is a suballocation use offset in heap else use zero offset (placed resource / committed resource)
  if (pBuffer->pMtlAllocation->GetResource()) {
    pBuffer->mPositionInHeap = pBuffer->pMtlAllocation->GetOffset();
  } else {
    pBuffer->mPositionInHeap = 0;
  }

  *ppBuffer = pBuffer;
}
void RemoveBuffer(Renderer *pRenderer, Buffer *pBuffer) {
  ASSERT(pBuffer);
  DestroyBuffer(pRenderer->pResourceAllocator, pBuffer);
  free(pBuffer);
}

void AddRenderTarget(Renderer *pRenderer, const RenderTargetDesc *pDesc, RenderTarget **ppRenderTarget) {
  ASSERT(pRenderer);
  ASSERT(pDesc);
  ASSERT(ppRenderTarget);

  ((RenderTargetDesc *) pDesc)->mMipLevels = Math_MaxU32(1U, pDesc->mMipLevels);

  RenderTarget *pRenderTarget = (RenderTarget *) calloc(1, sizeof(*pRenderTarget));
  pRenderTarget->mDesc = *pDesc;

  TextureDesc rtDesc = {};
  rtDesc.mFlags = pRenderTarget->mDesc.mFlags;
  rtDesc.mWidth = pRenderTarget->mDesc.mWidth;
  rtDesc.mHeight = pRenderTarget->mDesc.mHeight;
  rtDesc.mDepth = pRenderTarget->mDesc.mDepth;
  rtDesc.mArraySize = pRenderTarget->mDesc.mArraySize;
  rtDesc.mMipLevels = pRenderTarget->mDesc.mMipLevels;
  rtDesc.mSampleCount = pRenderTarget->mDesc.mSampleCount;
  rtDesc.mSampleQuality = pRenderTarget->mDesc.mSampleQuality;
  rtDesc.mFormat = pRenderTarget->mDesc.mFormat;
  rtDesc.mClearValue = pRenderTarget->mDesc.mClearValue;
  rtDesc.mDescriptors = DESCRIPTOR_TYPE_TEXTURE;
  rtDesc.mStartState = RESOURCE_STATE_UNDEFINED;
  rtDesc.pNativeHandle = pDesc->pNativeHandle;
  rtDesc.mHostVisible = false;

  rtDesc.mDescriptors |= pDesc->mDescriptors;

#ifndef TARGET_IOS
  AddTexture(pRenderer, &rtDesc, (Texture **) &pRenderTarget->pTexture, true);
#else
  AddTexture(pRenderer, &rtDesc, &pRenderTarget->pTexture, true);
  // Combined depth stencil is not supported on iOS.
  else
  {
      rtDesc.mFormat = D24;
      AddTexture(pRenderer, &rtDesc, &pRenderTarget->pTexture, true);
      rtDesc.mFormat = S8;
      AddTexture(pRenderer, &rtDesc, &pRenderTarget->pStencil, true);
  }
#endif

  *ppRenderTarget = pRenderTarget;
}

void RemoveTexture(Renderer *pRenderer, Texture *pTexture) {
  ASSERT(pTexture);

  // Destroy descriptors
  if (pTexture->mDesc.mDescriptors & DESCRIPTOR_TYPE_RW_TEXTURE) {
    for (uint32_t i = 0; i < pTexture->mDesc.mMipLevels; ++i) {
      pTexture->pMtlUAVDescriptors[i] = nil;
    }
  }

  DestroyTexture(pRenderer->pResourceAllocator, pTexture);
  free(pTexture->pMtlUAVDescriptors);
  free(pTexture);
}

void RemoveRenderTarget(Renderer *pRenderer, RenderTarget *pRenderTarget) {
  RemoveTexture(pRenderer, (Texture *) pRenderTarget->pTexture);
#ifdef TARGET_IOS
                                                                                                                          if (pRenderTarget->pStencil)
        removeTexture(pRenderer, pRenderTarget->pStencil);
#endif
  free(pRenderTarget);
}

void AddSampler(Renderer *pRenderer, const SamplerDesc *pDesc, Sampler **ppSampler) {
  ASSERT(pRenderer);
  ASSERT(pRenderer->pDevice != nil);
  ASSERT(pDesc->mCompareFunc < MAX_COMPARE_MODES);

  Sampler *pSampler = (Sampler *) calloc(1, sizeof(*pSampler));
  ASSERT(pSampler);

  MTLSamplerDescriptor *samplerDesc = [[MTLSamplerDescriptor alloc] init];
  samplerDesc.minFilter =
      (pDesc->mMinFilter == FILTER_NEAREST ? MTLSamplerMinMagFilterNearest : MTLSamplerMinMagFilterLinear);
  samplerDesc.magFilter =
      (pDesc->mMagFilter == FILTER_NEAREST ? MTLSamplerMinMagFilterNearest : MTLSamplerMinMagFilterLinear);
  samplerDesc.mipFilter =
      (pDesc->mMipMapMode == MIPMAP_MODE_NEAREST ? MTLSamplerMipFilterNearest : MTLSamplerMipFilterLinear);
  samplerDesc.maxAnisotropy = (pDesc->mMaxAnisotropy == 0 ? 1 : pDesc->mMaxAnisotropy);    // 0 is not allowed in Metal
  samplerDesc.sAddressMode = gMtlAddressModeTranslator[pDesc->mAddressU];
  samplerDesc.tAddressMode = gMtlAddressModeTranslator[pDesc->mAddressV];
  samplerDesc.rAddressMode = gMtlAddressModeTranslator[pDesc->mAddressW];
  samplerDesc.compareFunction = gMtlComparisonFunctionTranslator[pDesc->mCompareFunc];

  pSampler->mtlSamplerState = [pRenderer->pDevice newSamplerStateWithDescriptor:samplerDesc];
  pSampler->mSamplerId = (++pRenderer->mSamplerIds << 8U) +
      Util::PthreadToUint64(Os::Thread::GetCurrentThreadID());

  *ppSampler = pSampler;
}

void RemoveSampler(Renderer *pRenderer, Sampler *pSampler) {
  ASSERT(pSampler);
  pSampler->mtlSamplerState = nil;
  free(pSampler);
}

void AddShaderBinary(Renderer *pRenderer,
    const BinaryShaderDesc *pDesc,
    Shader **ppShaderProgram) {
  ASSERT(pRenderer);
  ASSERT(pDesc && pDesc->mStages);
  ASSERT(ppShaderProgram);

  Shader *pShaderProgram = (Shader *) calloc(1, sizeof(*pShaderProgram));
  ASSERT(pShaderProgram);

  pShaderProgram->mStages = pDesc->mStages;

  tinystl::unordered_map<uint32_t, MTLVertexFormat> vertexAttributeFormats;

  uint32_t reflectionCount = 0;
  for (uint32_t i = 0; i < SHADER_STAGE_COUNT; ++i) {
    ShaderStage stage_mask = (ShaderStage) (1 << i);
    const BinaryShaderStageDesc *pStage = NULL;
    __strong id <MTLFunction> *compiled_code = nullptr;
    tinystl::string *entryPointName = nullptr;

    if (stage_mask == (pShaderProgram->mStages & stage_mask)) {
      switch (stage_mask) {
        case SHADER_STAGE_VERT: {
          pStage = &pDesc->mVert;
          compiled_code = &(pShaderProgram->mtlVertexShader);
          entryPointName = &(pShaderProgram->mtlVertexShaderEntryPoint);
        }
          break;
        case SHADER_STAGE_FRAG: {
          pStage = &pDesc->mFrag;
          compiled_code = &(pShaderProgram->mtlFragmentShader);
          entryPointName = &(pShaderProgram->mtlFragmentShaderEntryPoint);
        }
          break;
        case SHADER_STAGE_COMP: {
          pStage = &pDesc->mComp;
          compiled_code = &(pShaderProgram->mtlComputeShader);
          entryPointName = &(pShaderProgram->mtlComputeShaderEntryPoint);
        }
          break;
        default: break;
      }

      // Create a MTLLibrary from bytecode.
      dispatch_data_t byteCode =
          dispatch_data_create(pStage->pByteCode, pStage->mByteCodeSize, nil, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
      id <MTLLibrary> lib = [pRenderer->pDevice newLibraryWithData:byteCode error:nil];

      // Create a MTLFunction from the loaded MTLLibrary.
      NSString *entryPointNStr = [[NSString alloc] initWithUTF8String:pStage->mEntryPoint];
      id <MTLFunction> function = [lib newFunctionWithName:entryPointNStr];
      *compiled_code = function;
      *entryPointName = pStage->mEntryPoint;
    }
  }
  *ppShaderProgram = pShaderProgram;
}

void RemoveShader(Renderer *pRenderer, Shader *pShaderProgram) {
  ASSERT(pShaderProgram);
  pShaderProgram->mtlVertexShader = nil;
  pShaderProgram->mtlFragmentShader = nil;
  pShaderProgram->mtlComputeShader = nil;

  free(pShaderProgram);
}

void AddGraphicsComputeRootSignature(Renderer *pRenderer,
                                     const RootSignatureDesc *pRootSignatureDesc,
                                     RootSignature **ppRootSignature) {
  ASSERT(pRenderer);
  ASSERT(pRenderer->pDevice != nil);

  RootSignature *pRootSignature = (RootSignature *) calloc(1, sizeof(*pRootSignature));
  tinystl::vector<ShaderResource const *> shaderResources;

  // Collect static samplers
  tinystl::vector<tinystl::pair<ShaderResource const *, Sampler *>> staticSamplers;
  typedef tinystl::unordered_map<tinystl::string, Sampler *> ssm_type;
  ssm_type staticSamplerMap;

  for (uint32_t i = 0; i < pRootSignatureDesc->mStaticSamplerCount; ++i) {
    staticSamplerMap.insert(
        {pRootSignatureDesc->ppStaticSamplerNames[i],
         (Sampler *) pRootSignatureDesc->ppStaticSamplers[i]});
  }

  stb_udict32_init(&pRootSignature->pDescriptorNameToIndexMap, MaxDescriptorNamesPerRoot);

  // Collect all unique shader resources in the given shaders
  // Resources are parsed by name (two resources named "XYZ" in two shaders will be considered the same resource)
  for (uint32_t sh = 0; sh < pRootSignatureDesc->mShaderCount; ++sh) {
    TheForge_Shader const *pShader = pRootSignatureDesc->ppShaders[sh];

    if (pShader->mStages & SHADER_STAGE_COMP) {
      pRootSignature->mPipelineType = PIPELINE_TYPE_COMPUTE;
    } else {
      pRootSignature->mPipelineType = PIPELINE_TYPE_GRAPHICS;
    }

    for (uint32_t i = 0; i < pShader->mShaderResourceCount; ++i) {
      ShaderResource const *pRes = &pShader->pShaderResources[i];

      // Find all unique resources
      uint32_t index;
      int found = stb_udict32_get_flag(&pRootSignature->pDescriptorNameToIndexMap, stb_hash(pRes->name), &index);
      if (found) {
        if (pRes->type == DESCRIPTOR_TYPE_SAMPLER) {
          // If the sampler is a static sampler, no need to put it in the descriptor table
          ssm_type::const_hash_node *pNode = staticSamplerMap.find(pRes->name).node;

          if (pNode) {
            LOGINFOF("Descriptor (%s) : User specified Static Sampler", pRes->name);
            staticSamplers.push_back({pRes, pNode->second});
          } else {
            stb_udict32_add(&pRootSignature->pDescriptorNameToIndexMap,
                            stb_hash(pRes->name),
                            (uint32_t) shaderResources.size());
            shaderResources.emplace_back(pRes);
          }
        } else {
          stb_udict32_add(&pRootSignature->pDescriptorNameToIndexMap,
                          stb_hash(pRes->name),
                          (uint32_t) shaderResources.size());
          shaderResources.emplace_back(pRes);
        }
      }
    }
  }

  if ((uint32_t) shaderResources.size()) {
    pRootSignature->mDescriptorCount = (uint32_t) shaderResources.size();
    pRootSignature->pDescriptors = (DescriptorInfo *) calloc(pRootSignature->mDescriptorCount, sizeof(DescriptorInfo));
  }

  // Fill the descriptor array to be stored in the root signature
  for (uint32_t i = 0; i < (uint32_t) shaderResources.size(); ++i) {
    DescriptorInfo *pDesc = &pRootSignature->pDescriptors[i];
    ShaderResource const *pRes = shaderResources[i];
    uint32_t setIndex = pRes->set;
    DescriptorUpdateFrequency updateFreq = (DescriptorUpdateFrequency) setIndex;

    pDesc->mDesc.reg = pRes->reg;
    pDesc->mDesc.set = pRes->set;
    pDesc->mDesc.size = pRes->size;
    pDesc->mDesc.type = pRes->type;
    pDesc->mDesc.used_stages = pRes->used_stages;
    pDesc->mDesc.name_size = pRes->name_size;
    pDesc->mDesc.name = (const char *) calloc(pDesc->mDesc.name_size + 1, sizeof(char));
    memcpy((char *) pDesc->mDesc.name, pRes->name, pRes->name_size);
    pDesc->mUpdateFrquency = updateFreq;

    // In case we're binding a texture, we need to specify the texture type so the bound resource type matches the one defined in the shader.
    if (pRes->type == DESCRIPTOR_TYPE_TEXTURE || pRes->type == DESCRIPTOR_TYPE_RW_TEXTURE) {
      pDesc->mDesc.backend.mtlTextureType = pRes->backend.mtlTextureType;
    }

    // If we're binding an argument buffer, we also need to get the type of the resources that this buffer will store.
    if (pRes->backend.mtlArgumentBufferType != DESCRIPTOR_TYPE_UNDEFINED) {
      pDesc->mDesc.backend.mtlArgumentBufferType = pRes->backend.mtlArgumentBufferType;
    }
  }

  pRootSignature->mStaticSamplerCount = (uint32_t) staticSamplers.size();
  pRootSignature->ppStaticSamplers = (Sampler **) calloc(staticSamplers.size(), sizeof(Sampler *));
  pRootSignature->pStaticSamplerStages = (ShaderStage *) calloc(staticSamplers.size(), sizeof(ShaderStage));
  pRootSignature->pStaticSamplerSlots = (uint32_t *) calloc(staticSamplers.size(), sizeof(uint32_t));
  for (uint32_t i = 0; i < pRootSignature->mStaticSamplerCount; ++i) {
    pRootSignature->ppStaticSamplers[i] = staticSamplers[i].second;
    pRootSignature->pStaticSamplerStages[i] = staticSamplers[i].first->used_stages;
    pRootSignature->pStaticSamplerSlots[i] = staticSamplers[i].first->reg;
  }

  // Create descriptor manager for this thread.
  DescriptorManager *pManager = NULL;
  AddDescriptorManager(pRenderer, pRootSignature, &pManager);
  stb_ptrmap_add(&pRootSignature->pDescriptorManagerMap,
                 Os::Thread::GetCurrentThreadID(), pManager);

  *ppRootSignature = pRootSignature;
}

extern void AddRaytracingRootSignature(Renderer *pRenderer,
                                       const ShaderResource *pResources,
                                       uint32_t resourceCount,
                                       bool local,
                                       RootSignature **ppRootSignature,
                                       const RootSignatureDesc *pRootDesc = nullptr);

void AddRootSignature(Renderer *pRenderer,
                      const RootSignatureDesc *pRootSignatureDesc,
                      RootSignature **ppRootSignature) {
  switch (pRootSignatureDesc->mSignatureType) {
    case (ROOT_SIGNATURE_GRAPHICS_COMPUTE): {
      AddGraphicsComputeRootSignature(pRenderer, pRootSignatureDesc, ppRootSignature);
      break;
    }
    case (ROOT_SIGNATURE_RAYTRACING_LOCAL): {
      AddRaytracingRootSignature(pRenderer,
                                 (ShaderResource *) pRootSignatureDesc->pRaytracingShaderResources,
                                 pRootSignatureDesc->pRaytracingResourcesCount,
                                 true,
                                 ppRootSignature,
                                 pRootSignatureDesc);
      break;
    }
    case (ROOT_SIGNATURE_RAYTRACING_GLOBAL): {
      AddRaytracingRootSignature(pRenderer,
                                 (ShaderResource *) pRootSignatureDesc->pRaytracingShaderResources,
                                 pRootSignatureDesc->pRaytracingResourcesCount,
                                 false,
                                 ppRootSignature,
                                 pRootSignatureDesc);
      break;
    }
    default: {
      ASSERT(false);
    }
  }
}

void RemoveRootSignature(Renderer *pRenderer, RootSignature *pRootSignature) {

  stb_ptrmap *ptrmap = &pRootSignature->pDescriptorManagerMap;
  for (int i = 0; i < ptrmap->count; ++i) {
    RemoveDescriptorManager(pRenderer,
                            pRootSignature,
                            (DescriptorManager *) ptrmap->table[i].v);
  }

  stb_ptrmap_destroy(&pRootSignature->pDescriptorManagerMap);
  stb_udict32_destroy(&pRootSignature->pDescriptorNameToIndexMap);

  free(pRootSignature->ppStaticSamplers);
  free(pRootSignature->pStaticSamplerStages);
  free(pRootSignature->pStaticSamplerSlots);
  free(pRootSignature);
}

void AddGraphicsPipelineImpl(Renderer *pRenderer,
                             const GraphicsPipelineDesc *pDesc,
                             Pipeline **ppPipeline) {
  ASSERT(pRenderer);
  ASSERT(pRenderer->pDevice != nil);
  ASSERT(pDesc);
  ASSERT(pDesc->pShaderProgram);
  ASSERT(pDesc->pRootSignature);

  Pipeline *pPipeline = (Pipeline *) calloc(1, sizeof(*pPipeline));
  ASSERT(pPipeline);

  memcpy(&(pPipeline->mGraphics), pDesc, sizeof(*pDesc));
  pPipeline->mType = PIPELINE_TYPE_GRAPHICS;
  pPipeline->pShader = (Shader *) pPipeline->mGraphics.pShaderProgram;

  // create metal pipeline descriptor
  MTLRenderPipelineDescriptor *renderPipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
  renderPipelineDesc.vertexFunction = pPipeline->pShader->mtlVertexShader;
  renderPipelineDesc.fragmentFunction = pPipeline->pShader->mtlFragmentShader;
  renderPipelineDesc.sampleCount = pDesc->mSampleCount;

  uint32_t inputBindingCount = 0;
  // add vertex layout to descriptor
  if (pPipeline->mGraphics.pVertexLayout != nil) {
    uint32_t bindingValue = UINT32_MAX;
    // setup vertex descriptors
    for (uint i = 0; i < pPipeline->mGraphics.pVertexLayout->mAttribCount; i++) {
      const VertexAttrib *attrib = pPipeline->mGraphics.pVertexLayout->mAttribs + i;

      if (bindingValue != attrib->mBinding) {
        bindingValue = attrib->mBinding;
        inputBindingCount++;
      }

      renderPipelineDesc.vertexDescriptor.attributes[i].offset = attrib->mOffset;
      renderPipelineDesc.vertexDescriptor.attributes[i].bufferIndex = attrib->mBinding;
      renderPipelineDesc.vertexDescriptor.attributes[i].format = Util::ToMtlVertexFormat(attrib->mFormat);

      //setup layout for all bindings instead of just 0.
      renderPipelineDesc.vertexDescriptor.layouts[inputBindingCount - 1].stride +=
          Image_Format_BitWidth(attrib->mFormat) / 8;
      renderPipelineDesc.vertexDescriptor.layouts[inputBindingCount - 1].stepRate = 1;
      renderPipelineDesc.vertexDescriptor.layouts[inputBindingCount - 1].stepFunction =
          pPipeline->pShader->mtlVertexShader.patchType != MTLPatchTypeNone ? MTLVertexStepFunctionPerPatchControlPoint
                                                                            : MTLVertexStepFunctionPerVertex;
    }
  }

#if !defined(TARGET_IOS)
  // add pipeline settings to descriptor
  switch (pDesc->mPrimitiveTopo) {
    case PRIMITIVE_TOPO_POINT_LIST: renderPipelineDesc.inputPrimitiveTopology = MTLPrimitiveTopologyClassPoint;
      break;
    case PRIMITIVE_TOPO_LINE_LIST: renderPipelineDesc.inputPrimitiveTopology = MTLPrimitiveTopologyClassLine;
      break;
    case PRIMITIVE_TOPO_LINE_STRIP: renderPipelineDesc.inputPrimitiveTopology = MTLPrimitiveTopologyClassLine;
      break;
    case PRIMITIVE_TOPO_TRI_LIST: renderPipelineDesc.inputPrimitiveTopology = MTLPrimitiveTopologyClassTriangle;
      break;
    case PRIMITIVE_TOPO_TRI_STRIP: renderPipelineDesc.inputPrimitiveTopology = MTLPrimitiveTopologyClassTriangle;
      break;
    default: renderPipelineDesc.inputPrimitiveTopology = MTLPrimitiveTopologyClassTriangle;
      break;
  }
#endif

  // assign render target pixel format for all attachments
  const BlendState *blendState = pDesc->pBlendState ?
                                 (Metal::BlendState *) pDesc->pBlendState :
                                 pRenderer->pDefaultBlendState;
  for (uint32_t i = 0; i < pDesc->mRenderTargetCount; i++) {
    renderPipelineDesc.colorAttachments[i].pixelFormat =
        Util::ToMtlPixelFormat(pDesc->pColorFormats[i]);

    // set blend state
    bool hasBlendState = (blendState != nil);
    renderPipelineDesc.colorAttachments[i].blendingEnabled = hasBlendState;
    if (hasBlendState) {
      renderPipelineDesc.colorAttachments[i].rgbBlendOperation = blendState->blendStatePerRenderTarget[i].blendMode;
      renderPipelineDesc.colorAttachments[i].alphaBlendOperation =
          blendState->blendStatePerRenderTarget[i].blendAlphaMode;
      renderPipelineDesc.colorAttachments[i].sourceRGBBlendFactor = blendState->blendStatePerRenderTarget[i].srcFactor;
      renderPipelineDesc.colorAttachments[i].destinationRGBBlendFactor =
          blendState->blendStatePerRenderTarget[i].destFactor;
      renderPipelineDesc.colorAttachments[i].sourceAlphaBlendFactor =
          blendState->blendStatePerRenderTarget[i].srcAlphaFactor;
      renderPipelineDesc.colorAttachments[i].destinationAlphaBlendFactor =
          blendState->blendStatePerRenderTarget[i].destAlphaFactor;
    }
  }

  // assign pixel format form depth attachment
  if (pDesc->mDepthStencilFormat != NONE) {
    renderPipelineDesc.depthAttachmentPixelFormat = Util::ToMtlPixelFormat(pDesc->mDepthStencilFormat);
#ifndef TARGET_IOS
    if (renderPipelineDesc.depthAttachmentPixelFormat == MTLPixelFormatDepth24Unorm_Stencil8) {
      renderPipelineDesc.stencilAttachmentPixelFormat = renderPipelineDesc.depthAttachmentPixelFormat;
    }
#else
                                                                                                                            if (pDesc->mDepthStencilFormat == D24S8)
            renderPipelineDesc.stencilAttachmentPixelFormat = MTLPixelFormatStencil8;
#endif
  }
  if (pDesc->pRasterizerState) {
    pPipeline->mGraphics.pRasterizerState = pDesc->pRasterizerState;
  } else {
    pPipeline->mGraphics.pRasterizerState = pRenderer->pDefaultRasterizerState;
  }
  if (pDesc->pDepthState) {
    pPipeline->mGraphics.pDepthState = pDesc->pDepthState;
  } else {
    pPipeline->mGraphics.pDepthState = pRenderer->pDefaultDepthState;
  }

  // assign common tesselation configuration if needed.
  if (pPipeline->pShader->mtlVertexShader.patchType != MTLPatchTypeNone) {
    renderPipelineDesc.tessellationFactorScaleEnabled = NO;
    renderPipelineDesc.tessellationFactorFormat = MTLTessellationFactorFormatHalf;
    renderPipelineDesc.tessellationControlPointIndexType = MTLTessellationControlPointIndexTypeNone;
    renderPipelineDesc.tessellationFactorStepFunction = MTLTessellationFactorStepFunctionConstant;
    renderPipelineDesc.tessellationOutputWindingOrder = MTLWindingClockwise;
    renderPipelineDesc.tessellationPartitionMode = MTLTessellationPartitionModeFractionalEven;
#if TARGET_OS_IOS
                                                                                                                            // In iOS, the maximum tessellation factor is 16
        renderPipelineDesc.maxTessellationFactor = 16;
#elif TARGET_OS_OSX
    // In OS X, the maximum tessellation factor is 64
    renderPipelineDesc.maxTessellationFactor = 64;
#endif
  }

  // create pipeline from descriptor
  NSError *error = nil;
  pPipeline->mtlRenderPipelineState = [pRenderer->pDevice newRenderPipelineStateWithDescriptor:renderPipelineDesc
                                                                                       options:MTLPipelineOptionNone
                                                                                    reflection:nil
                                                                                         error:&error];
  if (!pPipeline->mtlRenderPipelineState) {
    LOGERRORF("Failed to create render pipeline state, error:\n%s", [[error localizedDescription] UTF8String]);
    return;
  }

  *ppPipeline = pPipeline;
}

void AddComputePipelineImpl(Renderer *pRenderer, const ComputePipelineDesc *pDesc, Pipeline **ppPipeline) {
  ASSERT(pRenderer);
  ASSERT(pRenderer->pDevice != nil);
  ASSERT(pDesc);
  ASSERT(pDesc->pShaderProgram);
  ASSERT(pDesc->pRootSignature);

  Pipeline *pPipeline = (Pipeline *) calloc(1, sizeof(*pPipeline));
  ASSERT(pPipeline);

  memcpy(&(pPipeline->mCompute), pDesc, sizeof(*pDesc));
  pPipeline->mType = PIPELINE_TYPE_COMPUTE;
  pPipeline->pShader = (Shader *) pPipeline->mCompute.pShaderProgram;

  NSError *error = nil;
  pPipeline->mtlComputePipelineState =
      [pRenderer->pDevice newComputePipelineStateWithFunction:pPipeline->pShader->mtlComputeShader error:&error];
  if (!pPipeline->mtlComputePipelineState) {
    LOGERRORF("Failed to create compute pipeline state, error:\n%s", [[error localizedDescription] UTF8String]);
    free(pPipeline);
    return;
  }

  *ppPipeline = pPipeline;
}

void AddComputePipeline(Renderer *pRenderer, const ComputePipelineDesc *pDesc, Pipeline **ppPipeline) {
  AddComputePipelineImpl(pRenderer, pDesc, ppPipeline);
}

void AddPipeline(Renderer *pRenderer, const GraphicsPipelineDesc *pDesc, Pipeline **ppPipeline) {
  AddGraphicsPipelineImpl(pRenderer, pDesc, ppPipeline);
}

extern void AddRaytracingPipeline(const RaytracingPipelineDesc *pDesc, Pipeline **ppPipeline);
extern void RemoveRaytracingPipeline(RaytracingPipeline *pPipeline);

void AddPipeline(Renderer *pRenderer, const PipelineDesc *pDesc, Pipeline **ppPipeline) {
  ASSERT(pRenderer);
  ASSERT(pRenderer->pDevice != nil);

  switch (pDesc->mType) {
    case (PIPELINE_TYPE_COMPUTE): {
      AddComputePipelineImpl(pRenderer, &pDesc->mComputeDesc, ppPipeline);
      break;
    }
    case (PIPELINE_TYPE_GRAPHICS): {
      AddGraphicsPipelineImpl(pRenderer, &pDesc->mGraphicsDesc, ppPipeline);
      break;
    }
    case (PIPELINE_TYPE_RAYTRACING): {
      AddRaytracingPipeline(&pDesc->mRaytracingDesc, ppPipeline);
      break;
    }
    default:break;
  }
}

void RemovePipeline(Renderer *pRenderer, Pipeline *pPipeline) {
  ASSERT(pPipeline);
  pPipeline->mtlRenderPipelineState = nil;
  pPipeline->mtlComputePipelineState = nil;
  if (pPipeline->mType == PIPELINE_TYPE_RAYTRACING) {
    RemoveRaytracingPipeline(pPipeline->pRaytracingPipeline);
  }
  free(pPipeline);
}

void AddBlendState(Renderer *pRenderer, const BlendStateDesc *pDesc, BlendState **ppBlendState) {
  int blendDescIndex = 0;

  BlendState blendState = {};

  // Go over each RT blend state.
  for (int i = 0; i < MAX_RENDER_TARGET_ATTACHMENTS; ++i) {
    if (pDesc->mRenderTargetMask & (1 << i)) {
      blendState.blendStatePerRenderTarget[i].srcFactor =
          gMtlBlendConstantTranslator[pDesc->mSrcFactors[blendDescIndex]];
      blendState.blendStatePerRenderTarget[i].destFactor =
          gMtlBlendConstantTranslator[pDesc->mDstFactors[blendDescIndex]];
      blendState.blendStatePerRenderTarget[i].srcAlphaFactor =
          gMtlBlendConstantTranslator[pDesc->mSrcAlphaFactors[blendDescIndex]];
      blendState.blendStatePerRenderTarget[i].destAlphaFactor =
          gMtlBlendConstantTranslator[pDesc->mDstAlphaFactors[blendDescIndex]];
      blendState.blendStatePerRenderTarget[i].blendMode = gMtlBlendOpTranslator[pDesc->mBlendModes[blendDescIndex]];
      blendState.blendStatePerRenderTarget[i].blendAlphaMode =
          gMtlBlendOpTranslator[pDesc->mBlendAlphaModes[blendDescIndex]];
    }

    if (pDesc->mIndependentBlend) {
      ++blendDescIndex;
    }
  }
  blendState.alphaToCoverage = pDesc->mAlphaToCoverage;

  *ppBlendState = (BlendState *) malloc(sizeof(blendState));
  memcpy(*ppBlendState, &blendState, sizeof(blendState));
}

void RemoveBlendState(Renderer *pRenderer, BlendState *pBlendState) {
  ASSERT(pBlendState);
  free(pBlendState);
}

void AddDepthState(Renderer *pRenderer, const DepthStateDesc *pDesc, DepthState **ppDepthState) {
  ASSERT(pDesc->mDepthFunc < MAX_COMPARE_MODES);
  ASSERT(pDesc->mStencilFrontFunc < MAX_COMPARE_MODES);
  ASSERT(pDesc->mStencilFrontFail < MAX_STENCIL_OPS);
  ASSERT(pDesc->mDepthFrontFail < MAX_STENCIL_OPS);
  ASSERT(pDesc->mStencilFrontPass < MAX_STENCIL_OPS);
  ASSERT(pDesc->mStencilBackFunc < MAX_COMPARE_MODES);
  ASSERT(pDesc->mStencilBackFail < MAX_STENCIL_OPS);
  ASSERT(pDesc->mDepthBackFail < MAX_STENCIL_OPS);
  ASSERT(pDesc->mStencilBackPass < MAX_STENCIL_OPS);

  MTLDepthStencilDescriptor *descriptor = [[MTLDepthStencilDescriptor alloc] init];
  descriptor.depthCompareFunction = gMtlComparisonFunctionTranslator[pDesc->mDepthFunc];
  descriptor.depthWriteEnabled = pDesc->mDepthWrite;
  descriptor.backFaceStencil.stencilCompareFunction = gMtlComparisonFunctionTranslator[pDesc->mStencilBackFunc];
  descriptor.backFaceStencil.depthFailureOperation = gMtlStencilOpTranslator[pDesc->mDepthBackFail];
  descriptor.backFaceStencil.stencilFailureOperation = gMtlStencilOpTranslator[pDesc->mStencilBackFail];
  descriptor.backFaceStencil.depthStencilPassOperation = gMtlStencilOpTranslator[pDesc->mStencilBackPass];
  descriptor.backFaceStencil.readMask = pDesc->mStencilReadMask;
  descriptor.backFaceStencil.writeMask = pDesc->mStencilWriteMask;
  descriptor.frontFaceStencil.stencilCompareFunction = gMtlComparisonFunctionTranslator[pDesc->mStencilFrontFunc];
  descriptor.frontFaceStencil.depthFailureOperation = gMtlStencilOpTranslator[pDesc->mDepthFrontFail];
  descriptor.frontFaceStencil.stencilFailureOperation = gMtlStencilOpTranslator[pDesc->mStencilFrontFail];
  descriptor.frontFaceStencil.depthStencilPassOperation = gMtlStencilOpTranslator[pDesc->mStencilFrontPass];
  descriptor.frontFaceStencil.readMask = pDesc->mStencilReadMask;
  descriptor.frontFaceStencil.writeMask = pDesc->mStencilWriteMask;

  DepthState *pDepthState = (DepthState *) calloc(1, sizeof(*pDepthState));
  pDepthState->mtlDepthState = [pRenderer->pDevice newDepthStencilStateWithDescriptor:descriptor];

  *ppDepthState = pDepthState;
}

void RemoveDepthState(Renderer *pRenderer, DepthState *pDepthState) {
  ASSERT(pDepthState);
  pDepthState->mtlDepthState = nil;
  free(pDepthState);
}

void AddRasterizerState(Renderer *pRenderer, const RasterizerStateDesc *pDesc, RasterizerState **ppRasterizerState) {
  ASSERT(pDesc->mFillMode < MAX_FILL_MODES);
  ASSERT(pDesc->mCullMode < MAX_CULL_MODES);
  ASSERT(pDesc->mFrontFace == FRONT_FACE_CCW || pDesc->mFrontFace == FRONT_FACE_CW);

  RasterizerState rasterizerState = {};

  rasterizerState.cullMode = MTLCullModeNone;
  if (pDesc->mCullMode == CULL_MODE_BACK) {
    rasterizerState.cullMode = MTLCullModeBack;
  } else if (pDesc->mCullMode == CULL_MODE_FRONT) {
    rasterizerState.cullMode = MTLCullModeFront;
  }

  rasterizerState.fillMode = (pDesc->mFillMode == FILL_MODE_SOLID ? MTLTriangleFillModeFill : MTLTriangleFillModeLines);
  rasterizerState.depthBias = pDesc->mDepthBias;
  rasterizerState.depthBiasSlopeFactor = pDesc->mSlopeScaledDepthBias;
  rasterizerState.scissorEnable = pDesc->mScissor;
  rasterizerState.multisampleEnable = pDesc->mMultiSample;
  rasterizerState.frontFace = (pDesc->mFrontFace == FRONT_FACE_CCW ? MTLWindingCounterClockwise : MTLWindingClockwise);

  *ppRasterizerState = (RasterizerState *) malloc(sizeof(rasterizerState));
  memcpy(*ppRasterizerState, &rasterizerState, sizeof(rasterizerState));
}

void RemoveRasterizerState(Renderer *pRenderer, RasterizerState *pRasterizerState) {
  ASSERT(pRasterizerState);
  free(pRasterizerState);
}

void AddIndirectCommandSignature(Renderer *pRenderer,
                                 const CommandSignatureDesc *pDesc,
                                 CommandSignature **ppCommandSignature) {
  assert(pRenderer != nil);
  assert(pDesc != nil);

  CommandSignature *pCommandSignature = (CommandSignature *) calloc(1, sizeof(CommandSignature));

  for (uint32_t i = 0; i < pDesc->mIndirectArgCount; i++) {
    const IndirectArgumentDescriptor *argDesc = pDesc->pArgDescs + i;
    if (argDesc->mType != INDIRECT_DRAW && argDesc->mType != INDIRECT_DISPATCH
        && argDesc->mType != INDIRECT_DRAW_INDEX) {
      assert(!"Unsupported indirect argument type.");
      free(pCommandSignature);
      return;
    }

    if (i == 0) {
      pCommandSignature->mDrawType = argDesc->mType;
    } else if (pCommandSignature->mDrawType != argDesc->mType) {
      assert(!"All elements in the root signature must be of the same type.");
      free(pCommandSignature);
      return;
    }
  }
  pCommandSignature->mIndirectArgDescCounts = pDesc->mIndirectArgCount;

  *ppCommandSignature = pCommandSignature;
}

void RemoveIndirectCommandSignature(Renderer *pRenderer, CommandSignature *pCommandSignature) {
  ASSERT(pCommandSignature);
  free(pCommandSignature);
}

// -------------------------------------------------------------------------------------------------
// Buffer functions
// -------------------------------------------------------------------------------------------------
void MapBuffer(Renderer *pRenderer, Buffer *pBuffer, ReadRange *pRange) {
  ASSERT(pBuffer->mDesc.mMemoryUsage != RESOURCE_MEMORY_USAGE_GPU_ONLY && "Trying to map non-cpu accessible resource");
  pBuffer->pCpuMappedAddress = pBuffer->mtlBuffer.contents;
}

void UnmapBuffer(Renderer *pRenderer, Buffer *pBuffer) {
  ASSERT(
      pBuffer->mDesc.mMemoryUsage != RESOURCE_MEMORY_USAGE_GPU_ONLY && "Trying to unmap non-cpu accessible resource");
  pBuffer->pCpuMappedAddress = nil;
}

void QueueSubmit(
    Queue *pQueue,
    uint32_t cmdCount,
    Cmd **ppCmds,
    Fence *pFence,
    uint32_t waitSemaphoreCount,
    Semaphore **ppWaitSemaphores,
    uint32_t signalSemaphoreCount,
    Semaphore **ppSignalSemaphores) {
  ASSERT(pQueue);
  ASSERT(cmdCount > 0);
  ASSERT(ppCmds);
  if (waitSemaphoreCount > 0) {
    ASSERT(ppWaitSemaphores);
  }
  if (signalSemaphoreCount > 0) {
    ASSERT(ppSignalSemaphores);
  }

  // set the queue built-in semaphore to signal when all command lists finished their execution
  __block uint32_t commandsFinished = 0;
  __weak dispatch_semaphore_t completedFence = nil;
  if (pFence) {
    completedFence = pFence->pMtlSemaphore;
    pFence->mSubmitted = true;
  }
  for (uint32_t i = 0; i < cmdCount; i++) {
    [ppCmds[i]->mtlCommandBuffer addCompletedHandler:^(id <MTLCommandBuffer> buffer) {
      commandsFinished++;
      if (commandsFinished == cmdCount) {
        if (completedFence) {
          dispatch_semaphore_signal(completedFence);
        }
      }
    }];
  }

  // commit the command lists
  for (uint32_t i = 0; i < cmdCount; i++) {
    // register the following semaphores for signaling after the work has been done
    for (uint32_t j = 0; j < signalSemaphoreCount; j++) {
      __weak dispatch_semaphore_t blockSemaphore = ppSignalSemaphores[j]->pMtlSemaphore;
      [ppCmds[i]->mtlCommandBuffer addCompletedHandler:^(id <MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(blockSemaphore);
      }];
    }

    // Commit any uncommited encoder. This is necessary before committing the command buffer
    Util::EndCurrentEncoders(ppCmds[i]);
    [ppCmds[i]->mtlCommandBuffer commit];
  }
}

void WaitForFences(Renderer *pRenderer, uint32_t fenceCount, Fence **ppFences) {
  ASSERT(pRenderer);
  ASSERT(fenceCount);
  ASSERT(ppFences);

  for (uint32_t i = 0; i < fenceCount; i++) {
    if (ppFences[i]->mSubmitted) {
      dispatch_semaphore_wait(ppFences[i]->pMtlSemaphore, DISPATCH_TIME_FOREVER);
    }
    ppFences[i]->mSubmitted = false;
  }
}

void WaitQueueIdle(Queue *pQueue) {
  ASSERT(pQueue);
  dispatch_semaphore_t queueCompletedSemaphore = dispatch_semaphore_create(0);
  id <MTLCommandBuffer> waitCmdBuf = [pQueue->mtlCommandQueue commandBufferWithUnretainedReferences];

  [waitCmdBuf addCompletedHandler:^(id <MTLCommandBuffer> mtlCmdBuff) {
    dispatch_semaphore_signal(queueCompletedSemaphore);
  }];

  [waitCmdBuf commit];

  dispatch_semaphore_wait(queueCompletedSemaphore, DISPATCH_TIME_FOREVER);

  queueCompletedSemaphore = nil;
}

void GetFenceStatus(Renderer *pRenderer, Fence *pFence, FenceStatus *pFenceStatus) {
  ASSERT(pFence);
  *pFenceStatus = FENCE_STATUS_COMPLETE;
  if (pFence->mSubmitted) {
    // Check the fence status (and mark it as unsubmitted it if it has succesfully decremented).
    long status = dispatch_semaphore_wait(pFence->pMtlSemaphore, DISPATCH_TIME_NOW);
    if (status == 0) {
      pFence->mSubmitted = false;
    }

    *pFenceStatus = (status == 0 ? FENCE_STATUS_COMPLETE : FENCE_STATUS_INCOMPLETE);
  }
}

void GetRawTextureHandle(Renderer *pRenderer, Texture *pTexture, void **ppHandle) {
  ASSERT(pRenderer);
  ASSERT(pTexture);
  ASSERT(ppHandle);

  *ppHandle = (void *) CFBridgingRetain(pTexture->mtlTexture);
}

void AddTexture(Renderer *pRenderer,
                const TextureDesc *pDesc,
                Texture **ppTexture,
                const bool isRT,
                const bool forceNonPrivate) {
  ASSERT(pRenderer);
  ASSERT(pDesc && pDesc->mWidth && pDesc->mHeight && (pDesc->mDepth || pDesc->mArraySize));
  if (pDesc->mSampleCount > SAMPLE_COUNT_1 && pDesc->mMipLevels > 1) {
    InternalLog(LOG_TYPE_ERROR, "Multi-Sampled textures cannot have mip maps", "MetalRenderer");
    return;
  }

  Texture *pTexture = (Texture *) calloc(1, sizeof(*pTexture));
  ASSERT(pTexture);

  if (pDesc->mHeight == 1) {
    ((TextureDesc *) pDesc)->mMipLevels = 1;
  }

  pTexture->mDesc = *pDesc;
  pTexture->mTextureId = (++pRenderer->mTextureIds << 8U) +
      Util::PthreadToUint64(Os::Thread::GetCurrentThreadID());

  pTexture->mtlPixelFormat = Util::ToMtlPixelFormat(pTexture->mDesc.mFormat);
#ifndef TARGET_IOS
  if (pTexture->mtlPixelFormat == MTLPixelFormatDepth24Unorm_Stencil8
      && ![pRenderer->pDevice isDepth24Stencil8PixelFormatSupported]) {
    InternalLog(LOG_TYPE_WARN, "Format D24S8 is not supported on this device. Using D32_SFLOAT_S8_UINT instead", "addTexture");
    pTexture->mtlPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    pTexture->mDesc.mFormat = Image_Format_D32_SFLOAT_S8_UINT;
  }
#endif

  pTexture->mIsCompressed = Util::IsMtlCompressedPixelFormat(pTexture->mtlPixelFormat);
  Image_ImageHeader img;
  Image_FillHeader(
      pTexture->mDesc.mWidth,
      pTexture->mDesc.mHeight,
      pTexture->mDesc.mDepth,
      pTexture->mDesc.mArraySize,
      pTexture->mDesc.mFormat,
      &img);
  if(pTexture->mDesc.mMipLevels > 1) {
    pTexture->mTextureSize = Image_BytesRequiredForMipMapsOf(&img);
  } else {
    pTexture->mTextureSize = Image_ByteCountOf(&img);
  }

  if (pTexture->mDesc.mHostVisible) {
    InternalLog(
        LOG_TYPE_WARN,
        "Host visible textures are not supported, memory of resulting texture will not be mapped for CPU visibility",
        "addTexture");
  }

  // If we've passed a native handle, it means the texture is already on device memory, and we just need to assign it.
  if (pDesc->pNativeHandle) {
    pTexture->mOwnsImage = false;
    pTexture->mtlTexture = (id <MTLTexture>) CFBridgingRelease(pDesc->pNativeHandle);
  }
    // Otherwise, we need to create a new texture.
  else {
    pTexture->mOwnsImage = true;

    // Create a MTLTextureDescriptor that matches our requirements.
    MTLTextureDescriptor *textureDesc = [[MTLTextureDescriptor alloc] init];

    textureDesc.pixelFormat = pTexture->mtlPixelFormat;
    textureDesc.width = pTexture->mDesc.mWidth;
    textureDesc.height = pTexture->mDesc.mHeight;
    textureDesc.depth = pTexture->mDesc.mDepth;
    textureDesc.mipmapLevelCount = pTexture->mDesc.mMipLevels;
    textureDesc.sampleCount = pTexture->mDesc.mSampleCount;
    textureDesc.arrayLength = pTexture->mDesc.mArraySize;
    textureDesc.storageMode = forceNonPrivate ? MTLStorageModeShared : MTLStorageModePrivate;
    textureDesc.cpuCacheMode = MTLCPUCacheModeDefaultCache;

    if (pDesc->mDepth > 1) {
      textureDesc.textureType = MTLTextureType3D;
    } else if (pDesc->mHeight > 1) {
      if (DESCRIPTOR_TYPE_TEXTURE_CUBE == (pDesc->mDescriptors & DESCRIPTOR_TYPE_TEXTURE_CUBE)) {
        if (pTexture->mDesc.mArraySize == 6) {
          textureDesc.textureType = MTLTextureTypeCube;
          textureDesc.arrayLength = 1;
        }
#ifndef TARGET_IOS
        else {
          textureDesc.textureType = MTLTextureTypeCubeArray;
          textureDesc.arrayLength /= 6;
        }
#else
        else if ([pRenderer->pDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily4_v1])
        {
            textureDesc.textureType = MTLTextureTypeCubeArray;
            textureDesc.arrayLength /= 6;
        }
        else
        {
            internal_log(LOG_TYPE_ERROR, "Cube Array textures are not supported on this iOS device", "addTexture");
        }
#endif
      } else {
        if (pDesc->mArraySize > 1) {
          textureDesc.textureType = MTLTextureType2DArray;
        } else if (pDesc->mSampleCount > SAMPLE_COUNT_1) {
          textureDesc.textureType = MTLTextureType2DMultisample;
        } else {
          textureDesc.textureType = MTLTextureType2D;
        }
      }
    } else {
      if (pDesc->mArraySize > 1) {
        textureDesc.textureType = MTLTextureType1DArray;
      } else {
        textureDesc.textureType = MTLTextureType1D;
      }
    }

    bool isDepthBuffer = Util::IsMtlDepthPixelFormat(pTexture->mtlPixelFormat);
    bool isMultiSampled = pTexture->mDesc.mSampleCount > 1;
    if (isDepthBuffer || isMultiSampled) {
      textureDesc.resourceOptions = MTLResourceStorageModePrivate;
    }
#ifdef TARGET_IOS
    if (pDesc->mFlags & TEXTURE_CREATION_FLAG_ON_TILE)
    {
      textureDesc.resourceOptions = MTLResourceStorageModeMemoryless;
    }
#endif

    if (isRT || isDepthBuffer) {
      textureDesc.usage |= MTLTextureUsageRenderTarget;
    }
    //Create texture views only if DESCRIPTOR_RW_TEXTURE was used.
    if ((pTexture->mDesc.mDescriptors & DESCRIPTOR_TYPE_RW_TEXTURE) != 0) {
      textureDesc.usage |= MTLTextureUsagePixelFormatView;
      textureDesc.usage |= MTLTextureUsageShaderWrite;
    }

    // Allocate the texture's memory.
    AllocatorMemoryRequirements mem_reqs = {0};
    mem_reqs.usage = forceNonPrivate ? (ResourceMemoryUsage) RESOURCE_MEMORY_USAGE_UNKNOWN
                                     : (ResourceMemoryUsage) RESOURCE_MEMORY_USAGE_GPU_ONLY;
    if (pDesc->mFlags & TEXTURE_CREATION_FLAG_OWN_MEMORY_BIT) {
      mem_reqs.flags |= RESOURCE_MEMORY_REQUIREMENT_OWN_MEMORY_BIT;
    }
    if (pDesc->mFlags & TEXTURE_CREATION_FLAG_EXPORT_BIT) {
      mem_reqs.flags |= RESOURCE_MEMORY_REQUIREMENT_SHARED_BIT;
    }
    if (pDesc->mFlags & TEXTURE_CREATION_FLAG_EXPORT_ADAPTER_BIT) {
      mem_reqs.flags |= RESOURCE_MEMORY_REQUIREMENT_SHARED_ADAPTER_BIT;
    }

    TextureCreateInfo alloc_info = {textureDesc, isRT || isDepthBuffer, isMultiSampled};
    bool allocSuccess;
    allocSuccess = CreateTexture(pRenderer->pResourceAllocator, &alloc_info, &mem_reqs, pTexture);
    ASSERT(allocSuccess);
  }

  NSRange slices = NSMakeRange(0, pDesc->mArraySize);

  if (pDesc->mDescriptors & DESCRIPTOR_TYPE_RW_TEXTURE) {
    MTLTextureType uavType = pTexture->mtlTexture.textureType;
    if (pTexture->mtlTexture.textureType == MTLTextureTypeCube
        || pTexture->mtlTexture.textureType == MTLTextureTypeCubeArray) {
      uavType = MTLTextureType2DArray;
    }
    pTexture->pMtlUAVDescriptors = (id <MTLTexture> __strong *) calloc(pDesc->mMipLevels, sizeof(id <MTLTexture>));
    for (uint32_t i = 0; i < pDesc->mMipLevels; ++i) {
      NSRange levels = NSMakeRange(i, 1);
      pTexture->pMtlUAVDescriptors[i] =
          [pTexture->mtlTexture newTextureViewWithPixelFormat:pTexture->mtlTexture.pixelFormat
                                                  textureType:uavType
                                                       levels:levels
                                                       slices:slices];
    }
  }

  *ppTexture = pTexture;
}

}} // end namespace TheForge::Metal

/*
 * Copyright (c) 2018-2019 Confetti Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/
