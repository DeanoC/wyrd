#pragma once
#ifndef WYRD_GUISHELL_PLATFORM_H
#define WYRD_GUISHELL_PLATFORM_H

#include "core/core.h"

// include this file only in platform specific areas
// it probably won't compile due to things like obj-c in apple
// etc. unless your expecting it.
#if PLATFORM == PLATFORM_APPLE_MAC
typedef struct GuiShell_AppleWindow {
  GuiShell_WindowDesc desc;
  MTKView *_Nonnull metalView;
  float retinaScale[2];
} GuiShell_AppleWindow;
#elif PLATFORM == PLATFORM_WINDOWS
typedef struct GuiShell_Win32Window {
  GuiShell_WindowDesc desc;
  HWND hwnd;
} GuiShell_Win32Window;
#endif


#endif //WYRD_PLATFORM_H
