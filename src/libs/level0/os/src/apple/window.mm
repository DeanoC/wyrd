#import <Foundation/Foundation.h>
#include <AppKit/NSOpenPanel.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include "core/core.h"
#include "core/logger.h"
#include "os/window.h"
#include "window.hpp"


void Os_OpenWindow(const char *app_name, Os_WindowDesc const *winDesc, Os_WindowHandle* handle) {
  Os::AppleWindow* window = (Os::AppleWindow*) calloc(1, sizeof(Os::AppleWindow));
  memcpy(&window->desc, winDesc, sizeof(Os_WindowDesc));

  *handle = *((Os_WindowHandle*)window);
}

void Os_CloseWindow(Os_WindowHandle handle) {
  free(handle);
}
void Os_HandleMessages(Os_WindowHandle handle) {

}
void Os_SetWindowSize(Os_WindowHandle handle, unsigned width, unsigned height) {

}
void Os_ToggleFullscreen(Os_WindowHandle handle) {

}
void Os_ShowWindow(Os_WindowHandle handle) {

}
void Os_HideWindow(Os_WindowHandle handle) {

}
void Os_MaximizeWindow(Os_WindowHandle handle) {

}
void Os_MinimizeWindow(Os_WindowHandle handle) {

}
void Os_SetMousePositionRelative(Os_WindowHandle handle, int32_t x, int32_t y) {

}

// Input handling
vec2_t Os_GetMousePosition(Os_WindowHandle handle) {

}
bool Os_GetKeyDown(Os_WindowHandle handle, int key) {

}
bool Os_GetKeyUp(Os_WindowHandle handle, int key) {

}
bool Os_GetJoystickButtonDown(Os_WindowHandle handle, int button) {

}
bool Os_GetJoystickButtonUp(Os_WindowHandle handle, int button) {

}

void Os_SetWindowRect(Os_WindowHandle handle, const Os_RectDesc_t* rect) {

}
