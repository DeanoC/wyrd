
#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

/* C based
#include "guishell/guishell.h"

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
    GuiShell_Terminate();
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

*/
#include "guishell/guishell.hpp"

struct CppGuiShellTest {

  static bool Init(){return true;}
  static void Exit(){}

  static bool Load(){return true;};
  static void Unload(){};

  static void Update(double deltaTime){
    static bool testsHaveRun = false;
    if(testsHaveRun == false ) {
      char const* argv[] = { "Guishell_tests" };
      Catch::Session().run(sizeof(argv)/sizeof(argv[0]), (char**) argv);
      testsHaveRun = true;
      GuiShell::Terminate();
    }
  }
  static void Draw(){};

  static GuiShell::Window::Desc const* InitialWindow() {
    static GuiShell::Window::Desc const initialWindow {
        "GuiShell CPP tests", // name
        -1, -1, // width and height
        0, // flags
        nullptr, // big icon
        nullptr, // small icon

        false, // fulscreen
        false, // iconified
        false, // maximized
        false, // minimized,
        true // visible
    };
    return &initialWindow;
  }

};

DECLARE_APP(CppGuiShellTest)

TEST_CASE("Initial Height/Width", "[GuiShell]") {

  GuiShell_WindowDesc desc;
  GuiShell_WindowGetCurrentDesc(&desc);
  REQUIRE(desc.width == 1920);
  REQUIRE(desc.height == 1080);
}

