#import <simd/simd.h>
#import <MetalKit/MetalKit.h>

#include "core/core.h"
#include "guishell/platform.hpp"
#include "theforge/shader_reflection.hpp"
#include "theforge/metal/renderer.hpp"
#include "theforge/metal/utils.hpp"
#include "theforge_display/metal/metal.hpp"

namespace TheForge { namespace Display { namespace Metal {

void ToggleVSync(TheForge::Metal::Renderer *pRenderer, SwapChain **pSwapchain) {

  using namespace TheForge::Metal;

#if !defined(TARGET_IOS)
  (*pSwapchain)->mDesc.mEnableVsync = !(*pSwapchain)->mDesc.mEnableVsync;
  //no need to have vsync on layers otherwise we will wait on semaphores
  //get a copy of the layer for nextDrawables
  CAMetalLayer *layer = (CAMetalLayer * )(*pSwapchain)->pMTKView.layer;

  //only available on macresources OS.
  //VSync seems to be necessary on iOS.
  if (!(*pSwapchain)->mDesc.mEnableVsync) {
    (*pSwapchain)->pMTKView.enableSetNeedsDisplay = YES;
    (*pSwapchain)->pMTKView.paused = YES;
    layer.displaySyncEnabled = false;
  } else {
    (*pSwapchain)->pMTKView.enableSetNeedsDisplay = NO;
    (*pSwapchain)->pMTKView.paused = NO;
    layer.displaySyncEnabled = true;
  }
#endif
}

void AddSwapChain(TheForge::Metal::Renderer *pRenderer, const SwapChainDesc *pDesc, SwapChain **ppSwapChain) {
  ASSERT(pRenderer);
  ASSERT(pDesc);
  ASSERT(ppSwapChain);

  using namespace TheForge::Metal;

  SwapChain *pSwapChain = (SwapChain *) calloc(1, sizeof(*pSwapChain));
  pSwapChain->mDesc = *pDesc;

  // Assign MTKView to the swapchain.
  GuiShell::AppleWindow *appleWindow = (GuiShell::AppleWindow *) GuiShell_GetPlatformWindowPtr();
  pSwapChain->pMTKView = appleWindow->metalView;
  pSwapChain->pMTKView.device = pRenderer->pDevice;
  pSwapChain->pMTKView.autoresizesSubviews = TRUE;
  pSwapChain->pMTKView.preferredFramesPerSecond = 60.0;
  pSwapChain->pMTKView.enableSetNeedsDisplay = NO;
  pSwapChain->pMTKView.paused = NO;
  pSwapChain->pMTKView.framebufferOnly = FALSE;

#if !defined(TARGET_IOS)
  //no need to have vsync on layers otherwise we will wait on semaphores
  //get a copy of the layer for nextDrawables
  CAMetalLayer *layer = (CAMetalLayer *) pSwapChain->pMTKView.layer;
  pSwapChain->pMTKView.layer = layer;

  //only available on macresources OS.
  //VSync seems to be necessary on iOS.
  if (!pDesc->mEnableVsync) {
    pSwapChain->pMTKView.enableSetNeedsDisplay = YES;
    pSwapChain->pMTKView.paused = YES;

    //This needs to be set to false to have working non-vsync
    //otherwise present drawables will wait on vsync.
    layer.displaySyncEnabled = false;
  } else {
    //This needs to be set to false to have working vsync
    layer.displaySyncEnabled = true;
  }

  pSwapChain->pMTKView.wantsLayer = YES;
#endif
  pSwapChain->mMTKDrawable = nil;

  // Set the view pixel format to match the swapchain's pixel format.
  MTLPixelFormat pixelFormat = Util::ToMtlPixelFormat(pSwapChain->mDesc.mColorFormat);
  pSwapChain->pMTKView.colorPixelFormat = pixelFormat;

  // Create present command buffer for the swapchain.
  TheForge::Metal::Queue *q = (TheForge::Metal::Queue * )(pSwapChain->mDesc.ppPresentQueues)[0];
  pSwapChain->presentCommandBuffer = [q->mtlCommandQueue commandBuffer];

  // Create the swapchain RT descriptor.
  TheForge::RenderTargetDesc descColor = {};
  descColor.mWidth = pSwapChain->mDesc.mWidth;
  descColor.mHeight = pSwapChain->mDesc.mHeight;
  descColor.mDepth = 1;
  descColor.mArraySize = 1;
  descColor.mFormat = pSwapChain->mDesc.mColorFormat;
  descColor.mClearValue = pSwapChain->mDesc.mColorClearValue;
  descColor.mSampleCount = TheForge::SAMPLE_COUNT_1;
  descColor.mSampleQuality = 0;

  TheForge::Metal::RenderTarget **renderTargets = (TheForge::Metal::RenderTarget **)
      calloc(pSwapChain->mDesc.mImageCount, sizeof(TheForge::Metal::RenderTarget));
  pSwapChain->ppSwapchainRenderTargets = (TheForge::RenderTarget **) renderTargets;

  for (uint32_t i = 0; i < pSwapChain->mDesc.mImageCount; ++i) {
    AddRenderTarget(pRenderer,
                    &descColor,
                    &renderTargets[i]);
  }
  *ppSwapChain = pSwapChain;
}

void RemoveSwapChain(TheForge::Metal::Renderer *pRenderer, SwapChain *pSwapChain) {
  ASSERT(pRenderer);
  ASSERT(pSwapChain);

  using namespace TheForge::Metal;

  pSwapChain->presentCommandBuffer = nil;

  for (uint32_t i = 0; i < pSwapChain->mDesc.mImageCount; ++i) {
    RemoveRenderTarget(pRenderer, (TheForge::Metal::RenderTarget *) pSwapChain->ppSwapchainRenderTargets[i]);
  }

  free(pSwapChain->ppSwapchainRenderTargets);
  free(pSwapChain);
}

void AcquireNextImage(TheForge::Metal::Renderer *pRenderer,
                      SwapChain *pSwapChain,
                      TheForge::Metal::Semaphore *pSignalSemaphore,
                      TheForge::Metal::Fence *pFence,
                      uint32_t *pImageIndex) {
  ASSERT(pRenderer);
  ASSERT(pRenderer->pDevice != nil);
  ASSERT(pSwapChain);
  ASSERT(pSignalSemaphore || pFence);

  CAMetalLayer *layer = (CAMetalLayer *) pSwapChain->pMTKView.layer;

  if (pSwapChain->mMTKDrawable == nil) {
    pSwapChain->mMTKDrawable = [layer nextDrawable];
  }

  // Look for the render target containing this texture.
  // If not found: assign it to an empty slot
  for (uint32_t i = 0; i < pSwapChain->mDesc.mImageCount; i++) {
    TheForge::Metal::RenderTarget *renderTarget = (TheForge::Metal::RenderTarget *) pSwapChain->ppSwapchainRenderTargets[i];
    TheForge::Metal::Texture *texture = (TheForge::Metal::Texture *) renderTarget->pTexture;
    if (texture->mtlTexture == pSwapChain->mMTKDrawable.texture) {
      *pImageIndex = i;
      return;
    }
  }

  // Not found: assign the texture to an empty slot
  for (uint32_t i = 0; i < pSwapChain->mDesc.mImageCount; i++) {
    TheForge::Metal::RenderTarget *renderTarget = (TheForge::Metal::RenderTarget *) pSwapChain->ppSwapchainRenderTargets[i];
    TheForge::Metal::Texture *texture = (TheForge::Metal::Texture *) renderTarget->pTexture;
    if (texture->mtlTexture == nil) {
      texture->mtlTexture = pSwapChain->mMTKDrawable.texture;

      // Update the swapchain RT size according to the new drawable's size.
      texture->mDesc.mWidth = (uint32_t) pSwapChain->mMTKDrawable.texture.width;
      texture->mDesc.mHeight = (uint32_t) pSwapChain->mMTKDrawable.texture.height;
      pSwapChain->ppSwapchainRenderTargets[i]->mDesc.mWidth = texture->mDesc.mWidth;
      pSwapChain->ppSwapchainRenderTargets[i]->mDesc.mHeight = texture->mDesc.mHeight;

      *pImageIndex = i;
      return;
    }
  }

  // The swapchain textures have changed internally:
  // Invalidate the texures and re-acquire the render targets
  for (uint32_t i = 0; i < pSwapChain->mDesc.mImageCount; i++) {
    TheForge::Metal::RenderTarget *renderTarget = (TheForge::Metal::RenderTarget *) pSwapChain->ppSwapchainRenderTargets[i];
    TheForge::Metal::Texture *texture = (TheForge::Metal::Texture *) renderTarget->pTexture;
    texture->mtlTexture = nil;
  }

  AcquireNextImage(pRenderer, pSwapChain, pSignalSemaphore, pFence, pImageIndex);
}

void QueuePresent(
    TheForge::Metal::Queue *pQueue,
    SwapChain *pSwapChain,
    uint32_t swapChainImageIndex,
    uint32_t waitSemaphoreCount,
    TheForge::Metal::Semaphore **ppWaitSemaphores) {
  ASSERT(pQueue);
  if (waitSemaphoreCount > 0) {
    ASSERT(ppWaitSemaphores);
  }
  ASSERT(pQueue->mtlCommandQueue != nil);

  @autoreleasepool {
#ifndef TARGET_IOS
    [pSwapChain->presentCommandBuffer presentDrawable:pSwapChain->mMTKDrawable];
#else
    [pSwapChain->presentCommandBuffer presentDrawable:pSwapChain->mMTKDrawable
                                     afterMinimumDuration:1.0 / pSwapChain->pMTKView.preferredFramesPerSecond];
        //[pSwapChain->presentCommandBuffer presentDrawable:pSwapChain->pMTKView.currentDrawable];
#endif
  }

  [pSwapChain->presentCommandBuffer commit];

  // after committing a command buffer no more commands can be encoded on it: create a new command buffer for future commands
  pSwapChain->presentCommandBuffer = [pQueue->mtlCommandQueue commandBuffer];
  pSwapChain->mMTKDrawable = nil;
}

} } } // end namespace