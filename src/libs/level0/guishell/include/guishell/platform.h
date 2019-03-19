#pragma once
#ifndef WYRD_GUISHELL_PLATFORM_H
#define WYRD_GUISHELL_PLATFORM_H

#include "core/core.h"

// include this file only in platform specific areas
// it probably won't compile due to things like obj-c in apple
// etc. unless your expecting it.
#if PLATFORM == PLATFORM_APPLE_MAC

struct GuiShell_AppleWindow {
  GuiShell_WindowDesc desc;
  MTKView *_Nonnull metalView;
  float retinaScale[2];
};

#endif


#endif //WYRD_PLATFORM_H
