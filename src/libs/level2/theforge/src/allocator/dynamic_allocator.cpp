#include "core/core.h"
#include "core/logger.h"
#include "os/thread.hpp"
#include "tinystl/vector.h"

#include "theforge/renderer.hpp"
#include "dynamic_allocator.hpp"
#include "allocator_list.hpp"
#include "allocator_pool.hpp"

static inline unsigned int round_up(unsigned int value, unsigned int multiple) {
  return ((value + multiple - 1) / multiple) * multiple;
}
static inline uint64_t round_up_64(uint64_t value, uint64_t multiple) {
  return ((value + multiple - 1) / multiple) * multiple;
}
// Aligns given value up to nearest multiply of align value. For example: AllocatorAlignUp(11, 8) = 16.
// Use types like uint32_t, uint64_t as T.
template<typename T>
static inline T AllocatorAlignUp(T val, T align) {
  return (val + align - 1) / align * align;
}

// Division with mathematical rounding to nearest number.
template<typename T>
inline T AllocatorRoundDiv(T x, T y) {
  return (x + (y / (T) 2)) / y;
}


// Define this macro to 1 to enable functions: resourceAllocBuildStatsString, resourceAllocFreeStatsString.
#define RESOURCE_STATS_STRING_ENABLED 1

namespace TheForge {

DynamicMemoryAllocator *DynamicMemoryAllocator::Create(Renderer *renderer, uint64_t size) {
  ASSERT(renderer);

  auto *pAllocator = (DynamicMemoryAllocator *) malloc(sizeof(DynamicMemoryAllocator));
  pAllocator->mCurrentPos = 0;
  pAllocator->mSize = size;
  pAllocator->pRenderer = renderer;
  Os_MutexCreate(pAllocator->pMutex);

  BufferDesc desc = {};
  desc.mDescriptors = (TheForge_DescriptorType) (DESCRIPTOR_TYPE_INDEX_BUFFER | DESCRIPTOR_TYPE_VERTEX_BUFFER
      | DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  desc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
  desc.mSize = pAllocator->mSize;
  desc.mFlags = BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
  //addBuffer(renderer, &desc, &pAllocator->pBuffer);

  pAllocator->mAlignment = renderer->pActiveGpuSettings->mUniformBufferAlignment;

  return pAllocator;
}

void DynamicMemoryAllocator::Destroy(DynamicMemoryAllocator *allocator) {

//  removeBuffer(allocator, allocator->pBuffer);

  Os_MutexDestroy(allocator->pMutex);
  free(allocator);
}

void DynamicMemoryAllocator::Consume(
    uint64_t size,
    void **ppCpuAddress,
    uint64_t *pOffset) {
  Os::MutexLock lock(pMutex);

  if (mCurrentPos + size > mSize) {
    Reset();
  }

  *ppCpuAddress = (uint8_t *) pBuffer->pCpuMappedAddress + mCurrentPos;
  *pOffset = mCurrentPos;

  // Increment position by multiple of 256 to use CBVs in same heap as other buffers
  mCurrentPos += round_up_64(size, mAlignment);
}

void DynamicMemoryAllocator::ConsumeLockFree(uint64_t size, void **ppCpuAddress, uint64_t *pOffset) {
  if (mCurrentPos + size > mSize) {
    Reset();
  }

  *ppCpuAddress = (uint8_t *) pBuffer->pCpuMappedAddress + mCurrentPos;
  *pOffset = mCurrentPos;

  // Increment position by multiple of 256 to use CBVs in same heap as other buffers
  mCurrentPos += round_up_64(size, mAlignment);
}

// -------------------------------------------------------------------------------------------------
// Helper functions and classes
// -------------------------------------------------------------------------------------------------
#define RESOURCE_SWAP(a, b) { auto c = a; a = b; b = c; }

#ifndef RESOURCE_SORT

template<typename Iterator, typename Compare>
Iterator AllocatorQuickSortPartition(Iterator beg, Iterator end, Compare cmp) {
  Iterator centerValue = end;
  --centerValue;
  Iterator insertIndex = beg;
  for (Iterator i = beg; i < centerValue; ++i) {
    if (cmp(*i, *centerValue)) {
      if (insertIndex != i) {
        RESOURCE_SWAP(*i, *insertIndex);
      }
      ++insertIndex;
    }
  }
  if (insertIndex != centerValue) {
    RESOURCE_SWAP(*insertIndex, *centerValue);
  }
  return insertIndex;
}

template<typename Iterator, typename Compare>
void AllocatorQuickSort(Iterator beg, Iterator end, Compare cmp) {
  if (beg < end) {
    Iterator it = AllocatorQuickSortPartition<Iterator, Compare>(beg, end, cmp);
    AllocatorQuickSort < Iterator, Compare > (beg, it, cmp);
    AllocatorQuickSort < Iterator, Compare > (it + 1, end, cmp);
  }
}

#define RESOURCE_SORT(beg, end, cmp) AllocatorQuickSort(beg, end, cmp)

#endif    // #ifndef RESOURCE_SORT

/*
 Returns true if two memory blocks occupy overlapping pages.
 ResourceA must be in less memory offset than ResourceB.
 Algorithm is based on "Vulkan 1.0.39 - A Specification (with all registered Vulkan extensions)"
 chapter 11.6 "Resource Memory Association", paragraph "Buffer-Image Granularity".
 */
static inline bool AllocatorBlocksOnSamePage(uint64_t resourceAOffset,
                                             uint64_t resourceASize,
                                             uint64_t resourceBOffset,
                                             uint64_t pageSize) {
  ASSERT(resourceAOffset + resourceASize <= resourceBOffset && resourceASize > 0 && pageSize > 0);
  uint64_t resourceAEnd = resourceAOffset + resourceASize - 1;
  uint64_t resourceAEndPage = resourceAEnd & ~(pageSize - 1);
  uint64_t resourceBStart = resourceBOffset;
  uint64_t resourceBStartPage = resourceBStart & ~(pageSize - 1);
  return resourceAEndPage == resourceBStartPage;
}

/*
 Returns true if given suballocation types could conflict and must respect
 VkPhysicalDeviceLimits::bufferImageGranularity. They conflict if one is buffer
 or linear image and another one is optimal image. If type is unknown, behave
 conservatively.
 */
// TODO: Review this function.
bool
AllocatorIsBufferImageGranularityConflict(AllocatorSuballocationType suballocType1,
                                          AllocatorSuballocationType suballocType2) {
  if (suballocType1 > suballocType2) {
    RESOURCE_SWAP(suballocType1, suballocType2);
  }

  switch (suballocType1) {
    case AllocatorSuballocationType::FREE: return false;
    case AllocatorSuballocationType::UNKNOWN: return true;
    case AllocatorSuballocationType::BUFFER:
      return suballocType2 == AllocatorSuballocationType::IMAGE_UNKNOWN
          || suballocType2 == AllocatorSuballocationType::IMAGE_OPTIMAL;
    case AllocatorSuballocationType::IMAGE_UNKNOWN:
      return suballocType2 == AllocatorSuballocationType::IMAGE_UNKNOWN ||
          suballocType2 == AllocatorSuballocationType::IMAGE_LINEAR
          || suballocType2 == AllocatorSuballocationType::IMAGE_OPTIMAL;
    case AllocatorSuballocationType::IMAGE_LINEAR: return suballocType2 == AllocatorSuballocationType::IMAGE_OPTIMAL;
    case AllocatorSuballocationType::IMAGE_OPTIMAL: return false;
    default: ASSERT(0);
      return true;
  }
}

// Helper RAII class to lock a mutex in constructor and unlock it in destructor (at the end of scope).
struct AllocatorMutexLock {
 public:
  AllocatorMutexLock(Os_Mutex_t *mutex, bool useMutex) : m_pMutex(useMutex ? mutex : nullptr) {
    if (m_pMutex) {
      Os_MutexAcquire(m_pMutex);
    }
  }

  ~AllocatorMutexLock() {
    if (m_pMutex) {
      Os_MutexRelease(m_pMutex);
    }
  }

 private:
  Os_Mutex_t *m_pMutex;
};

#if RESOURCE_DEBUG_GLOBAL_MUTEX
static RESOURCE_MUTEX gDebugGlobalMutex;
#define RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK AllocatorMutexLock debugGlobalMutexLock(gDebugGlobalMutex);
#else
#define RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK
#endif

/*
 Performs binary search and returns iterator to first element that is greater or
 equal to (key), according to comparison (cmp).
 Cmp should return true if first argument is less than second argument.
 Returned value is the found element, if present in the collection or place where
 new element with value (key) should be inserted.
 */
template<typename IterT, typename KeyT, typename CmpT>
static IterT AllocatorBinaryFindFirstNotLess(IterT beg, IterT end, const KeyT& key, CmpT cmp) {
  size_t down = 0, up = (end - beg);
  while (down < up) {
    const size_t mid = (down + up) / 2;
    if (cmp(*(beg + mid), key)) {
      down = mid + 1;
    } else {
      up = mid;
    }
  }
  return beg + down;
}

// -------------------------------------------------------------------------------------------------
// Memory allocation functions.
// -------------------------------------------------------------------------------------------------

static void *AllocatorMalloc(size_t size) {
  return malloc(size);
}

static void AllocatorFree(void *ptr) {
  free(ptr);
}

template<typename T>
static T *AllocatorAllocate() {
  return (T *) AllocatorMalloc(sizeof(T));
}

template<typename T>
static T *AllocatorAllocateArray(size_t count) {
  return (T *) AllocatorMalloc(sizeof(T) * count);
}

template<typename T>
static void resourceAlloc_delete(T *ptr) {
  ptr->~T();
  AllocatorFree(ptr);
}

template<typename T>
static void resourceAlloc_delete_array(T *ptr, size_t count) {
  if (ptr != nullptr) {
    for (size_t i = count; i--;) {
      ptr[i].~T();
    }
    AllocatorFree(ptr);
  }
}

#define AllocatorVector tinystl::vector

template<typename T>
static void VectorInsert(AllocatorVector<T>& vec, size_t index, const T& item) {
  vec.insert(vec.begin() + index, item);
}

template<typename T>
static void VectorRemove(AllocatorVector<T>& vec, size_t index) {
  vec.erase(vec.begin() + index);
}

#define AllocatorPair tinystl::pair
#define RESOURCE_MAP_TYPE(KeyT, ValueT) tinystl::unordered_map<KeyT, ValueT>

// -------------------------------------------------------------------------------------------------
// Resource allocation/suballocation declarations.
// -------------------------------------------------------------------------------------------------

class AllocatorBlock;




// -------------------------------------------------------------------------------------------------
// AllocatorBlock functionality implementation.
// -------------------------------------------------------------------------------------------------

// Struct used in some AllocatorBlock functions.
struct AllocatorSuballocationItemSizeLess {
  bool operator()(const AllocatorSuballocationList::iterator lhs,
                  const AllocatorSuballocationList::iterator rhs) const {
    return lhs->size < rhs->size;
  }
  bool operator()(const AllocatorSuballocationList::iterator lhs, uint64_t rhsSize) const {
    return lhs->size < rhsSize;
  }
};

AllocatorBlock::AllocatorBlock(ResourceAllocator *hAllocator) :
    m_MemoryTypeIndex(UINT32_MAX),
    m_BlockVectorType(RESOURCE_BLOCK_VECTOR_TYPE_COUNT),
    m_hMemory(NULL),
    m_Size(0),
    m_PersistentMap(false),
    m_pMappedData(RESOURCE_NULL),
    m_FreeCount(0),
    m_SumFreeSize(0) {
}

void AllocatorBlock::Init(
    uint32_t newMemoryTypeIndex,
    RESOURCE_BLOCK_VECTOR_TYPE newBlockVectorType,
    id <MTLHeap> newMemory,
    uint64_t newSize,
    bool persistentMap,
    void *pMappedData) {
  ASSERT(m_hMemory == nil);

  m_MemoryTypeIndex = newMemoryTypeIndex;
  m_BlockVectorType = newBlockVectorType;
  m_hMemory = newMemory;
  m_Size = newSize;
  m_PersistentMap = persistentMap;
  m_pMappedData = pMappedData;
  m_FreeCount = 1;
  m_SumFreeSize = newSize;

  m_Suballocations.clear();
  m_FreeSuballocationsBySize.clear();

  AllocatorSuballocation suballoc = {};
  suballoc.offset = 0;
  suballoc.size = newSize;
  suballoc.type = RESOURCE_SUBALLOCATION_TYPE_FREE;

  m_Suballocations.push_back(suballoc);
  AllocatorSuballocationList::iterator suballocItem = m_Suballocations.end();
  --suballocItem;
  m_FreeSuballocationsBySize.push_back(suballocItem);
}

void AllocatorBlock::Init(
    uint32_t newMemoryTypeIndex,
    RESOURCE_BLOCK_VECTOR_TYPE newBlockVectorType,
    id <MTLBuffer> newMemory,
    uint64_t newSize,
    bool persistentMap,
    void *pMappedData) {
  ASSERT(m_hMemory == nil);

  m_MemoryTypeIndex = newMemoryTypeIndex;
  m_BlockVectorType = newBlockVectorType;
  m_Buffer = newMemory;
  m_Size = newSize;
  m_PersistentMap = persistentMap;
  m_pMappedData = pMappedData;
  m_FreeCount = 1;
  m_SumFreeSize = newSize;

  m_Suballocations.clear();
  m_FreeSuballocationsBySize.clear();

  AllocatorSuballocation suballoc = {};
  suballoc.offset = 0;
  suballoc.size = newSize;
  suballoc.type = RESOURCE_SUBALLOCATION_TYPE_FREE;

  m_Suballocations.push_back(suballoc);
  AllocatorSuballocationList::iterator suballocItem = m_Suballocations.end();
  --suballocItem;
  m_FreeSuballocationsBySize.push_back(suballocItem);
}

void AllocatorBlock::Init(
    uint32_t newMemoryTypeIndex,
    RESOURCE_BLOCK_VECTOR_TYPE newBlockVectorType,
    id <MTLTexture> newMemory,
    uint64_t newSize,
    bool persistentMap,
    void *pMappedData) {
  ASSERT(m_hMemory == nil);

  m_MemoryTypeIndex = newMemoryTypeIndex;
  m_BlockVectorType = newBlockVectorType;
  m_Texture = newMemory;
  m_Size = newSize;
  m_PersistentMap = persistentMap;
  m_pMappedData = pMappedData;
  m_FreeCount = 1;
  m_SumFreeSize = newSize;

  m_Suballocations.clear();
  m_FreeSuballocationsBySize.clear();

  AllocatorSuballocation suballoc = {};
  suballoc.offset = 0;
  suballoc.size = newSize;
  suballoc.type = RESOURCE_SUBALLOCATION_TYPE_FREE;

  m_Suballocations.push_back(suballoc);
  AllocatorSuballocationList::iterator suballocItem = m_Suballocations.end();
  --suballocItem;
  m_FreeSuballocationsBySize.push_back(suballocItem);
}

void AllocatorBlock::Destroy(ResourceAllocator *allocator) {
  ASSERT(m_Buffer != NULL || m_hMemory != NULL);
  if (m_pMappedData != RESOURCE_NULL) {
    m_pMappedData = RESOURCE_NULL;
  }

  if (m_hMemory) {
    m_hMemory = nil;
  } else if (m_Buffer) {
    m_Buffer = nil;
  } else {
    m_Texture = nil;
  }
  m_hMemory = nil;
}

bool AllocatorBlock::Validate() const {
  if ((m_hMemory == nil) || (m_Size == 0) || m_Suballocations.empty()) {
    return false;
  }

  // Expected offset of new suballocation as calculates from previous ones.
  uint64_t calculatedOffset = 0;
  // Expected number of free suballocations as calculated from traversing their list.
  uint32_t calculatedFreeCount = 0;
  // Expected sum size of free suballocations as calculated from traversing their list.
  uint64_t calculatedSumFreeSize = 0;
  // Expected number of free suballocations that should be registered in
  // m_FreeSuballocationsBySize calculated from traversing their list.
  size_t freeSuballocationsToRegister = 0;
  // True if previous visisted suballocation was free.
  bool prevFree = false;

  for (AllocatorSuballocationList::const_iterator suballocItem = m_Suballocations.cbegin();
       suballocItem != m_Suballocations.cend();
       ++suballocItem) {
    const AllocatorSuballocation& subAlloc = *suballocItem;

    // Actual offset of this suballocation doesn't match expected one.
    if (subAlloc.offset != calculatedOffset) {
      return false;
    }

    const bool currFree = (subAlloc.type == RESOURCE_SUBALLOCATION_TYPE_FREE);
    // Two adjacent free suballocations are invalid. They should be merged.
    if (prevFree && currFree) {
      return false;
    }
    prevFree = currFree;

    if (currFree) {
      calculatedSumFreeSize += subAlloc.size;
      ++calculatedFreeCount;
      if (subAlloc.size >= RESOURCE_MIN_FREE_SUBALLOCATION_SIZE_TO_REGISTER) {
        ++freeSuballocationsToRegister;
      }
    }

    calculatedOffset += subAlloc.size;
  }

  // Number of free suballocations registered in m_FreeSuballocationsBySize doesn't
  // match expected one.
  if (m_FreeSuballocationsBySize.size() != freeSuballocationsToRegister) {
    return false;
  }

  uint64_t lastSize = 0;
  for (size_t i = 0; i < m_FreeSuballocationsBySize.size(); ++i) {
    AllocatorSuballocationList::iterator suballocItem = m_FreeSuballocationsBySize[i];

    // Only free suballocations can be registered in m_FreeSuballocationsBySize.
    if (suballocItem->type != RESOURCE_SUBALLOCATION_TYPE_FREE) {
      return false;
    }
    // They must be sorted by size ascending.
    if (suballocItem->size < lastSize) {
      return false;
    }

    lastSize = suballocItem->size;
  }

  // Check if totals match calculacted values.
  return (calculatedOffset == m_Size) && (calculatedSumFreeSize == m_SumFreeSize)
      && (calculatedFreeCount == m_FreeCount);
}

bool AllocatorBlock::CreateAllocationRequest(
    uint64_t bufferImageGranularity, uint64_t allocSize, uint64_t allocAlignment, AllocatorSuballocationType allocType,
    AllocatorAllocationRequest *pAllocationRequest) {
  ASSERT(allocSize > 0);
  ASSERT(allocType != RESOURCE_SUBALLOCATION_TYPE_FREE);
  ASSERT(pAllocationRequest != RESOURCE_NULL);
  RESOURCE_HEAVY_ASSERT(Validate());

  // There is not enough total free space in this allocation to fullfill the request: Early return.
  if (m_SumFreeSize < allocSize) {
    return false;
  }

  // New algorithm, efficiently searching freeSuballocationsBySize.
  const size_t freeSuballocCount = m_FreeSuballocationsBySize.size();
  if (freeSuballocCount > 0) {
    if (RESOURCE_BEST_FIT) {
      // Find first free suballocation with size not less than allocSize.
      AllocatorSuballocationList::iterator *const it = AllocatorBinaryFindFirstNotLess(
          m_FreeSuballocationsBySize.data(), m_FreeSuballocationsBySize.data() + freeSuballocCount, allocSize,
          AllocatorSuballocationItemSizeLess());
      size_t index = it - m_FreeSuballocationsBySize.data();
      for (; index < freeSuballocCount; ++index) {
        uint64_t offset = 0;
        const AllocatorSuballocationList::iterator suballocItem = m_FreeSuballocationsBySize[index];
        if (CheckAllocation(bufferImageGranularity, allocSize, allocAlignment, allocType, suballocItem, &offset)) {
          pAllocationRequest->freeSuballocationItem = suballocItem;
          pAllocationRequest->offset = offset;
          return true;
        }
      }
    } else {
      // Search staring from biggest suballocations.
      for (size_t index = freeSuballocCount; index--;) {
        uint64_t offset = 0;
        const AllocatorSuballocationList::iterator suballocItem = m_FreeSuballocationsBySize[index];
        if (CheckAllocation(bufferImageGranularity, allocSize, allocAlignment, allocType, suballocItem, &offset)) {
          pAllocationRequest->freeSuballocationItem = suballocItem;
          pAllocationRequest->offset = offset;
          return true;
        }
      }
    }
  }
  return false;
}

bool AllocatorBlock::CheckAllocation(
    uint64_t bufferImageGranularity, uint64_t allocSize, uint64_t allocAlignment, AllocatorSuballocationType allocType,
    AllocatorSuballocationList::const_iterator freeSuballocItem, uint64_t *pOffset) const {
  ASSERT(allocSize > 0);
  ASSERT(allocType != RESOURCE_SUBALLOCATION_TYPE_FREE);
  ASSERT(freeSuballocItem != m_Suballocations.cend());
  ASSERT(pOffset != RESOURCE_NULL);

  const AllocatorSuballocation& suballoc = *freeSuballocItem;
  ASSERT(suballoc.type == RESOURCE_SUBALLOCATION_TYPE_FREE);

  // Size of this suballocation is too small for this request: Early return.
  if ([m_hMemory
  maxAvailableSizeWithAlignment:
  allocAlignment] < allocSize)
  {
    return false;
  }

  // Start from offset equal to beginning of this suballocation.
  *pOffset = suballoc.offset;

  // Apply RESOURCE_DEBUG_MARGIN at the beginning.
  if ((RESOURCE_DEBUG_MARGIN > 0) && freeSuballocItem != m_Suballocations.cbegin()) {
    *pOffset += RESOURCE_DEBUG_MARGIN;
  }

  // Apply alignment.
  const uint64_t alignment = RESOURCE_MAX(allocAlignment, static_cast<uint64_t>(RESOURCE_DEBUG_ALIGNMENT));
  *pOffset = AllocatorAlignUp(*pOffset, alignment);

  // Check previous suballocations for BufferImageGranularity conflicts.
  // Make bigger alignment if necessary.
  if (bufferImageGranularity > 1) {
    bool bufferImageGranularityConflict = false;
    AllocatorSuballocationList::const_iterator prevSuballocItem = freeSuballocItem;
    while (prevSuballocItem != m_Suballocations.cbegin()) {
      --prevSuballocItem;
      const AllocatorSuballocation& prevSuballoc = *prevSuballocItem;
      if (AllocatorBlocksOnSamePage(prevSuballoc.offset, prevSuballoc.size, *pOffset, bufferImageGranularity)) {
        if (AllocatorIsBufferImageGranularityConflict(prevSuballoc.type, allocType)) {
          bufferImageGranularityConflict = true;
          break;
        }
      } else {
        // Already on previous page.
        break;
      }
    }
    if (bufferImageGranularityConflict) {
      *pOffset = AllocatorAlignUp(*pOffset, bufferImageGranularity);
    }
  }

  // Calculate padding at the beginning based on current offset.
  const uint64_t paddingBegin = *pOffset - suballoc.offset;

  // Calculate required margin at the end if this is not last suballocation.
  AllocatorSuballocationList::const_iterator next = freeSuballocItem;
  ++next;
  const uint64_t requiredEndMargin = (next != m_Suballocations.cend()) ? RESOURCE_DEBUG_MARGIN : 0;

  // Fail if requested size plus margin before and after is bigger than size of this suballocation.
  if (paddingBegin + allocSize + requiredEndMargin > suballoc.size) {
    return false;
  }

  // Check next suballocations for BufferImageGranularity conflicts.
  // If conflict exists, allocation cannot be made here.
  if (bufferImageGranularity > 1) {
    AllocatorSuballocationList::const_iterator nextSuballocItem = freeSuballocItem;
    ++nextSuballocItem;
    while (nextSuballocItem != m_Suballocations.cend()) {
      const AllocatorSuballocation& nextSuballoc = *nextSuballocItem;
      if (AllocatorBlocksOnSamePage(*pOffset, allocSize, nextSuballoc.offset, bufferImageGranularity)) {
        if (AllocatorIsBufferImageGranularityConflict(allocType, nextSuballoc.type)) {
          return false;
        }
      } else {
        // Already on next page.
        break;
      }
      ++nextSuballocItem;
    }
  }

  // All tests passed: Success. pOffset is already filled.
  return true;
}

bool AllocatorBlock::IsEmpty() const { return (m_Suballocations.size() == 1) && (m_FreeCount == 1); }

void AllocatorBlock::Alloc(const AllocatorAllocationRequest& request,
                           AllocatorSuballocationType type,
                           uint64_t allocSize) {
  ASSERT(request.freeSuballocationItem != m_Suballocations.end());
  AllocatorSuballocation& suballoc = *request.freeSuballocationItem;
  // Given suballocation is a free block.
  ASSERT(suballoc.type == RESOURCE_SUBALLOCATION_TYPE_FREE);
  // Given offset is inside this suballocation.
  ASSERT(request.offset >= suballoc.offset);
  const uint64_t paddingBegin = request.offset - suballoc.offset;
  ASSERT(suballoc.size >= paddingBegin + allocSize);
  const uint64_t paddingEnd = suballoc.size - paddingBegin - allocSize;

  // Unregister this free suballocation from m_FreeSuballocationsBySize and update
  // it to become used.
  UnregisterFreeSuballocation(request.freeSuballocationItem);

  suballoc.offset = request.offset;
  suballoc.size = allocSize;
  suballoc.type = type;

  // If there are any free bytes remaining at the end, insert new free suballocation after current one.
  if (paddingEnd) {
    AllocatorSuballocation paddingSuballoc = {};
    paddingSuballoc.offset = request.offset + allocSize;
    paddingSuballoc.size = paddingEnd;
    paddingSuballoc.type = RESOURCE_SUBALLOCATION_TYPE_FREE;
    AllocatorSuballocationList::iterator next = request.freeSuballocationItem;
    ++next;
    const AllocatorSuballocationList::iterator paddingEndItem = m_Suballocations.insert(next, paddingSuballoc);
    RegisterFreeSuballocation(paddingEndItem);
  }

  // If there are any free bytes remaining at the beginning, insert new free suballocation before current one.
  if (paddingBegin) {
    AllocatorSuballocation paddingSuballoc = {};
    paddingSuballoc.offset = request.offset - paddingBegin;
    paddingSuballoc.size = paddingBegin;
    paddingSuballoc.type = RESOURCE_SUBALLOCATION_TYPE_FREE;
    const AllocatorSuballocationList::iterator paddingBeginItem =
        m_Suballocations.insert(request.freeSuballocationItem, paddingSuballoc);
    RegisterFreeSuballocation(paddingBeginItem);
  }

  // Update totals.
  m_FreeCount = m_FreeCount - 1;
  if (paddingBegin > 0) {
    ++m_FreeCount;
  }
  if (paddingEnd > 0) {
    ++m_FreeCount;
  }
  m_SumFreeSize -= allocSize;
}

void AllocatorBlock::FreeSuballocation(AllocatorSuballocationList::iterator suballocItem) {
  // Change this suballocation to be marked as free.
  AllocatorSuballocation& suballoc = *suballocItem;
  suballoc.type = RESOURCE_SUBALLOCATION_TYPE_FREE;

  // Update totals.
  ++m_FreeCount;
  m_SumFreeSize += suballoc.size;

  // Merge with previous and/or next suballocation if it's also free.
  bool mergeWithNext = false;
  bool mergeWithPrev = false;

  AllocatorSuballocationList::iterator nextItem = suballocItem;
  ++nextItem;
  if ((nextItem != m_Suballocations.end()) && (nextItem->type == RESOURCE_SUBALLOCATION_TYPE_FREE)) {
    mergeWithNext = true;
  }

  AllocatorSuballocationList::iterator prevItem = suballocItem;
  if (suballocItem != m_Suballocations.begin()) {
    --prevItem;
    if (prevItem->type == RESOURCE_SUBALLOCATION_TYPE_FREE) {
      mergeWithPrev = true;
    }
  }

  if (mergeWithNext) {
    UnregisterFreeSuballocation(nextItem);
    MergeFreeWithNext(suballocItem);
  }

  if (mergeWithPrev) {
    UnregisterFreeSuballocation(prevItem);
    MergeFreeWithNext(prevItem);
    RegisterFreeSuballocation(prevItem);
  } else {
    RegisterFreeSuballocation(suballocItem);
  }
}

void AllocatorBlock::Free(const ResourceAllocation *allocation) {
  const uint64_t allocationOffset = allocation->GetOffset();
  for (AllocatorSuballocationList::iterator suballocItem = m_Suballocations.begin();
       suballocItem != m_Suballocations.end();
       ++suballocItem) {
    AllocatorSuballocation& suballoc = *suballocItem;
    if (suballoc.offset == allocationOffset) {
      FreeSuballocation(suballocItem);
      RESOURCE_HEAVY_ASSERT(Validate());
      return;
    }
  }
  ASSERT(0 && "Not found!");
}

#if RESOURCE_STATS_STRING_ENABLED

void AllocatorBlock::PrintDetailedMap(class AllocatorStringBuilder& sb) const {
  sb.Add("{\n\t\t\t\"Bytes\": ");
  sb.AddNumber(m_Size);
  sb.Add(",\n\t\t\t\"FreeBytes\": ");
  sb.AddNumber(m_SumFreeSize);
  sb.Add(",\n\t\t\t\"Suballocations\": ");
  sb.AddNumber((uint64_t) m_Suballocations.size());
  sb.Add(",\n\t\t\t\"FreeSuballocations\": ");
  sb.AddNumber(m_FreeCount);
  sb.Add(",\n\t\t\t\"SuballocationList\": [");

  size_t i = 0;
  for (AllocatorSuballocationList::const_iterator suballocItem = m_Suballocations.cbegin();
       suballocItem != m_Suballocations.cend();
       ++suballocItem, ++i) {
    if (i > 0) {
      sb.Add(",\n\t\t\t\t{ \"Type\": ");
    } else {
      sb.Add("\n\t\t\t\t{ \"Type\": ");
    }
    sb.AddString(RESOURCE_SUBALLOCATION_TYPE_NAMES[suballocItem->type]);
    sb.Add(", \"Size\": ");
    sb.AddNumber(suballocItem->size);
    sb.Add(", \"Offset\": ");
    sb.AddNumber(suballocItem->offset);
    sb.Add(" }");
  }

  sb.Add("\n\t\t\t]\n\t\t}");
}

#endif    // #if RESOURCE_STATS_STRING_ENABLED

void AllocatorBlock::MergeFreeWithNext(AllocatorSuballocationList::iterator item) {
  ASSERT(item != m_Suballocations.end());
  ASSERT(item->type == RESOURCE_SUBALLOCATION_TYPE_FREE);

  AllocatorSuballocationList::iterator nextItem = item;
  ++nextItem;
  ASSERT(nextItem != m_Suballocations.end());
  ASSERT(nextItem->type == RESOURCE_SUBALLOCATION_TYPE_FREE);

  item->size += nextItem->size;
  --m_FreeCount;
  m_Suballocations.erase(nextItem);
}

void AllocatorBlock::RegisterFreeSuballocation(AllocatorSuballocationList::iterator item) {
  ASSERT(item->type == RESOURCE_SUBALLOCATION_TYPE_FREE);
  ASSERT(item->size > 0);

  if (item->size >= RESOURCE_MIN_FREE_SUBALLOCATION_SIZE_TO_REGISTER) {
    if (m_FreeSuballocationsBySize.empty()) {
      m_FreeSuballocationsBySize.push_back(item);
    } else {
      AllocatorSuballocationList::iterator *const it = AllocatorBinaryFindFirstNotLess(
          m_FreeSuballocationsBySize.data(),
          m_FreeSuballocationsBySize.data() + m_FreeSuballocationsBySize.size(),
          item,
          AllocatorSuballocationItemSizeLess());
      size_t index = it - m_FreeSuballocationsBySize.data();
      VectorInsert(m_FreeSuballocationsBySize, index, item);
    }
  }
}

void AllocatorBlock::UnregisterFreeSuballocation(AllocatorSuballocationList::iterator item) {
  ASSERT(item->type == RESOURCE_SUBALLOCATION_TYPE_FREE);
  ASSERT(item->size > 0);

  if (item->size >= RESOURCE_MIN_FREE_SUBALLOCATION_SIZE_TO_REGISTER) {
    AllocatorSuballocationList::iterator *const it = AllocatorBinaryFindFirstNotLess(
        m_FreeSuballocationsBySize.data(), m_FreeSuballocationsBySize.data() + m_FreeSuballocationsBySize.size(), item,
        AllocatorSuballocationItemSizeLess());
    for (size_t index = it - m_FreeSuballocationsBySize.data(); index < m_FreeSuballocationsBySize.size(); ++index) {
      if (m_FreeSuballocationsBySize[index] == item) {
        VectorRemove(m_FreeSuballocationsBySize, index);
        return;
      }
      ASSERT((m_FreeSuballocationsBySize[index]->size == item->size) && "Not found.");
    }
    ASSERT(0 && "Not found.");
  }
}

// -------------------------------------------------------------------------------------------------
// AllocatorBlockVector functionality implementation.
// -------------------------------------------------------------------------------------------------

AllocatorBlockVector::AllocatorBlockVector(ResourceAllocator *hAllocator) : m_hAllocator(hAllocator) {}

AllocatorBlockVector::~AllocatorBlockVector() {
  for (size_t i = m_Blocks.size(); i--;) {
    m_Blocks[i]->Destroy(m_hAllocator);
    resourceAlloc_delete(m_Blocks[i]);
  }
}

void AllocatorBlockVector::Remove(AllocatorBlock *pBlock) {
  for (uint32_t blockIndex = 0; blockIndex < m_Blocks.size(); ++blockIndex) {
    if (m_Blocks[blockIndex] == pBlock) {
      VectorRemove(m_Blocks, blockIndex);
      return;
    }
  }
  ASSERT(0);
}

void AllocatorBlockVector::IncrementallySortBlocks() {
  // Bubble sort only until first swap.
  for (size_t i = 1; i < m_Blocks.size(); ++i) {
    if (m_Blocks[i - 1]->m_SumFreeSize > m_Blocks[i]->m_SumFreeSize) {
      RESOURCE_SWAP(m_Blocks[i - 1], m_Blocks[i]);
      return;
    }
  }
}

#if RESOURCE_STATS_STRING_ENABLED

void AllocatorBlockVector::PrintDetailedMap(class AllocatorStringBuilder& sb) const {
  for (size_t i = 0; i < m_Blocks.size(); ++i) {
    if (i > 0) {
      sb.Add(",\n\t\t");
    } else {
      sb.Add("\n\t\t");
    }
    m_Blocks[i]->PrintDetailedMap(sb);
  }
}

#endif    // #if RESOURCE_STATS_STRING_ENABLED

void AllocatorBlockVector::UnmapPersistentlyMappedMemory() {
  for (size_t i = m_Blocks.size(); i--;) {
    AllocatorBlock *pBlock = m_Blocks[i];
    if (pBlock->m_pMappedData != RESOURCE_NULL) {
      ASSERT(pBlock->m_PersistentMap != false);
      pBlock->m_pMappedData = RESOURCE_NULL;
    }
  }
}

bool AllocatorBlockVector::MapPersistentlyMappedMemory() {
  bool finalResult = true;
  for (size_t i = 0, count = m_Blocks.size(); i < count; ++i) {
    AllocatorBlock *pBlock = m_Blocks[i];
    if (pBlock->m_PersistentMap) {
      pBlock->m_pMappedData = pBlock->m_Buffer.contents;
    }
  }
  return finalResult;
}

void AllocatorBlockVector::AddStats(AllocatorStats *pStats, uint32_t memTypeIndex, uint32_t memHeapIndex) const {
  for (uint32_t allocIndex = 0; allocIndex < m_Blocks.size(); ++allocIndex) {
    const AllocatorBlock *const pBlock = m_Blocks[allocIndex];
    ASSERT(pBlock);
    RESOURCE_HEAVY_ASSERT(pBlock->Validate());
    AllocatorStatInfo allocationStatInfo;
    CalcAllocationStatInfo(allocationStatInfo, *pBlock);
    AllocatorAddStatInfo(pStats->total, allocationStatInfo);
    AllocatorAddStatInfo(pStats->memoryType[memTypeIndex], allocationStatInfo);
    AllocatorAddStatInfo(pStats->memoryHeap[memHeapIndex], allocationStatInfo);
  }
}

}