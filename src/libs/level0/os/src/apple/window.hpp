#pragma once
#ifndef WYRD_OS_APPLE_WINDOW_HPP
#define WYRD_OS_APPLE_WINDOW_HPP

#include "core/core.h"
#include "os/window.h"

namespace Os {

struct AppleWindow {
  Os_WindowDesc desc;
  Os_RectDesc_t windowedRect;
  MTKView *_Nonnull metalView;
};

} // end namespace Os


#endif //WYRD_OS_APPLE_WINDOW_HPP
