#pragma once
#ifndef WYRD_ALLOCATOR_HPP
#define WYRD_ALLOCATOR_HPP

#include "core/core.h"
#include "theforge/renderer.hpp"

namespace TheForge {

/// Allocation memory type.
enum class AllocatorMemoryType {
  DEFAULT_BUFFER = 0,
  UPLOAD_BUFFER,
  READBACK_BUFFER,
  TEXTURE_SMALL,
  TEXTURE_DEFAULT,
  TEXTURE_MS,
  TEXTURE_RTV_DSV,
  TEXTURE_RTV_DSV_MS,
  TEXTURE_RTV_DSV_SHARED,
  TEXTURE_RTV_DSV_SHARED_MS,
  TEXTURE_RTV_DSV_SHARED_ADAPTER,
  TEXTURE_RTV_DSV_SHARED_ADAPTER_MS,
  DEFAULT_UAV,
  UPLOAD_UAV,
  READBACK_UAV,
  NUM_TYPES
};

/// Suballocation type.
enum class AllocatorSuballocationType {
  FREE = 0,
  UNKNOWN = 1,
  BUFFER = 2,
  BUFFER_SRV_UAV = 3,
  IMAGE_UNKNOWN = 4,
  IMAGE_LINEAR = 5,
  IMAGE_OPTIMAL = 6,
  IMAGE_RTV_DSV = 7,
  IMAGE_RTV_DSV_SHARED = 8,
  IMAGE_RTV_DSV_SHARED_ADAPTER = 9,
};

/// Flags for created Allocator.
enum AllocatorFlagBits {
  /** Allocator and all objects created from it will not be synchronized internally, so you must guarantee they are used from only one thread at a time or synchronized externally by you.
   Using this flag may increase performance because internal mutexes are not used.
   */
      RESOURCE_ALLOCATOR_EXTERNALLY_SYNCHRONIZED_BIT = 0x00000001,
  RESOURCE_ALLOCATOR_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
};
typedef uint32_t AllocatorFlags;

/// Description of a Allocator to be created.
struct AllocatorCreateInfo {
  RendererApi api;        ///< Which renderer api is the allocator working for
  AllocatorFlags flags;   ///< Flags for created allocator. Use AllocatorFlagBits enum.

  /// Size of a single memory block to allocate for resources.
  /** Set to 0 to use default, which is currently 256 MB. */
  uint64_t preferredLargeHeapBlockSize;
  /// Size of a single memory block to allocate for resources from a small heap <= 512 MB.
  /** Set to 0 to use default, which is currently 64 MB. */
  uint64_t preferredSmallHeapBlockSize;

  void *apiCreateInfo;
};

struct AllocatorCreateInfoMetal {
  /// Metal device.
  /** It must be valid throughout whole lifetime of created Allocator. */
  id <MTLDevice> device;
};

// Info needed for each Metal allocation.
struct AllocationInfo {
//  MTLSizeAndAlign mSizeAlign;
  bool mIsRT;
  bool mIsMS;
};

/// Flags to be passed as AllocatorMemoryRequirements::flags.
// TODO: Review this enum.
enum AllocatorMemoryRequirementFlagBits {
  /** Set this flag if the allocation should have its own memory block.
   Use it for special, big resources, like fullscreen images used as attachments.
   This flag must also be used for host visible resources that you want to map
   simultaneously because otherwise they might end up as regions of the same
   VkDeviceMemory, while mapping same VkDeviceMemory multiple times is illegal.
   */
      RESOURCE_MEMORY_REQUIREMENT_OWN_MEMORY_BIT = 0x00000001,

  /** \brief Set this flag to only try to allocate from existing VkDeviceMemory blocks and never create new such block.
   If new allocation cannot be placed in any of the existing blocks, allocation
   fails with VK_ERROR_OUT_OF_DEVICE_MEMORY error.
   It makes no sense to set RESOURCE_MEMORY_REQUIREMENT_OWN_MEMORY_BIT and
   RESOURCE_MEMORY_REQUIREMENT_NEVER_ALLOCATE_BIT at the same time. */
      RESOURCE_MEMORY_REQUIREMENT_NEVER_ALLOCATE_BIT = 0x00000002,
  /** \brief Set to use a memory that will be persistently mapped and retrieve pointer to it.
   Pointer to mapped memory will be returned through AllocatorAllocationInfo::pMappedData. You cannot
   map the memory on your own as multiple maps of a single VkDeviceMemory are
   illegal.
   */
      RESOURCE_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT = 0x00000004,
  /** \brief Set to use a memory that can be shared with multiple processes.
   */
      RESOURCE_MEMORY_REQUIREMENT_SHARED_BIT = 0x00000008,
  /** \brief Set to use a memory that can be shared with multiple gpus.
   */
      RESOURCE_MEMORY_REQUIREMENT_SHARED_ADAPTER_BIT = 0x00000010,
  RESOURCE_MEMORY_REQUIREMENT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
};
typedef uint32_t AllocatorMemoryRequirementFlags;

struct AllocatorMemoryRequirements {
  AllocatorMemoryRequirementFlags flags;
  ResourceMemoryUsage usage;
  void *pUserData;
};

/** Parameters of AllocatorAllocation objects, that can be retrieved using function resourceAllocGetAllocationInfo().*/
struct ResourceAllocationInfo {
  /** \brief Memory type index that this allocation was allocated from.
   It never changes.
   */
  uint32_t memoryType;
  /** \brief Offset into deviceMemory object to the beginning of this allocation, in bytes. (deviceMemory, offset) pair is unique to this allocation.
   It can change after call to resourceAllocDefragment() if this allocation is passed to the function.
   */
  uint64_t offset;
  /** \brief Size of this allocation, in bytes.
   It never changes.
   */
  uint64_t size;
  /** \brief Pointer to the beginning of this allocation as mapped data. Null if this alloaction is not persistently mapped.
   It can change after call to resourceAllocUnmapPersistentlyMappedMemory(), resourceAllocMapPersistentlyMappedMemory().
   It can also change after call to resourceAllocDefragment() if this allocation is passed to the function.
   */
  void *pMappedData;
  /** \brief Custom general-purpose pointer that was passed as AllocatorMemoryRequirements::pUserData or set using resourceAllocSetAllocationUserData().
   It can change after call to resourceAllocSetAllocationUserData() for this allocation.
   */
  void *pUserData;

  void *apiInfo;
};

struct ResourceAllocationInfoMetal {
  /** \brief Handle to Metal Heap.
 Same memory object can be shared by multiple allocations.
 It can change after call to resourceAllocDefragment() if this allocation is passed to the function.
 */
  id <MTLHeap> deviceMemory;
  /** \brief Handle to Metal Buffer.
   Same memory object can be shared by multiple allocations.
   It can change after call to resourceAllocDefragment() if this allocation is passed to the function.
   */
  id <MTLBuffer> resource;
};

struct ResourceAllocatorHandle {};
struct ResourceAllocationHandle {};

bool AllocatorCreate(const AllocatorCreateInfo *pCreateInfo, ResourceAllocatorHandle *pAllocator);
void AllocatorDestroy(ResourceAllocatorHandle *allocator);
uint64_t AllocatorGetPreferredBlockSize(ResourceMemoryUsage memUsage, uint32_t memTypeIndex);
uint64_t AllocatorGetBufferImageGranularity();
uint32_t AllocatorGetMemoryHeapCount();
uint32_t AllocatorGetMemoryTypeCount();
bool AllocatorAllocateMemory(const AllocationInfo& info,
                             const AllocatorMemoryRequirements& resourceAllocMemReq,
                             AllocatorSuballocationType suballocType,
                             ResourceAllocationHandle *pAllocation);
void AllocatorFreeMemory(ResourceAllocationHandle *allocation);
void AllocatorUnmapPersistentlyMappedMemory();
bool AllocatorMapPersistentlyMappedMemory();
void GetAllocationInfo(ResourceAllocationHandle *hAllocation, ResourceAllocationInfo *pAllocationInfo);

bool ResourceAllocMapMemory(ResourceAllocatorHandle *allocator, ResourceAllocationHandle *allocation, void **ppData);
void ResourceAllocUnmapMemory(ResourceAllocatorHandle *allocator, ResourceAllocationHandle *allocation);

struct BufferCreateInfo {};
struct TextureCreateInfo {};

}

#endif //WYRD_ALLOCATOR_HPP
