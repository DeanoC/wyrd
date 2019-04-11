#include "core/core.h"
#include "theforge/renderer.hpp"
#include "theforge_display/theforge_display.hpp"
#include "theforge/metal/renderer.hpp"
#include "theforge_display/metal/metal.hpp"

namespace TheForge { namespace Display {

void AddSwapChain(TheForge::Renderer *pRenderer,
    const SwapChainDesc *p_desc,
    SwapChain **pp_swap_chain) {
  Metal::AddSwapChain((TheForge::Metal::Renderer *) pRenderer, p_desc, (Metal::SwapChain **) pp_swap_chain);
}

void RemoveSwapChain(TheForge::Renderer *pRenderer, SwapChain *p_swap_chain) {
  Metal::RemoveSwapChain((TheForge::Metal::Renderer *) pRenderer, (Metal::SwapChain *) p_swap_chain);
}
void QueuePresent(TheForge::Queue *p_queue,
                  SwapChain *p_swap_chain,
                  uint32_t swap_chain_image_index,
                  uint32_t wait_semaphore_count,
                  TheForge::Semaphore **pp_wait_semaphores) {
  Metal::QueuePresent((TheForge::Metal::Queue *) p_queue,
                      (Metal::SwapChain *) p_swap_chain,
                      swap_chain_image_index,
                      wait_semaphore_count,
                      (TheForge::Metal::Semaphore **) pp_wait_semaphores);
}

void ToggleVSync(TheForge::Renderer *pRenderer, SwapChain **ppSwapchain) {
  Metal::ToggleVSync((TheForge::Metal::Renderer *) pRenderer, (Metal::SwapChain **) ppSwapchain);
}

void AcquireNextImage(TheForge::Renderer *pRenderer,
                      SwapChain *p_swap_chain,
                      TheForge::Semaphore *p_signal_semaphore,
                      TheForge::Fence *p_fence,
                      uint32_t *p_image_index) {
  Metal::AcquireNextImage((TheForge::Metal::Renderer *) pRenderer,
                          (Metal::SwapChain *) p_swap_chain,
                          (TheForge::Metal::Semaphore *) p_signal_semaphore,
                          (TheForge::Metal::Fence *) p_fence,
                          p_image_index);
}
Image_Format GetRecommendedSwapchainFormat(bool hintHDR) {
  return Metal::GetRecommendedSwapchainFormat(hintHDR);
}
} }// end namespace