#pragma once
#ifndef WYRD_GUISHELL_GUISHELL_HPP
#define WYRD_GUISHELL_GUISHELL_HPP

#include "core/core.h"
#include "guishell/guishell.h"
#include "guishell/window.hpp"

// Init, Load, Unload, Update, Draw and Exit and InitialWindow must be defined
// as static function in the users app class
#define DECLARE_APP(type) static type theApp; \
DEFINE_APPLICATION_MAIN \
EXTERN_C void GuiShell_AppConfig(GuiShell_Functions* functions, GuiShell_WindowDesc* initialWindow) { \
  functions->init = &theApp.Init; \
  functions->load = &theApp.Load; \
  functions->unload = &theApp.Unload; \
  functions->update = &theApp.Update; \
  functions->draw = &theApp.Draw; \
  functions->exit = &theApp.Exit; \
  memcpy(initialWindow, theApp.InitialWindow(), sizeof(GuiShell_WindowDesc)); \
}

namespace GuiShell {

inline void Terminate() {
  GuiShell_Terminate();
}

} // end namespace GuiShell

#endif //WYRD_GUISHELL_GUISHELL_HPP
