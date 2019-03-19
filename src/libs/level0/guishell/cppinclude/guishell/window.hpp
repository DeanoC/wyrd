#pragma once
#ifndef WYRD_GUISHELL_WINDOW_HPP
#define WYRD_GUISHELL_WINDOW_HPP

#include "core/core.h"
#include "guishell/window.h"
#include "os/rect.hpp"

namespace GuiShell {
namespace Window {

using Desc = GuiShell_WindowDesc;

inline void GetCurrentDesc(Desc *desc) {
  GuiShell_WindowGetCurrentDesc(desc);
}

inline void* GetPlatformWindowPtr() {
  return GuiShell_GetPlatformWindowPtr();
}

} } // end namespace GuiShell::Window

#endif //WYRD_GUISHELL_WINDOW_HPP
