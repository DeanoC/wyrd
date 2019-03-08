#pragma once
#ifndef WYRD_THEFORGE_ALLOCATOR_RESOURCE_ALLOCATOR_HPP
#define WYRD_THEFORGE_ALLOCATOR_RESOURCE_ALLOCATOR_HPP

#include "core/core.h"
#include "core/logger.h"
#include "allocator_list.hpp"

namespace TheForge {

enum class ResourceBlockVectorType {
  UNMAPPED,
  MAPPED,
  COUNT
};

struct OwnAllocation {
//  id<MTLBuffer>  m_Buffer;
//  id<MTLTexture> m_Texture;
  uint32_t m_MemoryTypeIndex;
  bool m_PersistentMap;
  void *m_pMappedData;
};

struct BlockAllocation {
  struct AllocatorBlock *m_Block;
  uint64_t m_Offset;
};

struct ResourceAllocation {
 public:
  enum ALLOCATION_TYPE {
    ALLOCATION_TYPE_NONE,
    ALLOCATION_TYPE_BLOCK,
    ALLOCATION_TYPE_OWN,
  };

  void InitBlockAllocation(
      struct AllocatorBlock *block,
      uint64_t offset,
      uint64_t alignment,
      uint64_t size,
      AllocatorSuballocationType suballocationType,
      void *pUserData) {
    ASSERT(m_Type == ALLOCATION_TYPE_NONE);
    ASSERT(block != nullptr);
    m_Type = ALLOCATION_TYPE_BLOCK;
    m_Alignment = alignment;
    m_Size = size;
    m_pUserData = pUserData;
    m_SuballocationType = suballocationType;
    m_BlockAllocation.m_Block = block;
    m_BlockAllocation.m_Offset = offset;
  }

  void ChangeBlockAllocation(AllocatorBlock *block, uint64_t offset) {
    ASSERT(block != nullptr);
    ASSERT(m_Type == ALLOCATION_TYPE_BLOCK);
    m_BlockAllocation.m_Block = block;
    m_BlockAllocation.m_Offset = offset;
  }

  void InitOwnAllocation(
      uint32_t memoryTypeIndex,
      AllocatorSuballocationType suballocationType,
      bool persistentMap,
      void *pMappedData,
      uint64_t size,
      void *pUserData) {
    ASSERT(m_Type == ALLOCATION_TYPE_NONE);
    m_Type = ALLOCATION_TYPE_OWN;
    m_Alignment = 0;
    m_Size = size;
    m_pUserData = pUserData;
    m_SuballocationType = suballocationType;
    m_OwnAllocation.m_MemoryTypeIndex = memoryTypeIndex;
    m_OwnAllocation.m_PersistentMap = persistentMap;
    m_OwnAllocation.m_pMappedData = pMappedData;
  }

  ALLOCATION_TYPE GetType() const { return m_Type; }
  uint64_t GetAlignment() const { return m_Alignment; }
  uint64_t GetSize() const { return m_Size; }
  void *GetUserData() const { return m_pUserData; }
  void SetUserData(void *pUserData) { m_pUserData = pUserData; }
  AllocatorSuballocationType GetSuballocationType() const { return m_SuballocationType; }

  AllocatorBlock *GetBlock() const {
    ASSERT(m_Type == ALLOCATION_TYPE_BLOCK);
    return m_BlockAllocation.m_Block;
  }
  uint64_t GetOffset() const { return (m_Type == ALLOCATION_TYPE_BLOCK) ? m_BlockAllocation.m_Offset : 0; }
  //id<MTLHeap>                GetMemory() const;
  //id<MTLBuffer>              GetResource() const;
  uint32_t GetMemoryTypeIndex() const;
  ResourceBlockVectorType GetBlockVectorType() const;
  void *GetMappedData() const;
  OwnAllocation *GetOwnAllocation() { return &m_OwnAllocation; }

  bool OwnAllocMapPersistentlyMappedMemory() {
    ASSERT(m_Type == ALLOCATION_TYPE_OWN);
    if (m_OwnAllocation.m_PersistentMap) {
      m_OwnAllocation.m_pMappedData = m_OwnAllocation.m_Buffer.contents;
    }
    return true;
  }
  void OwnAllocUnmapPersistentlyMappedMemory() {
    ASSERT(m_Type == ALLOCATION_TYPE_OWN);
    if (m_OwnAllocation.m_pMappedData) {
      ASSERT(m_OwnAllocation.m_PersistentMap);
      m_OwnAllocation.m_pMappedData = nil;
    }
  }

 private:
  uint64_t m_Alignment;
  uint64_t m_Size;
  void *m_pUserData;
  ALLOCATION_TYPE m_Type;
  AllocatorSuballocationType m_SuballocationType;

  union {
    // Allocation out of AllocatorBlock.
    BlockAllocation m_BlockAllocation;

    // Allocation for an object that has its own private VkDeviceMemory.
    OwnAllocation m_OwnAllocation;
  };
};

// -------------------------------------------------------------------------------------------------
// ResourceAllocator struct declaration.
// -------------------------------------------------------------------------------------------------

// Main allocator object.
struct ResourceAllocator {
  bool m_UseMutex;
//  id <MTLDevice> m_Device;
  bool m_AllocationCallbacksSpecified;
  uint64_t m_PreferredLargeHeapBlockSize;
  uint64_t m_PreferredSmallHeapBlockSize;
  // Non-zero when we are inside UnmapPersistentlyMappedMemory...MapPersistentlyMappedMemory.
  // Counter to allow nested calls to these functions.
  uint32_t m_UnmapPersistentlyMappedMemoryCounter;

  struct AllocatorBlockVector
      *m_pBlockVectors[(int) AllocatorMemoryType::NUM_TYPES][(int) ResourceBlockVectorType::COUNT];
  /* There can be at most one allocation that is completely empty - a
   hysteresis to avoid pessimistic case of alternating creation and destruction
   of a VkDeviceMemory. */
  bool m_HasEmptyBlock[(int) AllocatorMemoryType::NUM_TYPES];
  Os_Mutex_t *m_BlocksMutex[(int) AllocatorMemoryType::NUM_TYPES];

  // Each vector is sorted by memory (handle value).
  typedef tinystl::vector<ResourceAllocation *> AllocationVectorType;
  AllocationVectorType *m_pOwnAllocations[(int) AllocatorMemoryType::NUM_TYPES][(int) ResourceBlockVectorType::COUNT];
  Os_Mutex_t *m_OwnAllocationsMutex[(int) AllocatorMemoryType::NUM_TYPES];

  ResourceAllocator(const AllocatorCreateInfo *pCreateInfo);
  ~ResourceAllocator();

  uint64_t GetPreferredBlockSize(ResourceMemoryUsage memUsage, uint32_t memTypeIndex) const;

  uint64_t GetBufferImageGranularity() const {
    return RESOURCE_MAX(static_cast<uint64_t>(RESOURCE_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY), 1);
  }

  uint32_t GetMemoryHeapCount() const { return (int) AllocatorMemoryType::NUM_TYPES; }
  uint32_t GetMemoryTypeCount() const { return (int) AllocatorMemoryType::NUM_TYPES; }

  // Main allocation function.
  bool AllocateMemory(
      const AllocationInfo& info,
      const AllocatorMemoryRequirements& resourceAllocMemReq,
      AllocatorSuballocationType suballocType,
      ResourceAllocation **pAllocation);

  // Main deallocation function.
  void FreeMemory(ResourceAllocation *allocation);

  void CalculateStats(AllocatorStats *pStats);

#if RESOURCE_STATS_STRING_ENABLED
  void PrintDetailedMap(class AllocatorStringBuilder& sb);
#endif

  void UnmapPersistentlyMappedMemory();
  bool MapPersistentlyMappedMemory();

  static void GetAllocationInfo(ResourceAllocation *hAllocation, ResourceAllocationInfo *pAllocationInfo);

 private:
  bool AllocateMemoryOfType(
      const MTLSizeAndAlign& mtlMemReq, const AllocatorMemoryRequirements& resourceAllocMemReq, uint32_t memTypeIndex,
      AllocatorSuballocationType suballocType, ResourceAllocation **pAllocation);

  // Allocates and registers new VkDeviceMemory specifically for single allocation.
  bool AllocateOwnMemory(
      uint64_t size, AllocatorSuballocationType suballocType, uint32_t memTypeIndex, bool map, void *pUserData,
      ResourceAllocation **pAllocation);

  // Tries to free pMemory as Own Memory. Returns true if found and freed.
  void FreeOwnMemory(ResourceAllocation *allocation);
};

}

#endif //WYRD_THEFORGE_ALLOCATOR_RESOURCE_ALLOCATOR_HPP
