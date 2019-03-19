#pragma once
#ifndef WYRD_GUISHELL_GUISHELL_H
#define WYRD_GUISHELL_GUISHELL_H
#include "core/core.h"
#include "guishell/window.h"

typedef bool (*GuiShell_InitFunc)();
typedef bool (*GuiShell_LoadFunc)();
typedef void (*GuiShell_UnloadFunc)();
typedef void (*GuiShell_UpdateFunc)(double deltaTimeMS);
typedef void (*GuiShell_DrawFunc)();
typedef void (*GuiShell_ExitFunc)();
typedef void (*GuiShell_AbortFunc)();

// the shell will call these function if not null at various times
// in the lifetime of the app
typedef struct GuiShell_Functions {
  GuiShell_InitFunc init;
  GuiShell_LoadFunc load;
  GuiShell_UnloadFunc unload;

  GuiShell_UpdateFunc update;
  GuiShell_DrawFunc draw;

  GuiShell_ExitFunc exit;
  GuiShell_AbortFunc abort;
} GuiShell_Functions;

// an app must provide this function
/// fill in the initial window config and functions
EXTERN_C void GuiShell_AppConfig(GuiShell_Functions* functions, GuiShell_WindowDesc* initialWindow);

// call these to tell the shell various things you want to change
EXTERN_C void GuiShell_Terminate();

EXTERN_C int Main( int argc_, char* argv[]);

#define DEFINE_APPLICATION_MAIN \
int main(int argc, char* argv[])					\
{													\
	return Main(argc, argv);					    \
}

#endif //WYRD_GUISHELL_GUISHELL_HPP
