#pragma once
#ifndef WYRD_THEFORGE_ALLOCATOR_ALLOCATOR_POOL_HPP
#define WYRD_THEFORGE_ALLOCATOR_ALLOCATOR_POOL_HPP

#include "core/core.h"
#include "core/logger.h"
#include "tinystl/vector.h"

namespace TheForge {

// -------------------------------------------------------------------------------------------------
// Allocator pool class.
// -------------------------------------------------------------------------------------------------

/*
 Allocator for objects of type T using a list of arrays (pools) to speed up
 allocation. Number of elements that can be allocated is not bounded because
 allocator can create multiple blocks.
 */
template<typename T>
class AllocatorPoolAllocator {
 public:
  AllocatorPoolAllocator(size_t itemsPerBlock);
  ~AllocatorPoolAllocator();
  void Clear();
  T *Alloc();
  void Free(T *ptr);

 private:
  union Item {
    uint32_t NextFreeIndex;
    T Value;
  };

  struct ItemBlock {
    Item *pItems;
    uint32_t FirstFreeIndex;
  };

  size_t m_ItemsPerBlock;
  tinystl::vector<ItemBlock> m_ItemBlocks;

  ItemBlock& CreateNewBlock();
};

template<typename T>
AllocatorPoolAllocator<T>::AllocatorPoolAllocator(size_t itemsPerBlock): m_ItemsPerBlock(itemsPerBlock) {
  ASSERT(itemsPerBlock > 0);
}

template<typename T>
AllocatorPoolAllocator<T>::~AllocatorPoolAllocator() {
  Clear();
}

template<typename T>
void AllocatorPoolAllocator<T>::Clear() {
  for (size_t i = m_ItemBlocks.size(); i--;) {
    resourceAlloc_delete_array(m_ItemBlocks[i].pItems, m_ItemsPerBlock);
  }
  m_ItemBlocks.clear();
}

template<typename T>
T *AllocatorPoolAllocator<T>::Alloc() {
  for (size_t i = m_ItemBlocks.size(); i--;) {
    ItemBlock& block = m_ItemBlocks[i];
    // This block has some free items: Use first one.
    if (block.FirstFreeIndex != UINT32_MAX) {
      Item *const pItem = &block.pItems[block.FirstFreeIndex];
      block.FirstFreeIndex = pItem->NextFreeIndex;
      return &pItem->Value;
    }
  }

  // No block has free item: Create new one and use it.
  ItemBlock& newBlock = CreateNewBlock();
  Item *const pItem = &newBlock.pItems[0];
  newBlock.FirstFreeIndex = pItem->NextFreeIndex;
  return &pItem->Value;
}

template<typename T>
void AllocatorPoolAllocator<T>::Free(T *ptr) {
  // Search all memory blocks to find ptr.
  for (size_t i = 0; i < m_ItemBlocks.size(); ++i) {
    ItemBlock& block = m_ItemBlocks[i];

    // Casting to union.
    Item *pItemPtr;
    memcpy(&pItemPtr, &ptr, sizeof(pItemPtr));

    // Check if pItemPtr is in address range of this block.
    if ((pItemPtr >= block.pItems) && (pItemPtr < block.pItems + m_ItemsPerBlock)) {
      const uint32_t index = static_cast<uint32_t>(pItemPtr - block.pItems);
      pItemPtr->NextFreeIndex = block.FirstFreeIndex;
      block.FirstFreeIndex = index;
      return;
    }
  }
  ASSERT(0 && "Pointer doesn't belong to this memory pool.");
}

template<typename T>
typename AllocatorPoolAllocator<T>::ItemBlock& AllocatorPoolAllocator<T>::CreateNewBlock() {
  ItemBlock newBlock = {
      (Item *) malloc(sizeof(Item) * m_ItemsPerBlock),
      0};
  memset(newBlock.pItems, 0, sizeof(Item) * m_ItemsPerBlock);

  m_ItemBlocks.push_back(newBlock);

  // Setup singly-linked list of all free items in this block.
  for (uint32_t i = 0; i < m_ItemsPerBlock - 1; ++i) {
    newBlock.pItems[i].NextFreeIndex = i + 1;
  }
  newBlock.pItems[m_ItemsPerBlock - 1].NextFreeIndex = UINT32_MAX;
  return m_ItemBlocks.back();
}

}

#endif //WYRD_THEFORGE_ALLOCATOR_ALLOCATOR_POOL_HPP
