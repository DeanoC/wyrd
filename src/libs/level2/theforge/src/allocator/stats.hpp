
#pragma once
#ifndef WYRD_THEFORGE_ALLOCATOR_STATS_HPP
#define WYRD_THEFORGE_ALLOCATOR_STATS_HPP

namespace TheForge {
/// Struct containing resource allocation statistics.
struct AllocatorStatInfo {
  uint32_t AllocationCount;
  uint32_t SuballocationCount;
  uint32_t UnusedRangeCount;
  uint64_t UsedBytes;
  uint64_t UnusedBytes;
  uint64_t SuballocationSizeMin, SuballocationSizeAvg, SuballocationSizeMax;
  uint64_t UnusedRangeSizeMin, UnusedRangeSizeAvg, UnusedRangeSizeMax;
};

/// General statistics from current state of Allocator.
struct AllocatorStats {
  AllocatorStatInfo memoryType[(int) AllocatorMemoryType::NUM_TYPES];
  AllocatorStatInfo memoryHeap[(int) AllocatorMemoryType::NUM_TYPES];
  AllocatorStatInfo total;
};
}
#endif //WYRD_THEFORGE_ALLOCATOR_STATS_HPP
