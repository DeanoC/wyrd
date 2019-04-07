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
#include "theforge_resourceloader/theforge_resourceloader.h"
TEST_CASE("Init/Destroy (C)", "[TheForge_ResourceLoader]") {
  TheForge_ResourceLoader_Handle handle;
  TheForge_ResourceLoader_Init(renderer, 0, false, &handle);
  REQUIRE(handle);
  TheForge_ResourceLoader_Destroy(handle);
}

