#pragma once
#ifndef WYRD_OS_RECT_HPP
#define WYRD_OS_RECT_HPP

#include "core/core.h"
#include "os/rect.h"

namespace Os {

using Rect = Os_Rect;

inline int32_t Width(const Rect& rect) {
  return rect.right - rect.left;
}

inline int32_t Height(const Rect& rect) {
  return rect.bottom - rect.top;
}

inline int32_t Width(const Rect* rect) {
  return rect->right - rect->left;
}

inline int32_t Height(const Rect* rect) {
  return rect->bottom - rect->top;
}

} // end namespace Os

#endif //WYRD_RECT_HPP
