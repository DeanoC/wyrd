#pragma once
#ifndef WYRD_OS_RECTDESC_H
#define WYRD_OS_RECTDESC_H

#include "core/core.h"

typedef struct Os_Rect {
  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;
} Os_Rect;

EXTERN_C inline int32_t Os_GetRectWidth(const Os_Rect *rect) {
  return rect->right - rect->left;
}

EXTERN_C inline int32_t Os_GetRectHeight(const Os_Rect *rect) {
  return rect->bottom - rect->top;
}

#endif //WYRD_OS_RECTDESC_H
