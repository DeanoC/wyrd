#include "core/core.h"
#include "core/logger.h"
#include "logmanager/logmanager.h"
#include "cmdlineshell/cmdlineshell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

#include "theforge/renderer.h"
TheForge_Renderer *renderer;

int Main(int argc, char const *argv[]) {
  LogMan_Handle logManHandle = LogMan_Alloc();
  LogMan_SetInfoQuiet(logManHandle, true);

  TheForge_RendererDesc desc {};
  TheForge_InitRenderer("test", &desc, &renderer);
  ASSERT(renderer);

  int res = Catch::Session().run(argc, (char**)argv);

  TheForge_RemoveRenderer(renderer);
  LogMan_Free(logManHandle);
  return res;
}

TEST_CASE("simple add/remove APIs (C)", "[TheForge]") {
  SECTION("Fence") {
    TheForge_Fence *fence;
    TheForge_AddFence(renderer, &fence);
    REQUIRE(fence);
    TheForge_RemoveFence(renderer, fence);
  }

  SECTION("Semaphore") {
    TheForge_Semaphore *semaphore;
    TheForge_AddSemaphore(renderer, &semaphore);
    REQUIRE(semaphore);
    TheForge_RemoveSemaphore(renderer, semaphore);
  }

  SECTION("Queue") {
    SECTION("NONE NORMAL DIRECT") {
      TheForge_Queue *queue;
      TheForge_QueueDesc desc {
          TheForge_QUEUE_FLAG_NONE,
          TheForge_QUEUE_PRIORITY_NORMAL,
          TheForge_CMD_POOL_DIRECT,
          0
      };

      TheForge_AddQueue(renderer, &desc, &queue);
      REQUIRE(queue);

      TheForge_RemoveQueue(renderer, queue);
    }SECTION("DISABLE_GPU_TIMEOUT NORMAL DIRECT") {
      TheForge_Queue *queue;
      TheForge_QueueDesc desc {
          TheForge_QUEUE_FLAG_DISABLE_GPU_TIMEOUT,
          TheForge_QUEUE_PRIORITY_NORMAL,
          TheForge_CMD_POOL_DIRECT,
          0
      };

      TheForge_AddQueue(renderer, &desc, &queue);
      REQUIRE(queue);

      TheForge_RemoveQueue(renderer, queue);
    }

    SECTION("NONE NORMAL BUNDLE") {
      TheForge_Queue *queue;
      TheForge_QueueDesc desc {
          TheForge_QUEUE_FLAG_NONE,
          TheForge_QUEUE_PRIORITY_NORMAL,
          TheForge_CMD_POOL_BUNDLE,
          0
      };

      TheForge_AddQueue(renderer, &desc, &queue);
      REQUIRE(queue);

      TheForge_RemoveQueue(renderer, queue);
    }SECTION("NONE NORMAL COPY") {
      TheForge_Queue *queue;
      TheForge_QueueDesc desc {
          TheForge_QUEUE_FLAG_NONE,
          TheForge_QUEUE_PRIORITY_NORMAL,
          TheForge_CMD_POOL_COPY,
          0
      };

      TheForge_AddQueue(renderer, &desc, &queue);
      REQUIRE(queue);

      TheForge_RemoveQueue(renderer, queue);
    }SECTION("NONE NORMAL COMPUTE") {
      TheForge_Queue *queue;
      TheForge_QueueDesc desc {
          TheForge_QUEUE_FLAG_NONE,
          TheForge_QUEUE_PRIORITY_NORMAL,
          TheForge_CMD_POOL_COMPUTE,
          0
      };

      TheForge_AddQueue(renderer, &desc, &queue);
      REQUIRE(queue);

      TheForge_RemoveQueue(renderer, queue);
    }
  }
}
TEST_CASE("BlendState add/remove API (C)", "[TheForge]") {
  SECTION("Add RT0 blender") {
    TheForge_BlendStateDesc desc {};
    desc.mSrcFactors[0] = TheForge_BC_ONE;
    desc.mDstFactors[0] = TheForge_BC_ONE;
    desc.mSrcAlphaFactors[0] = TheForge_BC_ZERO;
    desc.mDstAlphaFactors[0] = TheForge_BC_ZERO;
    desc.mBlendModes[0] = TheForge_BM_ADD;
    desc.mBlendAlphaModes[0] = TheForge_BM_ADD;
    desc.mMasks[0]  = 0x7fffffff;
    desc.mRenderTargetMask = TheForge_BLEND_STATE_TARGET_0;
    desc.mAlphaToCoverage = false;
    desc.mIndependentBlend = false;
    TheForge_BlendState *blendState;
    TheForge_AddBlendState(renderer, &desc, &blendState);
    REQUIRE(blendState);
    TheForge_RemoveBlendState(renderer, blendState);
  }
}

TEST_CASE("DepthState add/remove API (C)", "[TheForge]") {
  SECTION("depth test and write state ") {
    TheForge_DepthStateDesc desc {};
    desc.mDepthTest = true ;
    desc.mDepthWrite = true;
    desc.mDepthFunc = TheForge_CMP_LEQUAL;

    TheForge_DepthState *depthState;
    TheForge_AddDepthState(renderer, &desc, &depthState);
    REQUIRE(depthState);
    TheForge_RemoveDepthState(renderer, depthState);
  }
}

TEST_CASE("buffer add/remove API (C)", "[TheForge]") {
  SECTION("64K GPU CPU start UNDEFINED") {
    TheForge_BufferDesc desc {
      64 * 1024,
      TheForge_RESOURCE_MEMORY_USAGE_GPU_ONLY,
      TheForge_TEXTURE_CREATION_FLAG_NONE,
      TheForge_RESOURCE_MEMORY_USAGE_CPU_ONLY,
      TheForge_INDEX_TYPE_UINT16, // not used
      0,
      0,
      0,
      0,
      nullptr,
      Image_Format_UNDEFINED, // not used
      TheForge_DESCRIPTOR_TYPE_UNDEFINED,
      "test",
      nullptr,
      0,
      0
    };
    TheForge_Buffer* buffer;
    TheForge_AddBuffer(renderer, &desc, &buffer);
    REQUIRE(buffer);
    TheForge_RemoveBuffer(renderer, buffer);
  }

  SECTION("64K GPU CPU start VERTEX") {
    TheForge_BufferDesc desc {
        64 * 1024,
        TheForge_RESOURCE_MEMORY_USAGE_GPU_ONLY,
        TheForge_TEXTURE_CREATION_FLAG_NONE,
        TheForge_RESOURCE_MEMORY_USAGE_CPU_ONLY,
        TheForge_INDEX_TYPE_UINT16, // not used
        3 * sizeof(float), // vertex stride
        0,
        0,
        0,
        nullptr,
        Image_Format_UNDEFINED, // not used
        TheForge_DESCRIPTOR_TYPE_VERTEX_BUFFER,
        "test",
        nullptr,
        0,
        0
    };
    TheForge_Buffer* buffer;
    TheForge_AddBuffer(renderer, &desc, &buffer);
    REQUIRE(buffer);
    TheForge_RemoveBuffer(renderer, buffer);
  }

}