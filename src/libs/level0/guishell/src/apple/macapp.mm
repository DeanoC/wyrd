#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <AppKit/NSOpenPanel.h>
#include <AppKit/AppKit.h>

#include "core/core.h"
#include "core/logger.h"
#include "os/window.h"
#include "os/filesystem.hpp"
#include "guishell/guishell.hpp"
#include "macapp.hpp"

int main(int argc, char const* argv[])
{
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
void GetRecommendedResolution(Os_RectDesc_t* rect) {
  *rect = Os_RectDesc_t{ 0, 0, 1920, 1080 };
}

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

    Os_RectDesc_t rect;
    GetRecommendedResolution(&rect);

//    window->metalView = (MTKView*) CFBridgingRetain(view);

    @autoreleasepool {
      //if init fails then exit the app
      if (!GuiShell_Init()) {
        for (NSWindow *window in [NSApplication sharedApplication].windows) {
          [window close];
        }

        GuiShell_Terminate();
      }

      //if load fails then exit the app
      if (!GuiShell_Load()) {
        for (NSWindow *window in [NSApplication sharedApplication].windows) {
          [window close];
        }

        GuiShell_Terminate();
      }
    }
  }

  return self;
}

- (void)drawRectResized:(CGSize)size {
  // TODO deano
//  float newWidth = size.width * gRetinaScale.x;
//  float newHeight = size.height * gRetinaScale.y;
//  if (newWidth != pApp->mSettings.mWidth || newHeight != pApp->mSettings.mHeight) {
//    pApp->mSettings.mWidth = newWidth;
//    pApp->mSettings.mHeight = newHeight;
//    pApp->Unload();
//    pApp->Load();
//  }

  float newWidth = (float)size.width;
  float newHeight = (float)size.height;
  GuiShell_Unload();
  GuiShell_Load();
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

  GuiShell_Update(deltaTimeMS);
  GuiShell_Draw();

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
  GuiShell_Unload();
  GuiShell_Exit();
}
@end


