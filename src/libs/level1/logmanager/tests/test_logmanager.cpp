#include "core/core.h"
#include "core/logger.h"
#include "cmdlineshell/cmdlineshell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int Main(int argc, char const *argv[]) {
  return Catch::Session().run(argc, (char**)argv);
}

#include "logmanager/logmanager.hpp"

TEST_CASE("LogManager create/destroy", "[LogManager]") {
  LogManager *test = new LogManager;
  REQUIRE(test != nullptr);
  LOGINFO("test");
  LOGWARNING("test2");

  delete test;
  test = nullptr;
  LOGINFO("test");
  LOGWARNING("test2");

}