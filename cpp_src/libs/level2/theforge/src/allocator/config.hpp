#pragma once
#ifndef WYRD_THEFORGE_ALLOCATOR_CONFIG_HPP
#define WYRD_THEFORGE_ALLOCATOR_CONFIG_HPP

#ifndef RESOURCE_BEST_FIT
/**
 Main parameter for function assessing how good is a free suballocation for a new
 allocation request.
 - Set to 1 to use Best-Fit algorithm - prefer smaller blocks, as close to the
 size of requested allocations as possible.
 - Set to 0 to use Worst-Fit algorithm - prefer larger blocks, as large as
 possible.
 Experiments in special testing environment showed that Best-Fit algorithm is
 better.
 */
#define RESOURCE_BEST_FIT (1)
#endif

#ifndef RESOURCE_DEBUG_ALWAYS_OWN_MEMORY
/**
 Every object will have its own allocation.
 Define to 1 for debugging purposes only.
 NOTE: Set this to 1 on Intel GPUs. It seems like suballocating buffers from heaps can give us problems on Intel hardware.
 */
#define RESOURCE_DEBUG_ALWAYS_OWN_MEMORY (0)
#endif

#ifndef RESOURCE_DEBUG_ALIGNMENT
/**
 Minimum alignment of all suballocations, in bytes.
 Set to more than 1 for debugging purposes only. Must be power of two.
 */
#define RESOURCE_DEBUG_ALIGNMENT (1)
#endif

#ifndef RESOURCE_DEBUG_MARGIN
/**
 Minimum margin between suballocations, in bytes.
 Set nonzero for debugging purposes only.
 */
#define RESOURCE_DEBUG_MARGIN (0)
#endif

#ifndef RESOURCE_DEBUG_GLOBAL_MUTEX
/**
 Set this to 1 for debugging purposes only, to enable single mutex protecting all
 entry calls to the library. Can be useful for debugging multithreading issues.
 */
#define RESOURCE_DEBUG_GLOBAL_MUTEX (0)
#endif

#ifndef RESOURCE_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY
/**
 Minimum value for VkPhysicalDeviceLimits::bufferImageGranularity.
 Set to more than 1 for debugging purposes only. Must be power of two.
 */
#define RESOURCE_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY (1)
#endif

#ifndef RESOURCE_SMALL_HEAP_MAX_SIZE
/// Maximum size of a memory heap in Metal to consider it "small".
#define RESOURCE_SMALL_HEAP_MAX_SIZE (512 * 1024 * 1024)
#endif

#ifndef RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE
/// Default size of a block allocated as single VkDeviceMemory from a "large" heap.
#define RESOURCE_DEFAULT_LARGE_HEAP_BLOCK_SIZE (64 * 1024 * 1024)    // 64 MB
#endif

#ifndef RESOURCE_DEFAULT_SMALL_HEAP_BLOCK_SIZE
/// Default size of a block allocated as single VkDeviceMemory from a "small" heap.
#define RESOURCE_DEFAULT_SMALL_HEAP_BLOCK_SIZE (16 * 1024 * 1024)    // 16 MB
#endif

#endif //WYRD_THEFORGE_ALLOCATOR_CONFIG_HPP
