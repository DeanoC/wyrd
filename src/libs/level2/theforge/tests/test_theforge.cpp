#include "core/core.h"
#include "core/logger.h"
#include "cmdlineshell/cmdlineshell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int Main(int argc, char *argv[]) {
  return Catch::Session().run(argc, argv);
}

#include "theforge/renderer.h"

TEST_CASE("create/destroy (C)", "[TheForge]") {

  TheForge_RendererDesc desc;
  TheForge_Renderer* renderer;
  TheForge_InitRenderer("test", &desc, &renderer);

  TheForge_RemoveRenderer(renderer);
}