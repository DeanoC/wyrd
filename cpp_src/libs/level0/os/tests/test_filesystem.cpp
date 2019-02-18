#include "core/core.h"
#include "catch/catch.hpp"
#include "os/filesystem.h"

TEST_CASE("Path (platform/internal) (C)", "[OS FileSystem]") {
  char const testFileInternalPath[] = "test_data/test.txt";
  char pathOut[2048];

  bool intPathOk = FS_GetInternalPath(testFileInternalPath, pathOut, 2048);
  REQUIRE(intPathOk);
  REQUIRE(strcmp(testFileInternalPath, pathOut) == 0);

  REQUIRE(FS_IsInternalPath(testFileInternalPath));

#if PLATFORM == PLATFORM_WINDOWS
  bool platPathOk = FS_GetPlatformPath(testFileInternalPath, pathOut, 2048);
  REQUIRE(platPathOk);
  REQUIRE(strcmp(testFileInternalPath, pathOut) != 0);

  REQUIRE( FS_IsInternalPath(testFileInternalPath) == false);
#else
  bool platPathOk = FS_GetPlatformPath(testFileInternalPath, pathOut, 2048);
  REQUIRE(platPathOk);
  REQUIRE(strcmp(testFileInternalPath, pathOut) == 0);

  REQUIRE(FS_IsInternalPath(testFileInternalPath));

#endif

}

TEST_CASE("SplitPath (C)", "[OS FileSystem]") {
  char const testFilePath[] = "test_data/test.txt";
  size_t fileNamePos = FS_npos;
  size_t extensionPos = FS_npos;
  bool splitOk = FS_SplitPath(testFilePath, &fileNamePos, &extensionPos);
  REQUIRE(splitOk);
  REQUIRE(fileNamePos == 10);
  REQUIRE(strcmp(&testFilePath[fileNamePos], "test.txt") == 0);
  REQUIRE(extensionPos == 15);
  REQUIRE(strcmp(&testFilePath[extensionPos], "txt") == 0);

  char pathOnly[1024];
  strncpy(pathOnly, testFilePath, fileNamePos);
  pathOnly[fileNamePos] = 0;
  REQUIRE(strcmp(pathOnly, "test_data/") == 0);

}

TEST_CASE("ReplaceExtension (C)", "[OS FileSystem]") {
  char const testFilePath[] = "test_data/test.txt";
  char const test1FilePath[] = "test_data/test";
  char const test2FilePath[] = "test_data/test.";
  char const extReplace0[] = "tmp";
  char const extReplace1[] = "longer_ext";

  char buffer[1024];
  bool replace0Ok = FS_ReplaceExtension(testFilePath, extReplace0, buffer, sizeof(buffer));
  REQUIRE(replace0Ok);
  REQUIRE(strcmp("test_data/test.tmp", buffer) == 0);
  bool replace1Ok = FS_ReplaceExtension(testFilePath, extReplace1, buffer, sizeof(buffer));
  REQUIRE(replace1Ok);
  REQUIRE(strcmp("test_data/test.longer_ext", buffer) == 0);
  bool replace2Ok = FS_ReplaceExtension(buffer, "txt", buffer, sizeof(buffer));
  REQUIRE(replace2Ok);
  REQUIRE(strcmp(testFilePath, buffer) == 0);

  bool replace3Ok = FS_ReplaceExtension(test1FilePath, extReplace0, buffer, sizeof(buffer));
  REQUIRE(replace3Ok);
  REQUIRE(strcmp("test_data/test.tmp", buffer) == 0);

  bool replace4Ok = FS_ReplaceExtension(test2FilePath, extReplace0, buffer, sizeof(buffer));
  REQUIRE(replace4Ok);
  REQUIRE(strcmp("test_data/test.tmp", buffer) == 0);

}