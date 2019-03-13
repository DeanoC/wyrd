#pragma once
#ifndef WYRD_OS_WINDOWSDESC_H
#define WYRD_OS_WINDOWSDESC_H

#include "core/core.h"
#include "os/rectdesc.h"
#include "math/math.h"

typedef void *Os_IconHandle_t;
typedef void *Os_WindowHandle_t;

typedef struct Os_WindowsDesc_t {
  // TODO linux should allocate a WindowsDescExtraXXX and place in handle
  Os_WindowHandle_t handle; //hWnd
  Os_RectDesc_t windowedRect;
  Os_RectDesc_t fullscreenRect;
  Os_RectDesc_t clientRect;
  bool fullScreen;
  unsigned windowsFlags;
  Os_IconHandle_t bigIcon;
  Os_IconHandle_t smallIcon;

  bool cursorTracked;
  bool iconified;
  bool maximized;
  bool minimized;
  bool visible;

  // maybe that should go to the input system?
  // The last received cursor position, regardless of source
  int lastCursorPosX, lastCursorPosY;
} Os_WindowsDesc_t;

// Window handling
void openWindow(const char *app_name, Os_WindowsDesc_t *winDesc);
void closeWindow(const Os_WindowsDesc_t *winDesc);
void handleMessages();
void setWindowSize(Os_WindowsDesc_t *winDesc, unsigned width, unsigned height);
void toggleFullscreen(Os_WindowsDesc_t *winDesc);
void showWindow(Os_WindowsDesc_t *winDesc);
void hideWindow(Os_WindowsDesc_t *winDesc);
void maximizeWindow(Os_WindowsDesc_t *winDesc);
void minimizeWindow(Os_WindowsDesc_t *winDesc);

void setMousePositionRelative(const Os_WindowsDesc_t *winDesc, int32_t x, int32_t y);

// Input handling
vec2_t getMousePosition();
bool getKeyDown(int key);
bool getKeyUp(int key);
bool getJoystickButtonDown(int button);
bool getJoystickButtonUp(int button);

#ifdef __cplusplus
//void setWindowRect(WindowsDesc* winDesc, const RectDesc& rect);
#endif

#endif //WYRD_OS_WINDOWSDESC_H
