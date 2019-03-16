#pragma once
#ifndef WYRD_GUISHELL_GUISHELL_H
#define WYRD_GUISHELL_GUISHELL_H
#include "core/core.h"

// an app must provide these functions
EXTERN_C bool GuiShell_Init();
EXTERN_C bool GuiShell_Load();
EXTERN_C void GuiShell_Update(double deltaTimeMS);
EXTERN_C void GuiShell_Draw();
EXTERN_C void GuiShell_Unload();
EXTERN_C void GuiShell_Exit();
EXTERN_C void GuiShell_Terminate();


#endif //WYRD_GUISHELL_GUISHELL_HPP
