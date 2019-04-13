#include "core/core.h"
#include "os/filesystem.h"
#include "vfile/vfile.hpp"
#include "catch/catch.hpp"

#include "syoyo/tiny_exr.h"

// path to https://github.com/openexr/openexr-images
static const char *gBasePath = "test_data/exr/";

#define SET_PATH()   char existCurDir[1024]; \
Os_GetCurrentDir(existCurDir, sizeof(existCurDir)); \
char path[2048]; \
strcpy(path, existCurDir); \
strcat(path, gBasePath); \
Os_SetCurrentDir(path)

#define RESTORE_PATH()   Os_SetCurrentDir(existCurDir)

TEST_CASE("asakusa", "[tinyexr]") {
  SET_PATH();

  TinyExr_EXRVersion exr_version;
  TinyExr_EXRHeader exr_header;
  VFile::ScopedFile file = VFile::File::FromFile("asakusa.exr", Os_FM_ReadBinary);
  REQUIRE(file);

  int ret = TinyExr_ParseEXRVersion(&exr_version, file);
  REQUIRE(TINYEXR_SUCCESS == ret);
  file->Seek(0, VFile_SD_Begin);
  ret = TinyExr_ParseEXRHeader(&exr_header, &exr_version, file);
  REQUIRE(TINYEXR_SUCCESS == ret);

  RESTORE_PATH();
}

static void TestExr(char const *filename, bool tiled = false) {
  TinyExr_EXRVersion exr_version;
  LOGINFOF("Loading %s", filename);
  VFile::ScopedFile file = VFile::File::FromFile(filename, Os_FM_ReadBinary);
  REQUIRE(file);

  int ret = TinyExr_ParseEXRVersion(&exr_version, file);
  REQUIRE(TINYEXR_SUCCESS == ret);
  REQUIRE(tiled == (bool) exr_version.tiled);
  REQUIRE(false == (bool) exr_version.non_image);
  REQUIRE(false == (bool) exr_version.multipart);

  TinyExr_EXRHeader header;
  TinyExr_EXRImage image;
  TinyExr_InitEXRHeader(&header);
  TinyExr_InitEXRImage(&image);

  file->Seek(0, VFile_SD_Begin);
  ret = TinyExr_ParseEXRHeader(&header, &exr_version, file);
  REQUIRE(TINYEXR_SUCCESS == ret);

  file->Seek(0, VFile_SD_Begin);
  ret = TinyExr_LoadEXRImage(&image, &header, file);
  REQUIRE(TINYEXR_SUCCESS == ret);

  TinyExr_FreeEXRHeader(&header);
  TinyExr_FreeEXRImage(&image);
}

TEST_CASE("ScanLines", "[Load]") {
  SET_PATH();
  char const *inputs[] = {
      "ScanLines/Blobbies.exr",
      "ScanLines/CandleGlass.exr",
      // "ScanLines/Cannon.exr", // Cannon.exr will fail since it
      // uses b44 compression which is not yet supported on TinyEXR.
      "ScanLines/Desk.exr",
      "ScanLines/MtTamWest.exr",
      "ScanLines/PrismsLenses.exr",
      "ScanLines/StillLife.exr",
      "ScanLines/Tree.exr",
  };
  for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    TestExr(inputs[i]);
  }

  RESTORE_PATH();
}

TEST_CASE("Chromaticities", "[Load]") {
  SET_PATH();

  char const* inputs[] = {
      "Chromaticities/Rec709.exr",
      "Chromaticities/Rec709_YC.exr",
      "Chromaticities/XYZ.exr",
      "Chromaticities/XYZ_YC.exr",
  };
  for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    TestExr(inputs[i]);
  }
  RESTORE_PATH();
}

TEST_CASE("TestImages", "[Load]") {
  SET_PATH();

  char const* inputs[] = {
      "TestImages/AllHalfValues.exr",
      "TestImages/BrightRings.exr",
      "TestImages/BrightRingsNanInf.exr",
      // inputs.push_back("TestImages/GammaChart.exr"); // disable since this uses
      // pxr24 compression
      // inputs.push_back("TestImages/GrayRampsDiagonal.exr"); // pxr24
      // inputs.push_back("TestImages/GrayRampsHorizontal.exr"); // pxr24
      // inputs.push_back("TestImages/RgbRampsDiagonal.exr"); // pxr24
      // inputs.push_back("TestImages/SquaresSwirls.exr"); // pxr24
      "TestImages/WideColorGamut.exr",
      // inputs.push_back("TestImages/WideFloatRange.exr"); // pxr24
  };

  for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    TestExr(inputs[i]);
  }
  RESTORE_PATH();
}

TEST_CASE("LuminanceChroma", "[Load]") {
  SET_PATH();

  char const* inputs[] = {
      // "LuminanceChroma/CrissyField.exr", // b44
      // "LuminanceChroma/Flowers.exr", // b44
      // "LuminanceChroma/Garden.exr", // tiled
      "LuminanceChroma/MtTamNorth.exr",
      "LuminanceChroma/StarField.exr",
  };

  for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    TestExr(inputs[i]);
  }
  RESTORE_PATH();

}

TEST_CASE("DisplayWindow", "[Load]") {
  SET_PATH();

  char const* inputs[] = {
      "DisplayWindow/t01.exr",
      "DisplayWindow/t02.exr",
      "DisplayWindow/t03.exr",
      "DisplayWindow/t04.exr",
      "DisplayWindow/t05.exr",
      "DisplayWindow/t06.exr",
      "DisplayWindow/t07.exr",
      "DisplayWindow/t08.exr",
      "DisplayWindow/t09.exr",
      "DisplayWindow/t10.exr",
      "DisplayWindow/t11.exr",
      "DisplayWindow/t12.exr",
      "DisplayWindow/t13.exr",
      "DisplayWindow/t14.exr",
      "DisplayWindow/t15.exr",
      "DisplayWindow/t16.exr",
  };

  for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    TestExr(inputs[i]);
  }
  RESTORE_PATH();

}

TEST_CASE("Tiles/GoldenGate.exr", "[Version]") {
  SET_PATH();
  TestExr("Tiles/GoldenGate.exr", true);
  RESTORE_PATH();
}

TEST_CASE("LuminanceChroma/Garden.exr|Load", "[Load]") {
  SET_PATH();
  TestExr("LuminanceChroma/Garden.exr", true);
  RESTORE_PATH();
}

TEST_CASE("Tiles/Ocean.exr", "[Load]") {
  SET_PATH();
  TestExr("Tiles/Ocean.exr", true);
  RESTORE_PATH();
}