#pragma once
#ifndef WYRD_GUISHELL_WINDOW_HPP
#define WYRD_GUISHELL_WINDOW_HPP

#include "core/core.h"
#include "guishell/window.h"
#include "os/rect.hpp"

namespace GuiShell {
namespace Window {

using Handle = GuiShell_WindowHandle;
using Desc = GuiShell_WindowDesc;

void GetCurrentDesc(Handle handle, Desc *desc) {
  GuiShell_WindowGetCurrentDesc(handle, desc);
}

} } // end namespace GuiShell::Window

#endif //WYRD_GUISHELL_WINDOW_HPP
