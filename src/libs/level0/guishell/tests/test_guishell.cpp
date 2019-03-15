
#include "guishell/guishell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int Main(int argc, char const *argv[]) {
  return Catch::Session().run(argc, (char**) argv);
}


