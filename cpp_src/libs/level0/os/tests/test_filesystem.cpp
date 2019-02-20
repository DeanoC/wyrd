#include <tinystl/string.h>
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

TEST_CASE("GetParentPath (C)", "[OS FileSystem]") {
  char const testFilePath[] = "bob/test_data/test.txt";
  char const testDir0Path[] = "bob/test_data/";
  char const testDir1Path[] = "bob/test_data";

  char buffer[1024];
  bool const parentOk = FS_GetParentPath(testFilePath, buffer, sizeof(buffer));
  REQUIRE(parentOk);
  REQUIRE(strcmp("bob/test_data/", buffer) == 0);

  bool const parent0Ok = FS_GetParentPath(testDir0Path, buffer, sizeof(buffer));
  REQUIRE(parent0Ok);
  REQUIRE(strcmp("bob/", buffer) == 0);

  bool const parent1Ok = FS_GetParentPath(testDir1Path, buffer, sizeof(buffer));
  REQUIRE(parent1Ok);
  REQUIRE(strcmp("bob/", buffer) == 0);

}

TEST_CASE("FS_GetCurrentDir (C)", "[OS FileSystem]") {

  char buffer[1024];
  bool const getOk = FS_GetCurrentDir(buffer, sizeof(buffer));
  REQUIRE(getOk);

  tinystl::string path(buffer);
  if (path.back() == '/') {
    path.resize(path.size() - 1);
  }

  size_t const curdirPos = path.find_last('/');
  REQUIRE(curdirPos != tinystl::string::npos);
  REQUIRE(strcmp("live/", buffer + curdirPos + 1) == 0);

}

TEST_CASE("FS_SetCurrentDir (C)", "[OS FileSystem]") {

  char buffer[1024];
  bool const getOk = FS_GetCurrentDir(buffer, sizeof(buffer));
  REQUIRE(getOk);

  char buffer2[1024];
  bool const parentOk = FS_GetParentPath(buffer, buffer2, sizeof(buffer2));
  REQUIRE(parentOk);
  bool const setOk = FS_SetCurrentDir(buffer2);
  REQUIRE(setOk);

  char buffer3[1024];
  bool const getOk3 = FS_GetCurrentDir(buffer3, sizeof(buffer3));
  REQUIRE(getOk3);

  REQUIRE(strcmp(buffer3, buffer2) == 0);

  // reset
  bool const setOk1 = FS_SetCurrentDir(buffer);
  REQUIRE(setOk1);

}

TEST_CASE("File / Dir Exists (C)", "[OS FileSystem]") {

  char const testFilePath0[] = "test_data/test.txt";
  char const testFilePath1[] = "test_data/DOES_NOT_EXIST";
  char const testDirPath0[] = "test_data/";
  char const testDirPath1[] = "test_data/DOES_NOT_EXIST/";

  bool const fileOk = FS_FileExists(testFilePath0);
  REQUIRE(fileOk);
  bool const fileOk1 = FS_FileExists(testFilePath1);
  REQUIRE(fileOk1 == false);
  bool const fileOk2 = FS_FileExists(testDirPath1);
  REQUIRE(fileOk2 == false);

  bool const dirOk = FS_DirExists(testDirPath0);
  REQUIRE(dirOk);
  bool const dirOk1 = FS_DirExists(testDirPath1);
  REQUIRE(dirOk1 == false);
  bool const dirOk2 = FS_DirExists(testFilePath1);
  REQUIRE(dirOk2 == false);

}