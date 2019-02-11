#pragma once
#ifndef WYRD_UNIX_OS_COMMON_H
#define WYRD_UNIX_OS_COMMON_H


#define VK_USE_PLATFORM_XLIB_KHR

#if defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_XCB_KHR)
#include <X11/Xutil.h>
#endif

#if defined(VK_USE_PLATFORM_XLIB_KHR)
typedef struct WindowsDescXlibExtra {
	Display *display;
	Window xlib_window;
	Atom xlib_wm_delete_window;
} WindowsDescXlibExtra;

#elif defined(VK_USE_PLATFORM_XCB_KHR)
typedef struct WindowsDescXcbExtra {
	Display *display;
	xcb_connection_t *connection;
	xcb_screen_t *screen;
	xcb_window_t xcb_window;
	xcb_intern_atom_reply_t *atom_wm_delete_window;
} WindowsDescXxbExtra;
#endif

#define CALLTYPE
#define ThreadID pthread_t

#endif //WYRD_COMMON_H
