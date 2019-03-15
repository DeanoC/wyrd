#pragma once
#ifndef WYRD_OS_WINDOWSDESC_H
#define WYRD_OS_WINDOWSDESC_H

#include "core/core.h"
#include "os/rectdesc.h"
#include "math/math.h"
typedef struct{} *Os_IconHandle;
typedef struct{} *Os_WindowHandle;

typedef struct Os_WindowDesc {
  Os_RectDesc_t rect;
  bool fullScreen;
  unsigned windowsFlags;
  Os_IconHandle bigIcon;
  Os_IconHandle smallIcon;

  bool cursorTracked;
  bool iconified;
  bool maximized;
  bool minimized;
  bool visible;

} Os_WindowsDesc;

// Window handling
EXTERN_C void Os_OpenWindow(const char *app_name, Os_WindowsDesc const *winDesc, Os_WindowHandle* handle);
EXTERN_C void Os_CloseWindow(Os_WindowHandle handle);
EXTERN_C void Os_HandleMessages(Os_WindowHandle handle);
EXTERN_C void Os_SetWindowSize(Os_WindowHandle handle, unsigned width, unsigned height);
EXTERN_C void Os_ToggleFullscreen(Os_WindowHandle handle);
EXTERN_C void Os_ShowWindow(Os_WindowHandle handle);
EXTERN_C void Os_HideWindow(Os_WindowHandle handle);
EXTERN_C void Os_MaximizeWindow(Os_WindowHandle handle);
EXTERN_C void Os_MinimizeWindow(Os_WindowHandle handle);

EXTERN_C void Os_SetMousePositionRelative(Os_WindowHandle handle, int32_t x, int32_t y);

// Input handling
EXTERN_C vec2_t Os_GetMousePosition(Os_WindowHandle handle);
EXTERN_C bool Os_GetKeyDown(Os_WindowHandle handle, int key);
EXTERN_C bool Os_GetKeyUp(Os_WindowHandle handle, int key);
EXTERN_C bool Os_GetJoystickButtonDown(Os_WindowHandle handle, int button);
EXTERN_C bool Os_GetJoystickButtonUp(Os_WindowHandle handle, int button);

EXTERN_C void Os_SetWindowRect(Os_WindowHandle handle, const Os_RectDesc_t* rect);

#endif //WYRD_OS_WINDOWSDESC_H
