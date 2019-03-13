#pragma once
#ifndef WYRD_OS_ATOMICS_HPP
#define WYRD_OS_ATOMICS_HPP

#include "core/core.h"
#include "os/atomics.h"

namespace Os {
inline uint32_t Os_AtomicCompareAndSwap(volatile uint32_t *pDest, uint32_t swapTo, uint32_t compareWith) {
  return Os_AtomicCompareAndSwap32(pDest, swapTo, compareWith);
}

inline uint64_t AtomicCompareAndSwap(volatile uint64_t *pDest, uint64_t swapTo, uint64_t compareWith) {
  return Os_AtomicCompareAndSwap64(pDest, swapTo, compareWith);
}

inline void *AtomicCompareAndSwapPtr(void *volatile *pDest, void *swapTo, void *compareWith) {
  return Os_AtomicCompareAndSwapPtr(pDest, swapTo, compareWith);
}

inline void *AtomicExchangePtr(void *volatile *pDest, void *swapTo) {
  return Os_AtomicExchangePtr(pDest, swapTo);
}

inline uint32_t AtomicAdd(volatile uint32_t *pDest, uint32_t value) {
  return Os_AtomicAdd32(pDest, value);
}

inline uint64_t AtomicAdd(volatile uint64_t *pDest, uint64_t value) {
  return Os_AtomicAdd64(pDest, value);
}

inline uint64_t AtomicUpdateMax(volatile uint64_t *pDest, uint64_t value) {
  return Os_AtomicUpdateMax(pDest, value);
}

}

#endif //WYRD_OS_ATOMICS_HPP
