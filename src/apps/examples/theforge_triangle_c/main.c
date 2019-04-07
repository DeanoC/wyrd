#include "core/core.h"
#include "guishell/guishell.h"
#include "guishell/window.h"
#include "theforge/renderer.h"

DEFINE_APPLICATION_MAIN
const uint32_t gImageCount = 3;

TheForge_Renderer *pRenderer = NULL;

TheForge_Queue *pGraphicsQueue = NULL;
TheForge_CmdPool *pCmdPool = NULL;
TheForge_Cmd **ppCmds = NULL;

TheForge_SwapChain *pSwapChain = NULL;
TheForge_RenderTarget *pDepthBuffer = NULL;
TheForge_Fence *pRenderCompleteFences[gImageCount] = {NULL};
TheForge_Semaphore *pImageAcquiredSemaphore = NULL;
TheForge_Semaphore *pRenderCompleteSemaphores[gImageCount] = {NULL};

TheForge_Shader *pSphereShader = NULL;
TheForge_Buffer *pSphereVertexBuffer = NULL;
TheForge_Pipeline *pSpherePipeline = NULL;

TheForge_Shader *pSkyBoxDrawShader = NULL;
TheForge_Buffer *pSkyBoxVertexBuffer = NULL;
TheForge_Pipeline *pSkyBoxDrawPipeline = NULL;
TheForge_RootSignature *pRootSignature = NULL;
TheForge_Sampler *pSamplerSkyBox = NULL;
TheForge_Texture *pSkyBoxTextures[6];
TheForge_DepthState *pDepth = NULL;
TheForge_RasterizerState *pSkyboxRast = NULL;
TheForge_RasterizerState *pSphereRast = NULL;

TheForge_Buffer *pProjViewUniformBuffer[gImageCount] = {NULL};
TheForge_Buffer *pSkyboxUniformBuffer[gImageCount] = {NULL};

uint32_t gFrameIndex = 0;

//TheForge_UniformBlock gUniformData;
//TheForge_UniformBlock gUniformDataSky;

static bool AddSwapChain()
{
  GuiShell_WindowDesc windowDesc = {0};
  GuiShell_WindowGetCurrentDesc(&windowDesc);

  TheForge_SwapChainDesc swapChainDesc = {0};
  swapChainDesc.pWindow = &windowDesc;
  swapChainDesc.mPresentQueueCount = 1;
  swapChainDesc.ppPresentQueues = &pGraphicsQueue;
  swapChainDesc.mWidth = windowDesc.width;
  swapChainDesc.mHeight = windowDesc.height;
  swapChainDesc.mImageCount = gImageCount;
  swapChainDesc.mSampleCount = TheForge_SAMPLE_COUNT_1;
  swapChainDesc.mColorFormat = TheForge_GetRecommendedSwapchainFormat(true);
  swapChainDesc.mEnableVsync = false;
  TheForge_AddSwapChain(pRenderer, &swapChainDesc, &pSwapChain);

  return pSwapChain != NULL;
}

static bool AddDepthBuffer()
{
  GuiShell_WindowDesc windowDesc = {0};
  GuiShell_WindowGetCurrentDesc(&windowDesc);

  // Add depth buffer
  TheForge_RenderTargetDesc depthRT = {0};
  depthRT.mArraySize = 1;
  depthRT.mClearValue.depth = 1.0f;
  depthRT.mClearValue.stencil = 0;
  depthRT.mDepth = 1;
  depthRT.mFormat = Image_Format_D32_SFLOAT;
  depthRT.mWidth = windowDesc.width;
  depthRT.mHeight = windowDesc.height;
  depthRT.mSampleCount = TheForge_SAMPLE_COUNT_1;
  depthRT.mSampleQuality = 0;
  TheForge_AddRenderTarget(pRenderer, &depthRT, &pDepthBuffer);

  return pDepthBuffer != NULL;
}

static bool Init() {
  LOGINFO("Initing");
  // window and renderer setup
  TheForge_RendererDesc settings = {0};
  TheForge_InitRenderer("theforge_triangle_c", &settings, &pRenderer);

  //check for init success
  if (!pRenderer) {
    return false;
  }

  TheForge_QueueDesc queueDesc = {0};
  queueDesc.mType = TheForge_CMD_POOL_DIRECT;
  TheForge_AddQueue(pRenderer, &queueDesc, &pGraphicsQueue);
  TheForge_AddCmdPool(pRenderer, pGraphicsQueue, false, &pCmdPool);
  TheForge_AddCmd_n(pCmdPool, false, gImageCount, &ppCmds);

  for (uint32_t i = 0; i < gImageCount; ++i) {
    TheForge_AddFence(pRenderer, &pRenderCompleteFences[i]);
    TheForge_AddSemaphore(pRenderer, &pRenderCompleteSemaphores[i]);
  }
  TheForge_AddSemaphore(pRenderer, &pImageAcquiredSemaphore);

  TheForge_SamplerDesc samplerDesc = {
      TheForge_FILTER_LINEAR,
      TheForge_FILTER_LINEAR,
      TheForge_MIPMAP_MODE_NEAREST,
      TheForge_ADDRESS_MODE_CLAMP_TO_EDGE,
      TheForge_ADDRESS_MODE_CLAMP_TO_EDGE,
      TheForge_ADDRESS_MODE_CLAMP_TO_EDGE
  };
  TheForge_AddSampler(pRenderer, &samplerDesc, &pSamplerSkyBox);

//  TheForge_ShaderLoadDesc skyShader = {0};
//  skyShader.mStages[0] = { "skybox.vert", NULL, 0, FSR_SrcShaders };
//  skyShader.mStages[1] = { "skybox.frag", NULL, 0, FSR_SrcShaders };
//  ShaderLoadDesc basicShader = {};
//  basicShader.mStages[0] = { "basic.vert", NULL, 0, FSR_SrcShaders };
//  basicShader.mStages[1] = { "basic.frag", NULL, 0, FSR_SrcShaders };

//  TheForge_AddShader(pRenderer, &skyShader, &pSkyBoxDrawShader);
//  TheForge_AddShader(pRenderer, &basicShader, &pSphereShader);
  /*
  Shader* shaders[] = { pSphereShader, pSkyBoxDrawShader };
  const char* pStaticSamplers[] = { "uSampler0" };
  RootSignatureDesc rootDesc = {};
  rootDesc.mStaticSamplerCount = 1;
  rootDesc.ppStaticSamplerNames = pStaticSamplers;
  rootDesc.ppStaticSamplers = &pSamplerSkyBox;
  rootDesc.mShaderCount = 2;
  rootDesc.ppShaders = shaders;
  addRootSignature(pRenderer, &rootDesc, &pRootSignature);
*/
  TheForge_RasterizerStateDesc rasterizerStateDesc = {0};
  rasterizerStateDesc.mCullMode = TheForge_CULL_MODE_NONE;
  TheForge_AddRasterizerState(pRenderer, &rasterizerStateDesc, &pSkyboxRast);

  TheForge_RasterizerStateDesc sphereRasterizerStateDesc = {0};
  sphereRasterizerStateDesc.mCullMode = TheForge_CULL_MODE_FRONT;
  TheForge_AddRasterizerState(pRenderer, &sphereRasterizerStateDesc, &pSphereRast);

  TheForge_DepthStateDesc depthStateDesc = {};
  depthStateDesc.mDepthTest = true;
  depthStateDesc.mDepthWrite = true;
  depthStateDesc.mDepthFunc = TheForge_CMP_LEQUAL;
  TheForge_AddDepthState(pRenderer, &depthStateDesc, &pDepth);

  return true;
}

static bool Load() {
  LOGINFO("Loading");
  if (!AddSwapChain())
    return false;

  if (!AddDepthBuffer())
    return false;

  return true;
}

static void Update(double deltaTimeMS) {
}

static void Draw() {
  TheForge_AcquireNextImage(pRenderer, pSwapChain, pImageAcquiredSemaphore, NULL, &gFrameIndex);

  TheForge_RenderTarget* pRenderTarget = pSwapChain->ppSwapchainRenderTargets[gFrameIndex];
  TheForge_Semaphore*    pRenderCompleteSemaphore = pRenderCompleteSemaphores[gFrameIndex];
  TheForge_Fence*        pRenderCompleteFence = pRenderCompleteFences[gFrameIndex];

  // Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
  TheForge_FenceStatus fenceStatus;
  TheForge_GetFenceStatus(pRenderer, pRenderCompleteFence, &fenceStatus);
  if (fenceStatus == TheForge_FENCE_STATUS_INCOMPLETE)
    TheForge_WaitForFences(pRenderer, 1, &pRenderCompleteFence);

  // simply record the screen cleaning command
  TheForge_LoadActionsDesc loadActions = {0};
  loadActions.mLoadActionsColor[0] = TheForge_LOAD_ACTION_CLEAR;
  loadActions.mClearColorValues[0].r = 1.0f;
  loadActions.mClearColorValues[0].g = 1.0f;
  loadActions.mClearColorValues[0].b = 0.0f;
  loadActions.mClearColorValues[0].a = 0.0f;
  loadActions.mLoadActionDepth = TheForge_LOAD_ACTION_CLEAR;
  loadActions.mClearDepth.depth = 1.0f;
  loadActions.mClearDepth.stencil = 0;

  TheForge_Cmd* cmd = ppCmds[gFrameIndex];
  TheForge_BeginCmd(cmd);

  TheForge_TextureBarrier barriers[] = {
      { pRenderTarget->pTexture, TheForge_RESOURCE_STATE_RENDER_TARGET },
      { pDepthBuffer->pTexture, TheForge_RESOURCE_STATE_DEPTH_WRITE },
  };
  TheForge_CmdResourceBarrier(cmd, 0, NULL, 2, barriers, false);

  TheForge_CmdBindRenderTargets(cmd, 1, &pRenderTarget, pDepthBuffer, &loadActions, NULL, NULL, -1, -1);
  TheForge_CmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mDesc.mWidth, (float)pRenderTarget->mDesc.mHeight, 0.0f, 1.0f);
  TheForge_CmdSetScissor(cmd, 0, 0, pRenderTarget->mDesc.mWidth, pRenderTarget->mDesc.mHeight);

  TheForge_CmdBindRenderTargets(cmd, 0, NULL, NULL, NULL, NULL, NULL, -1, -1);
  TheForge_CmdEndDebugMarker(cmd);

  barriers[0].pTexture = pRenderTarget->pTexture;
  barriers[0].mNewState = TheForge_RESOURCE_STATE_PRESENT;

  TheForge_CmdResourceBarrier(cmd, 0, NULL, 1, barriers, true);
  TheForge_EndCmd(cmd);

  TheForge_QueueSubmit(pGraphicsQueue, 1, &cmd, pRenderCompleteFence, 1, &pImageAcquiredSemaphore, 1, &pRenderCompleteSemaphore);
  TheForge_QueuePresent(pGraphicsQueue, pSwapChain, gFrameIndex, 1, &pRenderCompleteSemaphore);

}

static void Unload() {
  LOGINFO("Unloading");

  TheForge_WaitQueueIdle(pGraphicsQueue);

//  TheForge_RemovePipeline(pRenderer, pSkyBoxDrawPipeline);
//  TheForge_RemovePipeline(pRenderer, pSpherePipeline);

  TheForge_RemoveSwapChain(pRenderer, pSwapChain);
  TheForge_RemoveRenderTarget(pRenderer, pDepthBuffer);

}

static void Exit() {
  LOGINFO("Exiting");
  /*
  for (uint32_t i = 0; i < gImageCount; ++i)
  {
    removeResource(pProjViewUniformBuffer[i]);
    removeResource(pSkyboxUniformBuffer[i]);
  }
  removeResource(pSphereVertexBuffer);
  removeResource(pSkyBoxVertexBuffer);

  for (uint i = 0; i < 6; ++i)
    removeResource(pSkyBoxTextures[i]); */

  TheForge_RemoveSampler(pRenderer, pSamplerSkyBox);
  //removeShader(pRenderer, pSphereShader);
  //removeShader(pRenderer, pSkyBoxDrawShader);
  //removeRootSignature(pRenderer, pRootSignature);

  TheForge_RemoveDepthState(pRenderer, pDepth);
  TheForge_RemoveRasterizerState(pRenderer, pSphereRast);
  TheForge_RemoveRasterizerState(pRenderer, pSkyboxRast);

  TheForge_RemoveSemaphore(pRenderer, pImageAcquiredSemaphore);

  for (uint32_t i = 0; i < gImageCount; ++i)
  {
    TheForge_RemoveFence(pRenderer, pRenderCompleteFences[i]);
    TheForge_RemoveSemaphore(pRenderer, pRenderCompleteSemaphores[i]);
  }

  TheForge_RemoveCmd_n(pCmdPool, gImageCount, ppCmds);
  TheForge_RemoveCmdPool(pRenderer, pCmdPool);

  TheForge_RemoveQueue(pRenderer, pGraphicsQueue);
  TheForge_RemoveRenderer(pRenderer);
}

static void Abort() {
  abort();
}

EXTERN_C void GuiShell_AppConfig(GuiShell_Functions *functions, GuiShell_WindowDesc *initialWindow) {
  functions->init = &Init;
  functions->load = &Load;
  functions->unload = &Unload;
  functions->update = &Update;
  functions->draw = &Draw;
  functions->exit = &Exit;
  functions->abort = &Abort;

  initialWindow->name = "theforge_triangle_c";
  initialWindow->width = -1;
  initialWindow->height = -1;
  initialWindow->fullScreen = false;
  initialWindow->windowsFlags = 0;
  initialWindow->visible = true;
  initialWindow->fullScreen = false;
}