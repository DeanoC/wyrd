#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <AppKit/NSOpenPanel.h>
#include <AppKit/AppKit.h>

#include "core/core.h"
#include "core/logger.h"
#include "os/filesystem.hpp"
#include "guishell/guishell.hpp"
#include "guishell/window.hpp"
#include "macapp.hpp"

namespace {

GuiShell_Functions gGuiShellFunctions = {};
GuiShell::Window::Desc gInitialMainWindowDesc = {};
AppleWindow gMainWindowHandle = {};

}

int main(int argc, char const* argv[])
{
  GuiShell_AppConfig(&gGuiShellFunctions, &gInitialMainWindowDesc);
  return NSApplicationMain(argc, argv);
}

/************************************************************************/
// GameViewController implementation
/************************************************************************/

@implementation GameViewController {
  MTKView *_view;
  id <MTLDevice> _device;
  MetalKitApplication *_application;
}

- (void)viewDidLoad {
  [super viewDidLoad];

  // Set the view to use the default device
  _device = MTLCreateSystemDefaultDevice();
  _view = (MTKView *) self.view;
  _view.delegate = self;
  _view.device = _device;
  _view.paused = NO;
  _view.enableSetNeedsDisplay = NO;
  _view.preferredFramesPerSecond = 60.0;
  [_view.window makeFirstResponder:self];
  _view.autoresizesSubviews = YES;

  // Adjust window size to match retina scaling.
  float retinaScale[2] = {(float) (_view.drawableSize.width / _view.frame.size.width),
                          (float) (_view.drawableSize.height / _view.frame.size.height)};
  NSSize windowSize = CGSizeMake(_view.frame.size.width / retinaScale[0], _view.frame.size.height / retinaScale[1]);
  [_view.window setContentSize:windowSize];
  [_view.window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];

  if (!_device) {
    NSLog(@"Metal is not supported on this device");
    self.view = [[NSView alloc] initWithFrame:self.view.frame];
  }

  // Kick-off the MetalKitApplication.
  _application = [[MetalKitApplication alloc]
      initWithMetalDevice:_device
renderDestinationProvider:self
                     view:_view];


  //register terminate callback
  NSApplication *app = [NSApplication sharedApplication];
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(applicationWillTerminate:)
                                               name:NSApplicationWillTerminateNotification
                                             object:app];
}

/*A notification named NSApplicationWillTerminateNotification.*/
- (void)applicationWillTerminate:(NSNotification *)notification {
  [_application shutdown];
}

- (BOOL)acceptsFirstResponder {
  return TRUE;
}

- (BOOL)canBecomeKeyView {
  return TRUE;
}

// Called whenever view changes orientation or layout is changed
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  view.window.contentView = _view;
  [_application drawRectResized:view.bounds.size];
}

// Called whenever the view needs to render
- (void)drawInMTKView:(nonnull MTKView *)view {
  @autoreleasepool {
    [_application update];
    [_application updateInput];
    //this is needed for NON Vsync mode.
    //This enables to force update the display
    if (_view.enableSetNeedsDisplay == YES) {
      [_view setNeedsDisplay:YES];
    }
  }
}

@end

/************************************************************************/
// MetalKitApplication implementation
/************************************************************************/

// Metal application implementation.
@implementation MetalKitApplication {
}

- (nonnull instancetype)initWithMetalDevice:(nonnull id <MTLDevice>)device
                  renderDestinationProvider:(nonnull id <RenderDestinationProvider>)renderDestinationProvider
                                       view:(nonnull MTKView *)view {
  self = [super init];

  if (self) {
    Os::FileSystem::SetCurrentDir(Os::FileSystem::GetExePath());

    int32_t width = gInitialMainWindowDesc.width;
    int32_t height = gInitialMainWindowDesc.height;

    //    window->metalView = (MTKView*) CFBridgingRetain(view);
    if (width == -1 || height == -1)
    {
      gInitialMainWindowDesc.width = width = 1920;
      gInitialMainWindowDesc.height = height = 1080;
    }
    else
    {
      //if width and height were set manually in App constructor
      //then override and set window size to user width/height.
      //That means we now render at size * gRetinaScale.
      //TODO: make sure pSettings->mWidth determines window size and not drawable size as on retina displays we need to make sure that's what user wants.
      NSSize windowSize = CGSizeMake(width, height);
      [view.window setContentSize:windowSize];
      [view setFrameSize:windowSize];
    }

    @autoreleasepool {
      //if init fails then exit the app
      if (gGuiShellFunctions.init && !gGuiShellFunctions.init()) {
        for (NSWindow *window in [NSApplication sharedApplication].windows) {
          [window close];
        }

        if(gGuiShellFunctions.abort)
        {
          gGuiShellFunctions.abort();
        } else {
          abort();
        }
      }

      //if load fails then exit the app
      if (gGuiShellFunctions.load && !gGuiShellFunctions.load()) {
        for (NSWindow *window in [NSApplication sharedApplication].windows) {
          [window close];
        }

        if(gGuiShellFunctions.abort)
        {
          gGuiShellFunctions.abort();
        } else {
          abort();
        }
      }
    }
  }

  return self;
}

- (void)drawRectResized:(CGSize)size {
  // TODO deano
//  float newWidth = size.width * gRetinaScale.x;
//  float newHeight = size.height * gRetinaScale.y;
  float newWidth = (float)size.width;
  float newHeight = (float)size.height;
  if (newWidth != gMainWindowHandle.desc.width ||
      newHeight != gMainWindowHandle.desc.height) {
    gMainWindowHandle.desc.width = (uint32_t)newWidth;
    gMainWindowHandle.desc.height = (uint32_t)newHeight;

    if (gGuiShellFunctions.unload) {
      gGuiShellFunctions.unload();
    }
    if (gGuiShellFunctions.load) {
      gGuiShellFunctions.load();
    }
  }

  if (gGuiShellFunctions.unload) {
    gGuiShellFunctions.unload();
  }
  if (gGuiShellFunctions.load) {
    gGuiShellFunctions.load();
  }
}

- (void)updateInput {
}

- (void)update {
  // TODO Deano
  float deltaTimeMS = 33.0f;//deltaTimer.GetMSec(true) / 1000.0f;
  // if framerate appears to drop below about 6, assume we're at a breakpoint and simulate 20fps.
  if (deltaTimeMS > 0.15f) {
    deltaTimeMS = 0.05f;
  }

  if (gGuiShellFunctions.update) {
    gGuiShellFunctions.update(deltaTimeMS);
  }
  if (gGuiShellFunctions.draw) {
    gGuiShellFunctions.draw();
  }

#ifdef AUTOMATED_TESTING
  testingCurrentFrameCount++;
    if (testingCurrentFrameCount >= testingMaxFrameCount)
    {
        for (NSWindow* window in [NSApplication sharedApplication].windows)
        {
            [window close];
        }

        [NSApp terminate:nil];
    }
#endif
}

- (void)shutdown {
//  InputSystem::Shutdown();
  if (gGuiShellFunctions.unload) {
    gGuiShellFunctions.unload();
  }

  if (gGuiShellFunctions.exit) {
    gGuiShellFunctions.exit();
  }
}
@end

// GuiShell Window API
EXTERN_C void GuiShell_WindowGetCurrentDesc(GuiShell_WindowHandle handle, GuiShell_WindowDesc* desc) {
 ASSERT(desc);
 memcpy(desc, &gMainWindowHandle.desc, sizeof(GuiShell_WindowDesc));
}

EXTERN_C void GuiShell_Terminate() {
  [[NSApplication sharedApplication] terminate:nil];
}

