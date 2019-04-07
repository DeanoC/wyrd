#include "core/core.h"
#include "theforge_display/theforge_display.hpp"
#include "theforge/renderer.hpp"

// I usually don't global namespace things
using namespace TheForge;
using namespace TheForge::Display;

EXTERN_C void TheForge_Display_AddSwapChain(Renderer *pRenderer,
                                    const SwapChainDesc *p_desc,
                                    SwapChain **pp_swap_chain) {
  AddSwapChain(pRenderer, p_desc, pp_swap_chain);
}
EXTERN_C void TheForge_Display_RemoveSwapChain(Renderer *pRenderer, SwapChain *p_swap_chain) {
  RemoveSwapChain(pRenderer,
                  p_swap_chain);
}
EXTERN_C void TheForge_Display_AcquireNextImage(Renderer *pRenderer,
                                        SwapChain *p_swap_chain,
                                        Semaphore *p_signal_semaphore,
                                        Fence *p_fence,
                                        uint32_t *p_image_index) {
  AcquireNextImage(pRenderer,
                   p_swap_chain,
                   p_signal_semaphore,
                   p_fence,
                   p_image_index);
}
EXTERN_C void TheForge_Display_QueuePresent(Queue *p_queue,
                                    SwapChain *p_swap_chain,
                                    uint32_t swap_chain_image_index,
                                    uint32_t wait_semaphore_count,
                                    Semaphore **pp_wait_semaphores) {
  QueuePresent(p_queue,
               p_swap_chain,
               swap_chain_image_index,
               wait_semaphore_count,
               pp_wait_semaphores);
}

EXTERN_C void TheForge_Display_ToggleVSync(Renderer *pRenderer, SwapChain **ppSwapchain) {
  ToggleVSync(pRenderer, ppSwapchain);
}
