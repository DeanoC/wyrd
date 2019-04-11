#pragma once
#ifndef WYRD_THEFORGE_DISPLAY_METAL_METAL_HPP
#define WYRD_THEFORGE_DISPLAY_METAL_METAL_HPP

#import <simd/simd.h>
#import <MetalKit/MetalKit.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>

#include "core/core.h"
#include "theforge/renderer.hpp"
#include "theforge_display/theforge_display.hpp"
#include "theforge/metal/renderer.hpp"

namespace TheForge { namespace Display { namespace Metal {

struct SwapChain : public TheForge::Display::SwapChain {
  MTKView *pMTKView;
  id <CAMetalDrawable> mMTKDrawable;
  id <MTLCommandBuffer> presentCommandBuffer;
};

void AddSwapChain(TheForge::Metal::Renderer *pRenderer, const SwapChainDesc *p_desc, SwapChain **pp_swap_chain);
void RemoveSwapChain(TheForge::Metal::Renderer *pRenderer, SwapChain *p_swap_chain);

void AcquireNextImage(TheForge::Metal::Renderer *pRenderer,
                      SwapChain *p_swap_chain,
                      TheForge::Metal::Semaphore *p_signal_semaphore,
                      TheForge::Metal::Fence *p_fence,
                      uint32_t *p_image_index);

void QueuePresent(TheForge::Metal::Queue *p_queue,
                  SwapChain *p_swap_chain,
                  uint32_t swap_chain_image_index,
                  uint32_t wait_semaphore_count,
                  TheForge::Metal::Semaphore **pp_wait_semaphores);

void ToggleVSync(TheForge::Metal::Renderer *pRenderer, SwapChain **ppSwapchain);

Image_Format GetRecommendedSwapchainFormat(bool hintHDR);

} } } // end namespace
#endif //WYRD_THEFORGE_DISPLAY_METAL_METAL_HPP
