#pragma once
#ifndef WYRD_GUISHELL_PLATFORM_HPP
#define WYRD_GUISHELL_PLATFORM_HPP

#include "core/core.h"
#include "guishell/window.h"
#include "guishell/platform.h"

namespace GuiShell {
#if PLATFORM == PLATFORM_APPLE_MAC
using AppleWindow = GuiShell_AppleWindow;
#endif

} // end namespace

#endif //WYRD_PLATFORM_HPP
