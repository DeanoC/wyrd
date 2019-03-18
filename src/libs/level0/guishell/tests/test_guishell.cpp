
#include "guishell/guishell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

namespace {
bool Init() {
  return true;
}

bool Load() {
  return true;
}

void Update(double deltaTimeMS) {
  static bool testsHaveRun = false;
  if(testsHaveRun == false ) {
    char const* argv[] = { "Guishell_tests" };
    Catch::Session().run(sizeof(argv)/sizeof(argv[0]), (char**) argv);
    testsHaveRun = true;
  }
}

void Draw() {

}

void Unload() {

}

void Exit() {
}

void Abort() {
  abort();
}

} // end anon namespace

EXTERN_C void GuiShell_AppConfig(GuiShell_Functions* functions, GuiShell_WindowDesc* initialWindow)
{
  functions->init = &Init;
  functions->load = &Load;
  functions->unload = &Unload;
  functions->update = &Update;
  functions->draw = &Draw;
  functions->exit = &Exit;
  functions->abort = &Abort;

  initialWindow->width = -1;
  initialWindow->height = -1;
  initialWindow->fullScreen = false;
  initialWindow->windowsFlags = 0;
  initialWindow->visible = true;
  initialWindow->fullScreen = false;
}

