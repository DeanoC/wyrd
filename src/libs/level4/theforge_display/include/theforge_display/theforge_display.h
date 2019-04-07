#pragma once
#ifndef WYRD_THEFORGE_DISPLAY_THEFORGE_DISPLAY_H
#define WYRD_THEFORGE_DISPLAY_THEFORGE_DISPLAY_H

#include "core/core.h"
#include "guishell/window.h"
#include "theforge/renderer.h"

typedef struct TheForge_Display_SwapChainDesc {
  /// Window handle
  GuiShell_WindowDesc *pWindow;
  /// Queues which should be allowed to present
  struct TheForge_Queue **ppPresentQueues;
  /// Number of present queues
  uint32_t mPresentQueueCount;
  /// Number of backbuffers in this swapchain
  uint32_t mImageCount;
  /// Width of the swapchain
  uint32_t mWidth;
  /// Height of the swapchain
  uint32_t mHeight;
  /// Sample count
  TheForge_SampleCount mSampleCount;
  /// Sample quality (DirectX12 only)
  uint32_t mSampleQuality;
  /// Color format of the swapchain
  Image_Format mColorFormat;
  /// Clear value
  TheForge_ClearValue mColorClearValue;
  /// Set whether swap chain will be presented using vsync
  bool mEnableVsync;
} TheForge_Display_SwapChainDesc;

typedef struct TheForge_Display_SwapChain {
  TheForge_Display_SwapChainDesc mDesc;
  /// Render targets created from the swapchain back buffers
  TheForge_RenderTarget **ppSwapchainRenderTargets;
} TheForge_Display_SwapChain;

EXTERN_C void TheForge_Display_AddSwapChain(TheForge_Renderer *pRenderer,
                                    const TheForge_Display_SwapChainDesc *p_desc,
                                    TheForge_Display_SwapChain **pp_swap_chain);
EXTERN_C void TheForge_Display_RemoveSwapChain(TheForge_Renderer *pRenderer, TheForge_Display_SwapChain *p_swap_chain);

EXTERN_C void TheForge_Display_AcquireNextImage(TheForge_Renderer *pRenderer,
                                        TheForge_Display_SwapChain *p_swap_chain,
                                        TheForge_Semaphore *p_signal_semaphore,
                                        TheForge_Fence *p_fence,
                                        uint32_t *p_image_index);
EXTERN_C void TheForge_Display_QueuePresent(TheForge_Queue *p_queue,
                                    TheForge_Display_SwapChain *p_swap_chain,
                                    uint32_t swap_chain_image_index,
                                    uint32_t wait_semaphore_count,
                                    TheForge_Semaphore **pp_wait_semaphores);
EXTERN_C void TheForge_Display_ToggleVSync(TheForge_Renderer *pRenderer, TheForge_Display_SwapChain **ppSwapchain);


#endif //WYRD_SWAPCHAIN_H
