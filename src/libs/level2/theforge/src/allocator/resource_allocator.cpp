#include "core/core.h"
#include "core/logger.h"
#include "tinystl/vector.h"
#include "allocator.hpp"
#include "dynamic_allocator.hpp"
#include "resource_allocator.hpp"

namespace TheForge {
/*
 Represents a region of AllocatorBlock that is either assigned and returned as
 allocated memory block or free.
 */
struct AllocatorSuballocation {
  //id<MTLBuffer> bufferResource;
  //id<MTLTexture> textureResource;
  void *mappedData;
  uint64_t offset;
  uint64_t size;
  AllocatorSuballocationType type;
};
typedef AllocatorList<AllocatorSuballocation> AllocatorSuballocationList;

// Parameters of an allocation.
struct AllocatorAllocationRequest {
  AllocatorSuballocationList::iterator freeSuballocationItem;
  uint64_t offset;
};


// -------------------------------------------------------------------------------------------------
// AllocatorBlock class declaration.
// -------------------------------------------------------------------------------------------------

/* Single block of memory with all the data about its regions assigned or free. */
class AllocatorBlock {
 public:
  uint32_t m_MemoryTypeIndex;
  ResourceBlockVectorType m_BlockVectorType;
//  id<MTLHeap>                m_hMemory;
//  id<MTLBuffer>              m_Buffer;
//  id<MTLTexture>             m_Texture;
  uint64_t m_Size;
  bool m_PersistentMap;
  void *m_pMappedData;
  uint32_t m_FreeCount;
  uint64_t m_SumFreeSize;
  AllocatorSuballocationList m_Suballocations;
  // Suballocations that are free and have size greater than certain threshold.
  // Sorted by size, ascending.
  tinystl::vector<AllocatorSuballocationList::iterator> m_FreeSuballocationsBySize;

  AllocatorBlock(ResourceAllocator *hAllocator);

  ~AllocatorBlock() { ASSERT(m_hMemory == NULL); }

  // Always call after construction.
/*  void Init(
      uint32_t newMemoryTypeIndex, ResourceBlockVectorType newBlockVectorType, id <MTLHeap> newMemory, uint64_t newSize,
      bool persistentMap, void *pMappedData);
  void Init(
      uint32_t newMemoryTypeIndex,
      ResourceBlockVectorType newBlockVectorType,
      id <MTLBuffer> newMemory,
      uint64_t newSize,
      bool persistentMap,
      void *pMappedData);
  void Init(
      uint32_t newMemoryTypeIndex,
      ResourceBlockVectorType newBlockVectorType,
      id <MTLTexture> newMemory,
      uint64_t newSize,
      bool persistentMap,
      void *pMappedData);*/
  // Always call before destruction.
  void Destroy(ResourceAllocator *allocator);

  // Validates all data structures inside this object. If not valid, returns false.
  bool Validate() const;

  // Tries to find a place for suballocation with given parameters inside this allocation.
  // If succeeded, fills pAllocationRequest and returns true.
  // If failed, returns false.
  bool CreateAllocationRequest(
      uint64_t bufferImageGranularity,
      uint64_t allocSize,
      uint64_t allocAlignment,
      AllocatorSuballocationType allocType,
      AllocatorAllocationRequest *pAllocationRequest);

  // Checks if requested suballocation with given parameters can be placed in given pFreeSuballocItem.
  // If yes, fills pOffset and returns true. If no, returns false.
  bool CheckAllocation(
      uint64_t bufferImageGranularity,
      uint64_t allocSize,
      uint64_t allocAlignment,
      AllocatorSuballocationType allocType,
      AllocatorSuballocationList::const_iterator freeSuballocItem,
      uint64_t *pOffset) const;

  // Returns true if this allocation is empty - contains only single free suballocation.
  bool IsEmpty() const;

  // Makes actual allocation based on request. Request must already be checked
  // and valid.
  void Alloc(const AllocatorAllocationRequest& request, AllocatorSuballocationType type, uint64_t allocSize);

  // Frees suballocation assigned to given memory region.
  void Free(const ResourceAllocation *allocation);

#if RESOURCE_STATS_STRING_ENABLED
  void PrintDetailedMap(class AllocatorStringBuilder& sb) const;
#endif

 private:
  // Given free suballocation, it merges it with following one, which must also be free.
  void MergeFreeWithNext(AllocatorSuballocationList::iterator item);
  // Releases given suballocation, making it free. Merges it with adjacent free
  // suballocations if applicable.
  void FreeSuballocation(AllocatorSuballocationList::iterator suballocItem);
  // Given free suballocation, it inserts it into sorted list of
  // m_FreeSuballocationsBySize if it's suitable.
  void RegisterFreeSuballocation(AllocatorSuballocationList::iterator item);
  // Given free suballocation, it removes it from sorted list of
  // m_FreeSuballocationsBySize if it's suitable.
  void UnregisterFreeSuballocation(AllocatorSuballocationList::iterator item);
};

// -------------------------------------------------------------------------------------------------
// AllocatorBlockVector struct declaration.
// -------------------------------------------------------------------------------------------------

/* Sequence of AllocatorBlock. Represents memory blocks allocated for a specific
 Metal memory type. */
struct AllocatorBlockVector {
  // Incrementally sorted by sumFreeSize, ascending.
  AllocatorVector<AllocatorBlock *> m_Blocks;

  AllocatorBlockVector(ResourceAllocator *hAllocator);
  ~AllocatorBlockVector();

  bool IsEmpty() const { return m_Blocks.empty(); }

  // Finds and removes given block from vector.
  void Remove(AllocatorBlock *pBlock);

  // Performs single step in sorting m_Blocks. They may not be fully sorted
  // after this call.
  void IncrementallySortBlocks();

  // Adds statistics of this BlockVector to pStats.
  void AddStats(AllocatorStats *pStats, uint32_t memTypeIndex, uint32_t memHeapIndex) const;

#if RESOURCE_STATS_STRING_ENABLED
  void PrintDetailedMap(class AllocatorStringBuilder& sb) const;
#endif

  void UnmapPersistentlyMappedMemory();
  bool MapPersistentlyMappedMemory();

 private:
  ResourceAllocator *m_hAllocator;
};

// -------------------------------------------------------------------------------------------------
// ResourceAllocation functionality implementation.
// -------------------------------------------------------------------------------------------------

id <MTLHeap> ResourceAllocation::GetMemory() const {
  return (m_Type == ALLOCATION_TYPE_BLOCK) ? m_BlockAllocation.m_Block->m_hMemory : nil;
}

id <MTLBuffer> ResourceAllocation::GetResource() const {
  if (m_Type == ALLOCATION_TYPE_OWN) {
    return NULL;
  }
  return (m_SuballocationType == RESOURCE_SUBALLOCATION_TYPE_BUFFER) ? m_BlockAllocation.m_Block->m_Buffer : nil;
}

uint32_t ResourceAllocation::GetMemoryTypeIndex() const {
  return (m_Type == ALLOCATION_TYPE_BLOCK) ? m_BlockAllocation.m_Block->m_MemoryTypeIndex
                                           : m_OwnAllocation.m_MemoryTypeIndex;
}

RESOURCE_BLOCK_VECTOR_TYPE ResourceAllocation::GetBlockVectorType() const {
  return (m_Type == ALLOCATION_TYPE_BLOCK)
         ? m_BlockAllocation.m_Block->m_BlockVectorType
         : (m_OwnAllocation.m_PersistentMap ? RESOURCE_BLOCK_VECTOR_TYPE_MAPPED : RESOURCE_BLOCK_VECTOR_TYPE_UNMAPPED);
}

void *ResourceAllocation::GetMappedData() const {
  switch (m_Type) {
    case ALLOCATION_TYPE_BLOCK:
      if (m_BlockAllocation.m_Block->m_pMappedData != RESOURCE_NULL) {
        return (char *) m_BlockAllocation.m_Block->m_pMappedData + m_BlockAllocation.m_Offset;
      } else {
        return RESOURCE_NULL;
      }
      break;
    case ALLOCATION_TYPE_OWN: return m_OwnAllocation.m_pMappedData;
    default: ASSERT(0);
      return RESOURCE_NULL;
  }
}

// -------------------------------------------------------------------------------------------------
// ResourceAllocator functionality implementation.
// -------------------------------------------------------------------------------------------------

struct AllocatorPointerLess {
  bool operator()(const void *lhs, const void *rhs) const { return lhs < rhs; }
};

bool resourceAllocFindMemoryTypeIndex(
    ResourceAllocator *allocator,
    const AllocationInfo *pAllocInfo,
    const AllocatorMemoryRequirements *pMemoryRequirements,
    const AllocatorSuballocationType pSuballocType,
    uint32_t *pMemoryTypeIndex) {
  ASSERT(allocator != RESOURCE_NULL);
  ASSERT(pMemoryRequirements != RESOURCE_NULL);
  ASSERT(pMemoryTypeIndex != RESOURCE_NULL);

  *pMemoryTypeIndex = UINT32_MAX;

  switch (pSuballocType) {
    case RESOURCE_SUBALLOCATION_TYPE_BUFFER:
      if (pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_GPU_ONLY) {
        *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_DEFAULT_BUFFER;
      } else if (
          pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_CPU_ONLY ||
              pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_CPU_TO_GPU) {
                *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_UPLOAD_BUFFER;
      } else if (pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_GPU_TO_CPU) {
                *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_READBACK_BUFFER;
      }
      break;
    case RESOURCE_SUBALLOCATION_TYPE_IMAGE_OPTIMAL:
      if (pAllocInfo->mSizeAlign.size <= 4096) {
        *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_TEXTURE_SMALL;
      } else if (pAllocInfo->mIsMS) {
        *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_TEXTURE_MS;
      } else {
        *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_TEXTURE_DEFAULT;
      }
      break;
    case RESOURCE_SUBALLOCATION_TYPE_IMAGE_RTV_DSV:
      if (pAllocInfo->mIsMS) {
        if (pMemoryRequirements->flags & RESOURCE_MEMORY_REQUIREMENT_SHARED_ADAPTER_BIT) {
          *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_TEXTURE_RTV_DSV_SHARED_ADAPTER_MS;
        } else if (pMemoryRequirements->flags & RESOURCE_MEMORY_REQUIREMENT_SHARED_BIT) {
          *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_TEXTURE_RTV_DSV_SHARED_MS;
        } else {
          *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_TEXTURE_RTV_DSV_MS;
        }
      } else {
        if (pMemoryRequirements->flags & RESOURCE_MEMORY_REQUIREMENT_SHARED_ADAPTER_BIT) {
          *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_TEXTURE_RTV_DSV_SHARED_ADAPTER;
        } else if (pMemoryRequirements->flags & RESOURCE_MEMORY_REQUIREMENT_SHARED_BIT) {
          *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_TEXTURE_RTV_DSV_SHARED;
        } else {
          *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_TEXTURE_RTV_DSV;
        }
      }
      break;
    case RESOURCE_SUBALLOCATION_TYPE_BUFFER_SRV_UAV:
      if (pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_GPU_ONLY) {
        *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_DEFAULT_UAV;
      } else if (
          pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_CPU_ONLY ||
              pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_CPU_TO_GPU) {
                *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_UPLOAD_UAV;
      } else if (pMemoryRequirements->usage == RESOURCE_MEMORY_USAGE_GPU_TO_CPU) {
                *pMemoryTypeIndex = RESOURCE_MEMORY_TYPE_READBACK_UAV;
      }
      break;
    default: break;
  }

  return (*pMemoryTypeIndex != UINT32_MAX) ? true : false;
}

ResourceAllocator::ResourceAllocator(const AllocatorCreateInfo *pCreateInfo) :
    m_UseMutex((pCreateInfo->flags & RESOURCE_ALLOCATOR_EXTERNALLY_SYNCHRONIZED_BIT) == 0),
    m_Device(pCreateInfo->device),
    m_PreferredLargeHeapBlockSize(0),
    m_PreferredSmallHeapBlockSize(0),
    m_UnmapPersistentlyMappedMemoryCounter(0) {
  ASSERT(pCreateInfo->device);

  memset(&m_pBlockVectors, 0, sizeof(m_pBlockVectors));
  memset(&m_HasEmptyBlock, 0, sizeof(m_HasEmptyBlock));
  memset(&m_pOwnAllocations, 0, sizeof(m_pOwnAllocations));

  m_PreferredLargeHeapBlockSize = (pCreateInfo->preferredLargeHeapBlockSize != 0)
                                  ? pCreateInfo->preferredLargeHeapBlockSize
                                  : static_cast<uint64_t>(RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE);
  m_PreferredSmallHeapBlockSize = (pCreateInfo->preferredSmallHeapBlockSize != 0)
                                  ? pCreateInfo->preferredSmallHeapBlockSize
                                  : static_cast<uint64_t>(RESOURCE_DEFAULT_SMALL_HEAP_BLOCK_SIZE);

  for (size_t i = 0; i < GetMemoryTypeCount(); ++i) {
    for (size_t j = 0; j < RESOURCE_BLOCK_VECTOR_TYPE_COUNT; ++j) {
      m_pBlockVectors[i][j] = conf_placement_new<AllocatorBlockVector>(AllocatorAllocate<AllocatorBlockVector>(), this);
      m_pOwnAllocations[i][j] = conf_placement_new<AllocationVectorType>(AllocatorAllocate<AllocationVectorType>());
    }
  }
}

ResourceAllocator::~ResourceAllocator() {
  for (size_t i = GetMemoryTypeCount(); i--;) {
    for (size_t j = RESOURCE_BLOCK_VECTOR_TYPE_COUNT; j--;) {
      resourceAlloc_delete(m_pOwnAllocations[i][j]);
      resourceAlloc_delete(m_pBlockVectors[i][j]);
    }
  }
}

static ResourceBlockVectorType AllocatorMemoryRequirementFlagsToBlockVectorType(AllocatorMemoryRequirementFlags flags) {
  return (flags & RESOURCE_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT) != 0 ? ResourceBlockVectorType::MAPPED
                                                                       : ResourceBlockVectorType::UNMAPPED;
}

bool ResourceAllocator::AllocateMemoryOfType(
    const MTLSizeAndAlign& mtlMemReq, const AllocatorMemoryRequirements& resourceAllocMemReq, uint32_t memTypeIndex,
    AllocatorSuballocationType suballocType, ResourceAllocation **pAllocation) {
  ASSERT(pAllocation != RESOURCE_NULL);
  RESOURCE_DEBUG_LOG("  AllocateMemory: MemoryTypeIndex=%u, Size=%llu", memTypeIndex, mtlMemReq.size);

  const uint64_t preferredBlockSize = GetPreferredBlockSize(resourceAllocMemReq.usage, memTypeIndex);

  // Only private storage heaps are supported on macOS, so any host visible memory must be allocated
  // as committed resource (own allocation).
  const bool hostVisible = resourceAllocMemReq.usage != RESOURCE_MEMORY_USAGE_GPU_ONLY;

  // Heuristics: Allocate own memory if requested size if greater than half of preferred block size.
  const bool ownMemory =
      hostVisible || (resourceAllocMemReq.flags & RESOURCE_MEMORY_REQUIREMENT_OWN_MEMORY_BIT) != 0
          || RESOURCE_DEBUG_ALWAYS_OWN_MEMORY ||
          ((resourceAllocMemReq.flags & RESOURCE_MEMORY_REQUIREMENT_NEVER_ALLOCATE_BIT) == 0
              && mtlMemReq.size > preferredBlockSize / 2);

  if (ownMemory) {
    if ((resourceAllocMemReq.flags & RESOURCE_MEMORY_REQUIREMENT_NEVER_ALLOCATE_BIT) != 0) {
      return false;
    } else {
      return AllocateOwnMemory(
          mtlMemReq.size,
          suballocType,
          memTypeIndex,
          (resourceAllocMemReq.flags & RESOURCE_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT) != 0,
          resourceAllocMemReq.pUserData,
          pAllocation);
    }
  } else {
    uint32_t blockVectorType = AllocatorMemoryRequirementFlagsToBlockVectorType(resourceAllocMemReq.flags);

    AllocatorMutexLock lock(m_BlocksMutex[memTypeIndex], m_UseMutex);
    AllocatorBlockVector *const blockVector = m_pBlockVectors[memTypeIndex][blockVectorType];
    ASSERT(blockVector);

    // 1. Search existing allocations.
    // Forward order - prefer blocks with smallest amount of free space.
    for (size_t allocIndex = 0; allocIndex < blockVector->m_Blocks.size(); ++allocIndex) {
      AllocatorBlock *const pBlock = blockVector->m_Blocks[allocIndex];
      ASSERT(pBlock);
      AllocatorAllocationRequest allocRequest = {};
      // Check if can allocate from pBlock.
      if (pBlock->CreateAllocationRequest(GetBufferImageGranularity(),
                                          mtlMemReq.size,
                                          mtlMemReq.align,
                                          suballocType,
                                          &allocRequest)) {
        // We no longer have an empty Allocation.
        if (pBlock->IsEmpty()) {
          m_HasEmptyBlock[memTypeIndex] = false;
        }
        // Allocate from this pBlock.
        pBlock->Alloc(allocRequest, suballocType, mtlMemReq.size);
        *pAllocation = conf_placement_new<ResourceAllocation>(AllocatorAllocate<ResourceAllocation>());
        (*pAllocation)
            ->InitBlockAllocation(
                pBlock,
                allocRequest.offset,
                mtlMemReq.align,
                mtlMemReq.size,
                suballocType,
                resourceAllocMemReq.pUserData);
        RESOURCE_HEAVY_ASSERT(pBlock->Validate());
        RESOURCE_DEBUG_LOG("    Returned from existing allocation #%u", (uint32_t) allocIndex);
        return true;
      }
    }

    // 2. Create new Allocation.
    if ((resourceAllocMemReq.flags & RESOURCE_MEMORY_REQUIREMENT_NEVER_ALLOCATE_BIT) != 0) {
      RESOURCE_DEBUG_LOG("    FAILED due to RESOURCE_MEMORY_REQUIREMENT_NEVER_ALLOCATE_BIT");
      return false;
    } else {
      bool res = false;

      const AllocatorHeapProperties *pHeapProps = &gHeapProperties[memTypeIndex];
      MTLHeapDescriptor * allocInfo = [[MTLHeapDescriptor
      alloc] init];
      allocInfo.size = preferredBlockSize;
      allocInfo.cpuCacheMode = pHeapProps->mCPUCacheMode;
      allocInfo.storageMode = pHeapProps->mStorageMode;

      // Start with full preferredBlockSize.
      id <MTLHeap> mem = nil;
      mem = [m_Device
      newHeapWithDescriptor:
      allocInfo];
      mem.label = [NSString
      stringWithFormat:@"%s", pHeapProps->pName];
      if (mem == nil) {
        // 3. Try half the size.
        allocInfo.size /= 2;
        if (allocInfo.size >= mtlMemReq.size) {
          mem = [m_Device
          newHeapWithDescriptor:
          allocInfo];
          mem.label = [NSString
          stringWithFormat:@"%s", pHeapProps->pName];
          if (mem == nil) {
            // 4. Try quarter the size.
            allocInfo.size /= 2;
            if (allocInfo.size >= mtlMemReq.size) {
              mem = [m_Device
              newHeapWithDescriptor:
              allocInfo];
              mem.label = [NSString
              stringWithFormat:@"%s", pHeapProps->pName];
            }
          }
        }
      }
      if (mem == nil) {
        // 5. Try OwnAlloc.
        res = AllocateOwnMemory(
            mtlMemReq.size,
            suballocType,
            memTypeIndex,
            (resourceAllocMemReq.flags & RESOURCE_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT) != 0,
            resourceAllocMemReq.pUserData,
            pAllocation);
        if (res) {
          // Succeeded: AllocateOwnMemory function already filld pMemory, nothing more to do here.
          RESOURCE_DEBUG_LOG("    Allocated as OwnMemory");
        } else {
          // Everything failed: Return error code.
          RESOURCE_DEBUG_LOG("    Metal texture allocation FAILED");
        }

        return res;
      }

      // Create new Allocation for it.
      AllocatorBlock *const pBlock = conf_placement_new<AllocatorBlock>(AllocatorAllocate<AllocatorBlock>(), this);
      pBlock->Init(memTypeIndex, (RESOURCE_BLOCK_VECTOR_TYPE) blockVectorType, mem, allocInfo.size, false, NULL);
      blockVector->m_Blocks.push_back(pBlock);

      // Allocate from pBlock. Because it is empty, dstAllocRequest can be trivially filled.
      AllocatorAllocationRequest allocRequest = {};
      allocRequest.freeSuballocationItem = pBlock->m_Suballocations.begin();
      allocRequest.offset = 0;
      pBlock->Alloc(allocRequest, suballocType, mtlMemReq.size);
      *pAllocation = conf_placement_new<ResourceAllocation>(AllocatorAllocate<ResourceAllocation>());
      (*pAllocation)
          ->InitBlockAllocation(
              pBlock,
              allocRequest.offset,
              mtlMemReq.align,
              mtlMemReq.size,
              suballocType,
              resourceAllocMemReq.pUserData);
      RESOURCE_HEAVY_ASSERT(pBlock->Validate());

      RESOURCE_DEBUG_LOG("    Created new allocation Size=%llu", allocInfo.SizeInBytes);

      return true;
    }
  }
  return false;
}

bool ResourceAllocator::AllocateOwnMemory(
    uint64_t size, AllocatorSuballocationType suballocType, uint32_t memTypeIndex, bool map, void *pUserData,
    ResourceAllocation **pAllocation) {
  ASSERT(pAllocation);

  *pAllocation = conf_placement_new<ResourceAllocation>(AllocatorAllocate<ResourceAllocation>());
  (*pAllocation)->InitOwnAllocation(memTypeIndex, suballocType, map, NULL, size, pUserData);

  // Register it in m_pOwnAllocations.
  {
    AllocatorMutexLock lock(m_OwnAllocationsMutex[memTypeIndex], m_UseMutex);
    AllocationVectorType *pOwnAllocations =
        m_pOwnAllocations[memTypeIndex][map ? RESOURCE_BLOCK_VECTOR_TYPE_MAPPED : RESOURCE_BLOCK_VECTOR_TYPE_UNMAPPED];
    ASSERT(pOwnAllocations);
    ResourceAllocation **const pOwnAllocationsBeg = pOwnAllocations->data();
    ResourceAllocation **const pOwnAllocationsEnd = pOwnAllocationsBeg + pOwnAllocations->size();
    const size_t indexToInsert =
        AllocatorBinaryFindFirstNotLess(pOwnAllocationsBeg, pOwnAllocationsEnd, *pAllocation, AllocatorPointerLess()) -
            pOwnAllocationsBeg;
    VectorInsert(*pOwnAllocations, indexToInsert, *pAllocation);
  }

  RESOURCE_DEBUG_LOG("    Allocated OwnMemory MemoryTypeIndex=#%u", memTypeIndex);

  return true;
}

uint64_t ResourceAllocator::GetPreferredBlockSize(ResourceMemoryUsage memUsage, uint32_t memTypeIndex) const {
  return gHeapProperties[memTypeIndex].mBlockSize;
}

bool ResourceAllocator::AllocateMemory(
    const AllocationInfo& info,
    const AllocatorMemoryRequirements& resourceAllocMemReq,
    AllocatorSuballocationType suballocType,
    ResourceAllocation **pAllocation) {
  if ((resourceAllocMemReq.flags & RESOURCE_MEMORY_REQUIREMENT_OWN_MEMORY_BIT) != 0 &&
      (resourceAllocMemReq.flags & RESOURCE_MEMORY_REQUIREMENT_NEVER_ALLOCATE_BIT) != 0) {
    ASSERT(
        0 &&
            "Specifying RESOURCE_MEMORY_REQUIREMENT_OWN_MEMORY_BIT together with RESOURCE_MEMORY_REQUIREMENT_NEVER_ALLOCATE_BIT makes no "
            "sense.");
    return false;
  }

  // Bit mask of memory Metal types acceptable for this allocation.
  uint32_t memTypeIndex = UINT32_MAX;
  bool res = resourceAllocFindMemoryTypeIndex(this, &info, &resourceAllocMemReq, suballocType, &memTypeIndex);
  if (res) {
    res = AllocateMemoryOfType(info.mSizeAlign, resourceAllocMemReq, memTypeIndex, suballocType, pAllocation);
    // Succeeded on first try.
    if (res) {
      return res;
    }
      // Allocation from this memory type failed. Try other compatible memory types.
    else {
      return false;
    }
  }
    // Can't find any single memory type maching requirements. res is VK_ERROR_FEATURE_NOT_PRESENT.
  else {
    return res;
  }
}

void ResourceAllocator::FreeMemory(ResourceAllocation *allocation) {
  ASSERT(allocation);

  if (allocation->GetType() == ResourceAllocation::ALLOCATION_TYPE_BLOCK) {
    AllocatorBlock *pBlockToDelete = RESOURCE_NULL;

    const uint32_t memTypeIndex = allocation->GetMemoryTypeIndex();
    const RESOURCE_BLOCK_VECTOR_TYPE blockVectorType = allocation->GetBlockVectorType();
    {
      AllocatorMutexLock lock(m_BlocksMutex[memTypeIndex], m_UseMutex);

      AllocatorBlockVector *pBlockVector = m_pBlockVectors[memTypeIndex][blockVectorType];
      AllocatorBlock *pBlock = allocation->GetBlock();

      pBlock->Free(allocation);
      RESOURCE_HEAVY_ASSERT(pBlock->Validate());

      RESOURCE_DEBUG_LOG("  Freed from MemoryTypeIndex=%u", memTypeIndex);

      // pBlock became empty after this deallocation.
      if (pBlock->IsEmpty()) {
        // Already has empty Allocation. We don't want to have two, so delete this one.
        if (m_HasEmptyBlock[memTypeIndex]) {
          pBlockToDelete = pBlock;
          pBlockVector->Remove(pBlock);
        }
          // We now have first empty Allocation.
        else {
          m_HasEmptyBlock[memTypeIndex] = true;
        }
      }
      // Must be called after srcBlockIndex is used, because later it may become invalid!
      pBlockVector->IncrementallySortBlocks();
    }
    // Destruction of a free Allocation. Deferred until this point, outside of mutex
    // lock, for performance reason.
    if (pBlockToDelete != RESOURCE_NULL) {
      RESOURCE_DEBUG_LOG("    Deleted empty allocation");
      pBlockToDelete->Destroy(this);
      resourceAlloc_delete(pBlockToDelete);
    }

    resourceAlloc_delete(allocation);
  } else    // AllocatorAllocation_T::ALLOCATION_TYPE_OWN
  {
    FreeOwnMemory(allocation);
  }
}

void ResourceAllocator::CalculateStats(AllocatorStats *pStats) {
  InitStatInfo(pStats->total);
  for (size_t i = 0; i < RESOURCE_MEMORY_TYPE_NUM_TYPES; ++i) {
    InitStatInfo(pStats->memoryType[i]);
  }
  for (size_t i = 0; i < RESOURCE_MEMORY_TYPE_NUM_TYPES; ++i) {
    InitStatInfo(pStats->memoryHeap[i]);
  }

  for (uint32_t memTypeIndex = 0; memTypeIndex < GetMemoryTypeCount(); ++memTypeIndex) {
    AllocatorMutexLock allocationsLock(m_BlocksMutex[memTypeIndex], m_UseMutex);
    const uint32_t heapIndex = memTypeIndex;
    for (uint32_t blockVectorType = 0; blockVectorType < RESOURCE_BLOCK_VECTOR_TYPE_COUNT; ++blockVectorType) {
      const AllocatorBlockVector *const pBlockVector = m_pBlockVectors[memTypeIndex][blockVectorType];
      ASSERT(pBlockVector);
      pBlockVector->AddStats(pStats, memTypeIndex, heapIndex);
    }
  }

  AllocatorPostprocessCalcStatInfo(pStats->total);
  for (size_t i = 0; i < GetMemoryTypeCount(); ++i) {
    AllocatorPostprocessCalcStatInfo(pStats->memoryType[i]);
  }
  for (size_t i = 0; i < GetMemoryHeapCount(); ++i) {
    AllocatorPostprocessCalcStatInfo(pStats->memoryHeap[i]);
  }
}

static const uint32_t RESOURCE_VENDOR_ID_AMD = 4098;

void ResourceAllocator::UnmapPersistentlyMappedMemory() {
  if (m_UnmapPersistentlyMappedMemoryCounter++ == 0) {
    /*if (m_PhysicalDeviceProperties.VendorId == RESOURCE_VENDOR_ID_AMD)
    {
        size_t memTypeIndex = D3D12_HEAP_TYPE_UPLOAD;
        {
            {
                // Process OwnAllocations.
                {
                    AllocatorMutexLock lock(m_OwnAllocationsMutex[memTypeIndex], m_UseMutex);
                    AllocationVectorType* pOwnAllocationsVector = m_pOwnAllocations[memTypeIndex][RESOURCE_BLOCK_VECTOR_TYPE_MAPPED];
                    for (size_t ownAllocIndex = pOwnAllocationsVector->size(); ownAllocIndex--; )
                    {
                        ResourceAllocation* hAlloc = (*pOwnAllocationsVector)[ownAllocIndex];
                        hAlloc->OwnAllocUnmapPersistentlyMappedMemory();
                    }
                }
                // Process normal Allocations.
                {
                    AllocatorMutexLock lock(m_BlocksMutex[memTypeIndex], m_UseMutex);
                    AllocatorBlockVector* pBlockVector = m_pBlockVectors[memTypeIndex][RESOURCE_BLOCK_VECTOR_TYPE_MAPPED];
                    pBlockVector->UnmapPersistentlyMappedMemory();
                }
            }
        }
    }*/
  }
}

bool ResourceAllocator::MapPersistentlyMappedMemory() {
  ASSERT(m_UnmapPersistentlyMappedMemoryCounter > 0);
  if (--m_UnmapPersistentlyMappedMemoryCounter == 0) {
    bool finalResult = true;
    /*if (m_PhysicalDeviceProperties.VendorId == RESOURCE_VENDOR_ID_AMD)
    {
        size_t memTypeIndex = D3D12_HEAP_TYPE_UPLOAD;
        {
            {
                // Process OwnAllocations.
                {
                    AllocatorMutexLock lock(m_OwnAllocationsMutex[memTypeIndex], m_UseMutex);
                    AllocationVectorType* pAllocationsVector = m_pOwnAllocations[memTypeIndex][RESOURCE_BLOCK_VECTOR_TYPE_MAPPED];
                    for (size_t ownAllocIndex = 0, ownAllocCount = pAllocationsVector->size(); ownAllocIndex < ownAllocCount; ++ownAllocIndex)
                    {
                        ResourceAllocation* hAlloc = (*pAllocationsVector)[ownAllocIndex];
                        hAlloc->OwnAllocMapPersistentlyMappedMemory();
                    }
                }
                // Process normal Allocations.
                {
                    AllocatorMutexLock lock(m_BlocksMutex[memTypeIndex], m_UseMutex);
                    AllocatorBlockVector* pBlockVector = m_pBlockVectors[memTypeIndex][RESOURCE_BLOCK_VECTOR_TYPE_MAPPED];
                    HRESULT localResult = pBlockVector->MapPersistentlyMappedMemory();
                    if (!SUCCEEDED(localResult))
                    {
                        finalResult = localResult;
                    }
                }
            }
        }
    }*/
    return finalResult;
  } else {
    return true;
  }
}

void ResourceAllocator::GetAllocationInfo(ResourceAllocation *hAllocation, ResourceAllocationInfo *pAllocationInfo) {
  pAllocationInfo->memoryType = hAllocation->GetMemoryTypeIndex();
  pAllocationInfo->deviceMemory = hAllocation->GetMemory();
  pAllocationInfo->resource = hAllocation->GetResource();
  pAllocationInfo->offset = hAllocation->GetOffset();
  pAllocationInfo->size = hAllocation->GetSize();
  pAllocationInfo->pMappedData = hAllocation->GetMappedData();
  pAllocationInfo->pUserData = hAllocation->GetUserData();
}

void ResourceAllocator::FreeOwnMemory(ResourceAllocation *allocation) {
  ASSERT(allocation && allocation->GetType() == ResourceAllocation::ALLOCATION_TYPE_OWN);

  const uint32_t memTypeIndex = allocation->GetMemoryTypeIndex();
  {
    AllocatorMutexLock lock(m_OwnAllocationsMutex[memTypeIndex], m_UseMutex);
    AllocationVectorType *const pOwnAllocations = m_pOwnAllocations[memTypeIndex][allocation->GetBlockVectorType()];
    ASSERT(pOwnAllocations);
    ResourceAllocation **const pOwnAllocationsBeg = pOwnAllocations->data();
    ResourceAllocation **const pOwnAllocationsEnd = pOwnAllocationsBeg + pOwnAllocations->size();
    ResourceAllocation **const pOwnAllocationIt =
        AllocatorBinaryFindFirstNotLess(pOwnAllocationsBeg, pOwnAllocationsEnd, allocation, AllocatorPointerLess());
    if (pOwnAllocationIt != pOwnAllocationsEnd) {
      const size_t ownAllocationIndex = pOwnAllocationIt - pOwnAllocationsBeg;
      VectorRemove(*pOwnAllocations, ownAllocationIndex);
    } else {
      ASSERT(0);
    }
  }

  RESOURCE_DEBUG_LOG("    Freed OwnMemory MemoryTypeIndex=%u", memTypeIndex);

  resourceAlloc_delete(allocation);
}

#if RESOURCE_STATS_STRING_ENABLED

void ResourceAllocator::PrintDetailedMap(AllocatorStringBuilder& sb)
{
  bool ownAllocationsStarted = false;
  for (size_t memTypeIndex = 0; memTypeIndex < GetMemoryTypeCount(); ++memTypeIndex)
  {
    AllocatorMutexLock ownAllocationsLock(m_OwnAllocationsMutex[memTypeIndex], m_UseMutex);
    for (uint32_t blockVectorType = 0; blockVectorType < RESOURCE_BLOCK_VECTOR_TYPE_COUNT; ++blockVectorType)
    {
      AllocationVectorType* const pOwnAllocVector = m_pOwnAllocations[memTypeIndex][blockVectorType];
      ASSERT(pOwnAllocVector);
      if (pOwnAllocVector->empty() == false)
      {
        if (ownAllocationsStarted)
        {
          sb.Add(",\n\t\"Type ");
        }
        else
        {
          sb.Add(",\n\"OwnAllocations\": {\n\t\"Type ");
          ownAllocationsStarted = true;
        }
        sb.AddNumber((uint64_t)memTypeIndex);
        if (blockVectorType == RESOURCE_BLOCK_VECTOR_TYPE_MAPPED)
        {
          sb.Add(" Mapped");
        }
        sb.Add("\": [");

        for (size_t i = 0; i < pOwnAllocVector->size(); ++i)
        {
          const ResourceAllocation* hAlloc = (*pOwnAllocVector)[i];
          if (i > 0)
          {
            sb.Add(",\n\t\t{ \"Size\": ");
          }
          else
          {
            sb.Add("\n\t\t{ \"Size\": ");
          }
          sb.AddNumber(hAlloc->GetSize());
          sb.Add(", \"Type\": ");
          sb.AddString(RESOURCE_SUBALLOCATION_TYPE_NAMES[hAlloc->GetSuballocationType()]);
          sb.Add(" }");
        }

        sb.Add("\n\t]");
      }
    }
  }
  if (ownAllocationsStarted)
  {
    sb.Add("\n}");
  }

  {
    bool allocationsStarted = false;
    for (size_t memTypeIndex = 0; memTypeIndex < GetMemoryTypeCount(); ++memTypeIndex)
    {
      AllocatorMutexLock globalAllocationsLock(m_BlocksMutex[memTypeIndex], m_UseMutex);
      for (uint32_t blockVectorType = 0; blockVectorType < RESOURCE_BLOCK_VECTOR_TYPE_COUNT; ++blockVectorType)
      {
        if (m_pBlockVectors[memTypeIndex][blockVectorType]->IsEmpty() == false)
        {
          if (allocationsStarted)
          {
            sb.Add(",\n\t\"Type ");
          }
          else
          {
            sb.Add(",\n\"Allocations\": {\n\t\"Type ");
            allocationsStarted = true;
          }
          sb.AddNumber((uint64_t)memTypeIndex);
          if (blockVectorType == RESOURCE_BLOCK_VECTOR_TYPE_MAPPED)
          {
            sb.Add(" Mapped");
          }
          sb.Add("\": [");

          m_pBlockVectors[memTypeIndex][blockVectorType]->PrintDetailedMap(sb);

          sb.Add("\n\t]");
        }
      }
    }
    if (allocationsStarted)
    {
      sb.Add("\n}");
    }
  }
}
#endif    // #if RESOURCE_STATS_STRING_ENABLED

}