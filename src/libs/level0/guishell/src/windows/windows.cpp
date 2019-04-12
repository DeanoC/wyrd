#include "core/core.h"
#include "core/windows.h"
#include "guishell/guishell.hpp"
#include "guishell/platform.h"
#include "tinystl/vector.h"
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <ios>

GuiShell_Functions gGuiShellFunctions = {};

struct WindowsSpecific {
  void createStandardArgs(LPSTR command_line);
  void getMessages(void);
  void ensureConsoleWindowsExists();
  bool registerClass(GuiShell_WindowDesc desc);
  uint32_t createWindow(GuiShell_WindowDesc desc);
  void destroyWindow(uint32_t index);

  HINSTANCE hInstance;
  HINSTANCE hPrevInstance;
  int nCmdShow;

  tinystl::vector<GuiShell_Win32Window> windows;

  static const int MAX_CMDLINE_ARGS = 1024;

  int argc;
  char *argv[MAX_CMDLINE_ARGS];
  char moduleFilename[MAX_PATH];
  bool windowsQuit = false;
} gWindowsSpecific;

namespace {
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_DESTROY:gWindowsSpecific.windowsQuit = true;
      PostQuitMessage(0);
      break;
    case WM_PAINT:ValidateRect(hWnd, NULL);
      break;
    default:return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
}
void WindowsSpecific::createStandardArgs(LPSTR command_line) {
  char *arg;
  int index;

  // count the arguments
  argc = 1;
  arg = command_line;

  while (arg[0] != 0) {
    while (arg[0] != 0 && arg[0] == ' ') {
      arg++;
    }

    if (arg[0] != 0) {
      argc++;
      while (arg[0] != 0 && arg[0] != ' ') {
        arg++;
      }
    }
  }

  if (argc > MAX_CMDLINE_ARGS) {
    argc = MAX_CMDLINE_ARGS;
  }

  // tokenize the arguments
  arg = command_line;
  index = 1;

  while (arg[0] != 0) {
    while (arg[0] != 0 && arg[0] == ' ') {
      arg++;
    }

    if (arg[0] != 0) {
      argv[index] = arg;
      index++;

      while (arg[0] != 0 && arg[0] != ' ') {
        arg++;
      }

      if (arg[0] != 0) {
        arg[0] = 0;
        arg++;
      }
    }
  }

  // put the program name into argv[0]
  argv[0] = moduleFilename;
}

//! Pumps windows messages
void WindowsSpecific::getMessages(void) {
  if (windows.empty()) {
    MSG Message;
    while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&Message);
      DispatchMessage(&Message);
    }
  } else {
    for (auto window : windows) {
      MSG Message;
      while (PeekMessage(&Message, window.hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
      }
    }
  }
}
void WindowsSpecific::ensureConsoleWindowsExists() {
  // maximum mumber of lines the output console should have
  static const WORD MAX_CONSOLE_LINES = 500;

  using namespace std;
  int hConHandle;
  HANDLE lStdHandle;

  CONSOLE_SCREEN_BUFFER_INFO coninfo;
  FILE *fp;

  // allocate a console for this app
  AllocConsole();

  // set the screen buffer to be big enough to let us scroll text
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);

  coninfo.dwSize.Y = MAX_CONSOLE_LINES;

  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

  // redirect unbuffered STDOUT to the console
  lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  hConHandle = _open_osfhandle((intptr_t) lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stdout = *fp;
  setvbuf(stdout, NULL, _IONBF, 0);

  // redirect unbuffered STDIN to the console
  lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle((intptr_t) lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "r");
  *stdin = *fp;
  setvbuf(stdin, NULL, _IONBF, 0);

  // redirect unbuffered STDERR to the console
  lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle((intptr_t) lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stderr = *fp;
  setvbuf(stderr, NULL, _IONBF, 0);


  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog

  // point to console as well
  ios::sync_with_stdio();
}

bool WindowsSpecific::registerClass(GuiShell_WindowDesc desc) {
  // Register class
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = (HICON) desc.bigIcon;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = desc.name;
  wcex.hIconSm = (HICON) desc.smallIcon;
  if (!RegisterClassEx(&wcex)) {
    return false;
  }

  return true;
}

uint32_t WindowsSpecific::createWindow(GuiShell_WindowDesc desc) {
  // Create window
  RECT rc = {0, 0, (LONG) desc.width, (LONG) desc.height};
  DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
  DWORD styleEx = 0;
  if (desc.fullScreen == false) {
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
  }
  if (desc.width == -1 || desc.height == -1) {
    desc.width = 1920;
    desc.height = 1080;
  }
  HWND hwnd = CreateWindowEx(styleEx,
                             desc.name,
                             desc.name,
                             style,
                             0, 0, rc.right - rc.left, rc.bottom - rc.top,
                             nullptr,
                             nullptr,
                             gWindowsSpecific.hInstance,
                             nullptr);
  if (!hwnd) { return ~0u; }
  gWindowsSpecific.getMessages();
  ShowWindow(hwnd, gWindowsSpecific.nCmdShow);
  gWindowsSpecific.getMessages();
  gWindowsSpecific.windows.emplace_back(GuiShell_Win32Window{desc, hwnd});

  return (uint32_t) gWindowsSpecific.windows.size() - 1;
}

void WindowsSpecific::destroyWindow(uint32_t index) {
  if (index == ~0u) { return; }
  if (index >= gWindowsSpecific.windows.size()) { return; }

  if (gWindowsSpecific.windows[index].hwnd == nullptr) { return; }

  DestroyWindow(gWindowsSpecific.windows[index].hwnd);
  gWindowsSpecific.windows[index].hwnd = nullptr;
}

int Main(int argc, char *argv[]) {
  bool okay = true;

  GuiShell_WindowDesc desc{};
  GuiShell_AppConfig(&gGuiShellFunctions, &desc);

  gWindowsSpecific.registerClass(desc);
  uint32_t mainWindowIndex = gWindowsSpecific.createWindow(desc);
  ASSERT(mainWindowIndex == 0);

  if (gGuiShellFunctions.init && !gGuiShellFunctions.init()) {
    if (gGuiShellFunctions.abort) {
      gGuiShellFunctions.abort();
    } else {
      gWindowsSpecific.windowsQuit = false;
    }
    okay = false;
  }
  if (gGuiShellFunctions.load && !gGuiShellFunctions.load()) {
    if (gGuiShellFunctions.abort) {
      gGuiShellFunctions.abort();
    } else {
      gWindowsSpecific.windowsQuit = false;
    }
    okay = false;
  }

  while (gWindowsSpecific.windowsQuit == false) {
    // TODO timing
    double deltaTimeMS = 18.0;

    gWindowsSpecific.getMessages();
    if (gGuiShellFunctions.update) {
      gGuiShellFunctions.update(deltaTimeMS);
    }
    if (gGuiShellFunctions.draw) {
      gGuiShellFunctions.draw();
    }
  }

  gWindowsSpecific.destroyWindow(mainWindowIndex);

  if (gGuiShellFunctions.unload) {
    gGuiShellFunctions.unload();
  }

  if (gGuiShellFunctions.exit) {
    gGuiShellFunctions.exit();
  }

  return okay ? 0 : 10;
}

EXTERN_C int LibWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  gWindowsSpecific.hInstance = hInstance;
  gWindowsSpecific.hPrevInstance = hPrevInstance;
  gWindowsSpecific.nCmdShow = nCmdShow;

  GetModuleFileNameA(NULL, gWindowsSpecific.moduleFilename, MAX_PATH);
  gWindowsSpecific.createStandardArgs(lpCmdLine);
//  gWindowsSpecific.ensureConsoleWindowsExists();

  return Main(gWindowsSpecific.argc, gWindowsSpecific.argv);
}

// GuiShell Window API
EXTERN_C void GuiShell_WindowGetCurrentDesc(GuiShell_WindowDesc *desc) {
  ASSERT(desc);
  memcpy(desc, &gWindowsSpecific.windows[0].desc, sizeof(GuiShell_WindowDesc));
}

EXTERN_C void GuiShell_Terminate() {
  gWindowsSpecific.windowsQuit = true;
}

EXTERN_C void *GuiShell_GetPlatformWindowPtr() {
  return &gWindowsSpecific.windows[0];
}