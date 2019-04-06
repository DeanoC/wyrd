#include "core/core.h"
#include "catch/catch.hpp"
#include "image/image.h"
#include "image/format_cracker.h"
#include "image/io.h"
#include "os/filesystem.h"
#include "vfile/vfile.hpp"
#include "syoyo/tiny_exr.h"

// path to https://github.com/openexr/openexr-images
static const char *gBasePath = "test_data/exr/";

#define SET_EXR_PATH()   char existCurDir[1024]; \
Os_GetCurrentDir(existCurDir, sizeof(existCurDir)); \
char path[2048]; \
strcpy(path, existCurDir); \
strcat(path, gBasePath); \
Os_SetCurrentDir(path)

#define RESTORE_EXR_PATH()   Os_SetCurrentDir(existCurDir)

static void TestExr(char const *filename, bool tiled = false) {
  TinyExr_EXRVersion exr_version;
  LOGINFOF("Loading %s", filename);
  VFile::ScopedFile file = VFile::File::FromFile(filename, Os_FM_ReadBinary);
  REQUIRE(file);

  Image_ImageHeader* image = Image_LoadEXR(file);
  REQUIRE(image != nullptr);

  file->Seek(0, VFile_SD_Begin);
  int ret = TinyExr_ParseEXRVersion(&exr_version, file);
  REQUIRE(TINYEXR_SUCCESS == ret);
  REQUIRE(tiled == exr_version.tiled);
  REQUIRE(false == exr_version.non_image);
  REQUIRE(false == exr_version.multipart);

  TinyExr_EXRHeader header;
  file->Seek(0, VFile_SD_Begin);
  ret = TinyExr_ParseEXRHeader(&header, &exr_version, file);
  REQUIRE(TINYEXR_SUCCESS == ret);
  TinyExr_EXRImage exrImage;
  TinyExr_InitEXRImage(&exrImage);
  ret = TinyExr_LoadEXRImage(&exrImage, &header, file);
  REQUIRE(TINYEXR_SUCCESS == ret);
  REQUIRE(exrImage.width == image->width);
  REQUIRE(exrImage.height == image->height);

  // clean up
  TinyExr_FreeEXRImage(&exrImage);
  TinyExr_FreeEXRHeader(&header);
  Image_Destroy(image);
}

TEST_CASE("Image io EXR Scanlines (C)", "[Image]") {
  SET_EXR_PATH();

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

  RESTORE_EXR_PATH();
}