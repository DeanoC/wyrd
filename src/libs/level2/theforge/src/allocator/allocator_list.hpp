#pragma once
#ifndef WYRD_THEFORGE_ALLOCATOR_ALLOCATOR_LIST_HPP
#define WYRD_THEFORGE_ALLOCATOR_ALLOCATOR_LIST_HPP

#include "core/core.h"
#include "core/logger.h"
#include "allocator_pool.hpp"

namespace TheForge {

// AllocatorRawList and AllocatorList classes.

template<typename T>
struct AllocatorListItem {
  AllocatorListItem *pPrev;
  AllocatorListItem *pNext;
  T Value;
};

// Doubly linked list.
template<typename T>
class AllocatorRawList {
 public:
  typedef AllocatorListItem<T> ItemType;

  AllocatorRawList();
  ~AllocatorRawList();
  void Clear();

  size_t GetCount() const { return m_Count; }
  bool IsEmpty() const { return m_Count == 0; }

  ItemType *Front() { return m_pFront; }
  const ItemType *Front() const { return m_pFront; }
  ItemType *Back() { return m_pBack; }
  const ItemType *Back() const { return m_pBack; }

  ItemType *PushBack();
  ItemType *PushFront();
  ItemType *PushBack(const T& value);
  ItemType *PushFront(const T& value);
  void PopBack();
  void PopFront();

  // Item can be null - it means PushBack.
  ItemType *InsertBefore(ItemType *pItem);
  // Item can be null - it means PushFront.
  ItemType *InsertAfter(ItemType *pItem);

  ItemType *InsertBefore(ItemType *pItem, const T& value);
  ItemType *InsertAfter(ItemType *pItem, const T& value);

  void Remove(ItemType *pItem);

 private:
  AllocatorPoolAllocator<ItemType> m_ItemAllocator;
  ItemType *m_pFront;
  ItemType *m_pBack;
  size_t m_Count;

  // Declared not defined, to block copy constructor and assignment operator.
  AllocatorRawList(const AllocatorRawList<T>& src);
  AllocatorRawList<T>& operator=(const AllocatorRawList<T>& rhs);
};

template<typename T>
AllocatorRawList<T>::AllocatorRawList()
    : m_ItemAllocator(128), m_pFront(nullptr), m_pBack(nullptr), m_Count(0) {
}

template<typename T>
AllocatorRawList<T>::~AllocatorRawList() {
  // Intentionally not calling Clear, because that would be unnecessary
  // computations to return all items to m_ItemAllocator as free.
}

template<typename T>
void AllocatorRawList<T>::Clear() {
  if (IsEmpty() == false) {
    ItemType *pItem = m_pBack;
    while (pItem != nullptr) {
      ItemType *const pPrevItem = pItem->pPrev;
      m_ItemAllocator.Free(pItem);
      pItem = pPrevItem;
    }
    m_pFront = nullptr;
    m_pBack = nullptr;
    m_Count = 0;
  }
}

template<typename T>
AllocatorListItem<T> *AllocatorRawList<T>::PushBack() {
  ItemType *const pNewItem = m_ItemAllocator.Alloc();
  pNewItem->pNext = nullptr;
  if (IsEmpty()) {
    pNewItem->pPrev = nullptr;
    m_pFront = pNewItem;
    m_pBack = pNewItem;
    m_Count = 1;
  } else {
    pNewItem->pPrev = m_pBack;
    m_pBack->pNext = pNewItem;
    m_pBack = pNewItem;
    ++m_Count;
  }
  return pNewItem;
}

template<typename T>
AllocatorListItem<T> *AllocatorRawList<T>::PushFront() {
  ItemType *const pNewItem = m_ItemAllocator.Alloc();
  pNewItem->pPrev = nullptr;
  if (IsEmpty()) {
    pNewItem->pNext = nullptr;
    m_pFront = pNewItem;
    m_pBack = pNewItem;
    m_Count = 1;
  } else {
    pNewItem->pNext = m_pFront;
    m_pFront->pPrev = pNewItem;
    m_pFront = pNewItem;
    ++m_Count;
  }
  return pNewItem;
}

template<typename T>
AllocatorListItem<T> *AllocatorRawList<T>::PushBack(const T& value) {
  ItemType *const pNewItem = PushBack();
  pNewItem->Value = value;
  return pNewItem;
}

template<typename T>
AllocatorListItem<T> *AllocatorRawList<T>::PushFront(const T& value) {
  ItemType *const pNewItem = PushFront();
  pNewItem->Value = value;
  return pNewItem;
}

template<typename T>
void AllocatorRawList<T>::PopBack() {
  RESOURCE_HEAVY_ASSERT(m_Count > 0);
  ItemType *const pBackItem = m_pBack;
  ItemType *const pPrevItem = pBackItem->pPrev;
  if (pPrevItem != nullptr) {
    pPrevItem->pNext = nullptr;
  }
  m_pBack = pPrevItem;
  m_ItemAllocator.Free(pBackItem);
  --m_Count;
}

template<typename T>
void AllocatorRawList<T>::PopFront() {
  RESOURCE_HEAVY_ASSERT(m_Count > 0);
  ItemType *const pFrontItem = m_pFront;
  ItemType *const pNextItem = pFrontItem->pNext;
  if (pNextItem != nullptr) {
    pNextItem->pPrev = nullptr;
  }
  m_pFront = pNextItem;
  m_ItemAllocator.Free(pFrontItem);
  --m_Count;
}

template<typename T>
void AllocatorRawList<T>::Remove(ItemType *pItem) {
  ASSERT(pItem != nullptr);
  ASSERT(m_Count > 0);

  if (pItem->pPrev != nullptr) {
    pItem->pPrev->pNext = pItem->pNext;
  } else {
    ASSERT(m_pFront == pItem);
    m_pFront = pItem->pNext;
  }

  if (pItem->pNext != nullptr) {
    pItem->pNext->pPrev = pItem->pPrev;
  } else {
    ASSERT(m_pBack == pItem);
    m_pBack = pItem->pPrev;
  }

  m_ItemAllocator.Free(pItem);
  --m_Count;
}

template<typename T>
AllocatorListItem<T> *AllocatorRawList<T>::InsertBefore(ItemType *pItem) {
  if (pItem != nullptr) {
    ItemType *const prevItem = pItem->pPrev;
    ItemType *const newItem = m_ItemAllocator.Alloc();
    newItem->pPrev = prevItem;
    newItem->pNext = pItem;
    pItem->pPrev = newItem;
    if (prevItem != nullptr) {
      prevItem->pNext = newItem;
    } else {
      ASSERT(m_pFront == pItem);
      m_pFront = newItem;
    }
    ++m_Count;
    return newItem;
  } else {
    return PushBack();
  }
}

template<typename T>
AllocatorListItem<T> *AllocatorRawList<T>::InsertAfter(ItemType *pItem) {
  if (pItem != nullptr) {
    ItemType *const nextItem = pItem->pNext;
    ItemType *const newItem = m_ItemAllocator.Alloc();
    newItem->pNext = nextItem;
    newItem->pPrev = pItem;
    pItem->pNext = newItem;
    if (nextItem != nullptr) {
      nextItem->pPrev = newItem;
    } else {
      ASSERT(m_pBack == pItem);
      m_pBack = newItem;
    }
    ++m_Count;
    return newItem;
  } else {
    return PushFront();
  }
}

template<typename T>
AllocatorListItem<T> *AllocatorRawList<T>::InsertBefore(ItemType *pItem, const T& value) {
  ItemType *const newItem = InsertBefore(pItem);
  newItem->Value = value;
  return newItem;
}

template<typename T>
AllocatorListItem<T> *AllocatorRawList<T>::InsertAfter(ItemType *pItem, const T& value) {
  ItemType *const newItem = InsertAfter(pItem);
  newItem->Value = value;
  return newItem;
}

template<typename T>
class AllocatorList {
 public:
  // Forward declarations.
  class iterator;
  class const_iterator;

  class iterator {
   public:
    iterator() : m_pList(nullptr), m_pItem(nullptr) {}

    T& operator*() const {
      ASSERT(m_pItem != nullptr);
      return m_pItem->Value;
    }
    T *operator->() const {
      ASSERT(m_pItem != nullptr);
      return &m_pItem->Value;
    }

    iterator& operator++() {
      ASSERT(m_pItem != nullptr);
      m_pItem = m_pItem->pNext;
      return *this;
    }
    iterator& operator--() {
      if (m_pItem != nullptr) {
        m_pItem = m_pItem->pPrev;
      } else {
        ASSERT(!m_pList->IsEmpty());
        m_pItem = m_pList->Back();
      }
      return *this;
    }

    iterator operator++(int) {
      iterator result = *this;
      ++*this;
      return result;
    }
    iterator operator--(int) {
      iterator result = *this;
      --*this;
      return result;
    }

    bool operator==(const iterator& rhs) const {
      ASSERT(m_pList == rhs.m_pList);
      return m_pItem == rhs.m_pItem;
    }
    bool operator!=(const iterator& rhs) const {
      ASSERT(m_pList == rhs.m_pList);
      return m_pItem != rhs.m_pItem;
    }

   private:
    AllocatorRawList<T> *m_pList;
    AllocatorListItem<T> *m_pItem;

    iterator(AllocatorRawList<T> *pList, AllocatorListItem<T> *pItem) : m_pList(pList), m_pItem(pItem) {}

    friend class AllocatorList<T>;
    friend class AllocatorList<T>::const_iterator;
  };

  class const_iterator {
   public:
    const_iterator() : m_pList(nullptr), m_pItem(nullptr) {}

    const_iterator(const iterator& src) : m_pList(src.m_pList), m_pItem(src.m_pItem) {}

    const T& operator*() const {
      ASSERT(m_pItem != nullptr);
      return m_pItem->Value;
    }
    const T *operator->() const {
      ASSERT(m_pItem != nullptr);
      return &m_pItem->Value;
    }

    const_iterator& operator++() {
      ASSERT(m_pItem != nullptr);
      m_pItem = m_pItem->pNext;
      return *this;
    }
    const_iterator& operator--() {
      if (m_pItem != nullptr) {
        m_pItem = m_pItem->pPrev;
      } else {
        ASSERT(!m_pList->IsEmpty());
        m_pItem = m_pList->Back();
      }
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator result = *this;
      ++*this;
      return result;
    }
    const_iterator operator--(int) {
      const_iterator result = *this;
      --*this;
      return result;
    }

    bool operator==(const const_iterator& rhs) const {
      ASSERT(m_pList == rhs.m_pList);
      return m_pItem == rhs.m_pItem;
    }
    bool operator!=(const const_iterator& rhs) const {
      ASSERT(m_pList == rhs.m_pList);
      return m_pItem != rhs.m_pItem;
    }

   private:
    const_iterator(const AllocatorRawList<T> *pList, const AllocatorListItem<T> *pItem)
        : m_pList(pList), m_pItem(pItem) {}

    const AllocatorRawList<T> *m_pList;
    const AllocatorListItem<T> *m_pItem;

    friend class AllocatorList<T>;
  };

  AllocatorList() : m_RawList() {}

  bool empty() const { return m_RawList.IsEmpty(); }
  size_t size() const { return m_RawList.GetCount(); }

  iterator begin() { return iterator(&m_RawList, m_RawList.Front()); }
  iterator end() { return iterator(&m_RawList, nullptr); }

  const_iterator cbegin() const { return const_iterator(&m_RawList, m_RawList.Front()); }
  const_iterator cend() const { return const_iterator(&m_RawList, nullptr); }

  void clear() { m_RawList.Clear(); }
  void push_back(const T& value) { m_RawList.PushBack(value); }
  void erase(iterator it) { m_RawList.Remove(it.m_pItem); }
  iterator insert(iterator it, const T& value) {
    return iterator(&m_RawList,
                    m_RawList.InsertBefore(it.m_pItem, value));
  }

 private:
  AllocatorRawList<T> m_RawList;
};

}

#endif //WYRD_THEFORGE_ALLOCATOR_ALLOCATOR_LIST_HPP
