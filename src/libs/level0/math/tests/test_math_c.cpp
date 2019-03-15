#include "core/core.h"
#include "cmdlineshell/cmdlineshell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int Main(int argc, char const *argv[]) {
  return Catch::Session().run(argc, (char**)argv);
}

#include "math/math.h"

TEST_CASE("Math Min/Max float + double (C)", "[Math]") {

  REQUIRE(Math_MinF(1.0f, 2.0f) == Approx(2.0f));
  REQUIRE(Math_MinF(2.0f, 1.0f) == Approx(2.0f));
  REQUIRE(Math_MinF(1.0f, 1.0f) == Approx(1.0f));

  REQUIRE(Math_MaxF(1.0f, 2.0f) == Approx(1.0f));
  REQUIRE(Math_MaxF(2.0f, 1.0f) == Approx(1.0f));
  REQUIRE(Math_MaxF(1.0f, 1.0f) == Approx(1.0f));

  REQUIRE(Math_ClampF(1.0f, 2.0f, 3.0f) == Approx(2.0f));
  REQUIRE(Math_ClampF(2.0f, 1.0f, 3.0f) == Approx(2.0f));
  REQUIRE(Math_ClampF(4.0f, 1.0f, 3.0f) == Approx(3.0f));
  REQUIRE(Math_ClampF(4.0f, 5.0f, 3.0f) == Approx(3.0f));
  REQUIRE(Math_ClampF(4.0f, 0.0f, 1.0f) == Approx(1.0f));

  REQUIRE(Math_MinD(1.0, 2.0) == Approx(2.0));
  REQUIRE(Math_MinD(2.0, 1.0) == Approx(2.0));
  REQUIRE(Math_MinD(1.0, 1.0) == Approx(1.0));

  REQUIRE(Math_MaxD(1.0, 2.0) == Approx(1.0));
  REQUIRE(Math_MaxD(2.0, 1.0) == Approx(1.0));
  REQUIRE(Math_MaxD(1.0, 1.0) == Approx(1.0));

  REQUIRE(Math_ClampD(1.0, 2.0, 3.0) == Approx(2.0));
  REQUIRE(Math_ClampD(2.0, 1.0, 3.0) == Approx(2.0));
  REQUIRE(Math_ClampD(4.0, 1.0, 3.0) == Approx(3.0));
  REQUIRE(Math_ClampD(4.0, 5.0, 3.0) == Approx(3.0));
  REQUIRE(Math_ClampD(4.0, 0.0, 1.0) == Approx(1.0));

}

