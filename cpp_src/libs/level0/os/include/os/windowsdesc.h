#pragma once
#ifndef WYRD_OS_WINDOWSDESC_H
#define WYRD_OS_WINDOWSDESC_H

#include "core/core.h"
#include "os/rectdesc.h"
#include "math/math.h"

typedef void* IconHandle;
typedef void* WindowHandle;

typedef struct WindowsDesc_t
{
	// TODO linux should allocate a WindowsDescExtraXXX and place in handle
	WindowHandle handle; //hWnd
	RectDesc windowedRect;
	RectDesc fullscreenRect;
	RectDesc clientRect;
	bool fullScreen;
	unsigned windowsFlags;
	IconHandle bigIcon;
	IconHandle smallIcon;

	bool cursorTracked;
	bool iconified;
	bool maximized;
	bool minimized;
	bool visible;

	// maybe that should go to the input system?
	// The last received cursor position, regardless of source
	int lastCursorPosX, lastCursorPosY;
} WindowsDesc_t;

#ifdef __cplusplus
struct WindowsDesc : public WindowsDesc_t
{
	WindowsDesc() : WindowsDesc_t{
		nullptr, {}, {}, {},
		false, 0, nullptr, nullptr, false,
		false, false, false, false, true
		} {}

};
#endif

// Window handling
void openWindow(const char* app_name, WindowsDesc_t* winDesc);
void closeWindow(const WindowsDesc_t* winDesc);
void handleMessages();
void setWindowSize(WindowsDesc_t* winDesc, unsigned width, unsigned height);
void toggleFullscreen(WindowsDesc_t* winDesc);
void showWindow(WindowsDesc_t* winDesc);
void hideWindow(WindowsDesc_t* winDesc);
void maximizeWindow(WindowsDesc_t* winDesc);
void minimizeWindow(WindowsDesc_t* winDesc);

void setMousePositionRelative(const WindowsDesc_t* winDesc, int32_t x, int32_t y);

// Input handling
vec2 getMousePosition();
bool getKeyDown(int key);
bool getKeyUp(int key);
bool getJoystickButtonDown(int button);
bool getJoystickButtonUp(int button);

#ifdef __cplusplus
//void setWindowRect(WindowsDesc* winDesc, const RectDesc& rect);
#endif

#endif //WYRD_OS_WINDOWSDESC_H
