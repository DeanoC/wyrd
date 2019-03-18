#pragma once
#ifndef WYRD_GUISHELL_WINDOW_H
#define WYRD_GUISHELL_WINDOW_H

#include "core/core.h"
#include "os/rect.h"
#include "math/math.h"

typedef struct{} *GuiShell_IconHandle;

typedef struct GuiShell_WindowDesc {
  char const* name;
  int32_t width;
  int32_t height;
  uint32_t windowsFlags;
  GuiShell_IconHandle bigIcon;
  GuiShell_IconHandle smallIcon;

  bool fullScreen;
  bool iconified;
  bool maximized;
  bool minimized;
  bool visible;

} GuiShell_WindowDesc;

EXTERN_C void GuiShell_WindowGetCurrentDesc(GuiShell_WindowDesc* desc);

#endif //WYRD_GUISHELL_WINDOW_H
