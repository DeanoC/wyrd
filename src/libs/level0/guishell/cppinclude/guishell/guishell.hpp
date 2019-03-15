#pragma once
#ifndef WYRD_GUISHELL_GUISHELL_HPP
#define WYRD_GUISHELL_GUISHELL_HPP

#include "core/core.h"
#include "guishell/guishell.h"
#include "tinystl/string.h"
#include "os/window.h"

namespace GuiShell {

class IApp {
 public:
  virtual bool Init() = 0;
  virtual void Exit() = 0;

  virtual bool Load() = 0;
  virtual void Unload() = 0;

  virtual void Update(float deltaTime) = 0;
  virtual void Draw() = 0;

  virtual tinystl::string GetName() = 0;

  struct Settings {
    /// Window width
    int32_t mWidth = -1;
    /// Window height
    int32_t mHeight = -1;
    /// Set to true if fullscreen mode has been requested
    bool mFullScreen = false;
    /// Set to true if app wants to use an external window
    bool mExternalWindow = false;
#if defined(TARGET_IOS)
    bool mShowStatusBar = false;
    float mContentScaleFactor = 0.f;
#endif
  } mSettings;

  Os_WindowsDesc *pWindow;
  tinystl::string mCommandLine;
};
} // end namespae GuiShell

#endif //WYRD_GUISHELL_GUISHELL_HPP
