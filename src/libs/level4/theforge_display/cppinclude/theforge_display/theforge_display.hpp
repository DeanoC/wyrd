#pragma once
#ifndef WYRD_THEFORGE_DISPLAY_THEFORGE_DISPLAY_HPP
#define WYRD_THEFORGE_DISPLAY_THEFORGE_DISPLAY_HPP

#include "core/core.h"
#include "theforge/renderer.hpp"
#include "theforge_display/theforge_display.h"

namespace TheForge { namespace Display {
using SwapChainDesc = TheForge_Display_SwapChainDesc;
using SwapChain = TheForge_Display_SwapChain;

void AddSwapChain(Renderer *pRenderer,
                  const SwapChainDesc *p_desc,
                  SwapChain **pp_swap_chain);

void RemoveSwapChain(Renderer *pRenderer, SwapChain *p_swap_chain);

void AcquireNextImage(Renderer *pRenderer,
                      SwapChain *p_swap_chain,
                      Semaphore *p_signal_semaphore,
                      Fence *p_fence,
                      uint32_t *p_image_index);
void QueuePresent(Queue *p_queue,
                  SwapChain *p_swap_chain,
                  uint32_t swap_chain_image_index,
                  uint32_t wait_semaphore_count,
                  Semaphore **pp_wait_semaphores);
void ToggleVSync(Renderer *pRenderer, SwapChain **ppSwapchain);

Image_Format GetRecommendedSwapchainFormat(bool hintHDR);

} } // end namespace

#endif //WYRD_THEFORGE_DISPLAY_THEFORGE_DISPLAY_HPP
