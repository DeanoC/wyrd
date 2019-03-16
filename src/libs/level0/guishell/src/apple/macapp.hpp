#pragma once
#ifndef WYRD_GUISHELL_MACAPP_HPP
#define WYRD_GUISHELL_MACAPP_HPP

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <AppKit/NSOpenPanel.h>

// Protocol abstracting the platform specific view in order to keep the Renderer class independent from platform
@protocol RenderDestinationProvider

@end

// Interface that controls the main updating/rendering loop on Metal appplications.
@interface MetalKitApplication : NSObject

- (nonnull instancetype)initWithMetalDevice:(nonnull id <MTLDevice>)device
    renderDestinationProvider:(nonnull id <RenderDestinationProvider>)renderDestinationProvider
    view:(nonnull MTKView *)view;

- (void)drawRectResized:(CGSize)size;
- (void)updateInput;
- (void)update;
- (void)shutdown;

@end

// Our view controller.  Implements the MTKViewDelegate protocol, which allows it to accept
// per-frame update and drawable resize callbacks.  Also implements the RenderDestinationProvider
// protocol, which allows our renderer object to get and set drawable properties such as pixel
// format and sample count
@interface GameViewController : NSViewController <MTKViewDelegate, RenderDestinationProvider>
@end

#endif //WYRD_GUISHELL_MACAPP_HPP
