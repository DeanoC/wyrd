
#include "guishell/guishell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

EXTERN_C bool GuiShell_Init() {
  return true;
}

EXTERN_C bool GuiShell_Load() {
  char const* argv[] = { "Guishell_tests" };
  return Catch::Session().run(sizeof(argv)/sizeof(argv[0]), (char**) argv) == 0;
}

EXTERN_C void GuiShell_Update(double deltaTimeMS) {

}

EXTERN_C void GuiShell_Draw() {

}

EXTERN_C void GuiShell_Unload() {

}

EXTERN_C void GuiShell_Exit() {
}

EXTERN_C void GuiShell_Terminate() {
  abort();
}

