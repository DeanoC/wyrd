#include "core/core.h"
#include "core/logger.h"
#include "catch/catch.hpp"
#include "os/filesystem.h"
#include "tinystl/string.h"

TEST_CASE("Path (platform/internal) (C)", "[OS FileSystem]") {
  char const testFileInternalPath[] = "test_data/test.txt";
  char pathOut[2048];

  bool intPathOk = Os_GetInternalPath(testFileInternalPath, pathOut, 2048);
  REQUIRE(intPathOk);
  REQUIRE(strcmp(testFileInternalPath, pathOut) == 0);

  REQUIRE(Os_IsInternalPath(pathOut));

#if PLATFORM == PLATFORM_WINDOWS
  bool platPathOk = Os_GetPlatformPath(testFileInternalPath, pathOut, 2048);
  REQUIRE(platPathOk);
  REQUIRE(strcmp(testFileInternalPath, pathOut) != 0);

  REQUIRE(!Os_IsInternalPath(pathOut));
#else
  bool platPathOk = Os_GetPlatformPath(testFileInternalPath, pathOut, 2048);
  REQUIRE(platPathOk);
  REQUIRE(strcmp(testFileInternalPath, pathOut) == 0);

  REQUIRE(Os_IsInternalPath(testFileInternalPath));

#endif

}

TEST_CASE("SplitPath (C)", "[OS FileSystem]") {
  char const testFilePath[] = "test_data/test.txt";
  size_t fileNamePos = FS_npos;
  size_t extensionPos = FS_npos;
  bool splitOk = Os_SplitPath(testFilePath, &fileNamePos, &extensionPos);
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
  bool replace0Ok = Os_ReplaceExtension(testFilePath, extReplace0, buffer, sizeof(buffer));
  REQUIRE(replace0Ok);
  REQUIRE(strcmp("test_data/test.tmp", buffer) == 0);
  bool replace1Ok = Os_ReplaceExtension(testFilePath, extReplace1, buffer, sizeof(buffer));
  REQUIRE(replace1Ok);
  REQUIRE(strcmp("test_data/test.longer_ext", buffer) == 0);
  bool replace2Ok = Os_ReplaceExtension(buffer, "txt", buffer, sizeof(buffer));
  REQUIRE(replace2Ok);
  REQUIRE(strcmp(testFilePath, buffer) == 0);

  bool replace3Ok = Os_ReplaceExtension(test1FilePath, extReplace0, buffer, sizeof(buffer));
  REQUIRE(replace3Ok);
  REQUIRE(strcmp("test_data/test.tmp", buffer) == 0);

  bool replace4Ok = Os_ReplaceExtension(test2FilePath, extReplace0, buffer, sizeof(buffer));
  REQUIRE(replace4Ok);
  REQUIRE(strcmp("test_data/test.tmp", buffer) == 0);

}

TEST_CASE("GetParentPath (C)", "[OS FileSystem]") {
  char const testFilePath[] = "bob/test_data/test.txt";
  char const testDir0Path[] = "bob/test_data/";
  char const testDir1Path[] = "bob/test_data";

  char buffer[1024];
  bool const parentOk = Os_GetParentPath(testFilePath, buffer, sizeof(buffer));
  REQUIRE(parentOk);
  REQUIRE(strcmp("bob/test_data/", buffer) == 0);

  bool const parent0Ok = Os_GetParentPath(testDir0Path, buffer, sizeof(buffer));
  REQUIRE(parent0Ok);
  REQUIRE(strcmp("bob/", buffer) == 0);

  bool const parent1Ok = Os_GetParentPath(testDir1Path, buffer, sizeof(buffer));
  REQUIRE(parent1Ok);
  REQUIRE(strcmp("bob/", buffer) == 0);

}

TEST_CASE("Os_GetCurrentDir (C)", "[OS FileSystem]") {

  char buffer[1024];
  bool const getOk = Os_GetCurrentDir(buffer, sizeof(buffer));
  REQUIRE(getOk);

  tinystl::string path(buffer);
  if (path.back() == '/') {
    path.resize(path.size() - 1);
  }

  size_t const curdirPos = path.find_last('/');
  REQUIRE(curdirPos != tinystl::string::npos);
  LOGWARNING(buffer);
  REQUIRE(strcmp("live/", buffer + curdirPos + 1) == 0);

}

TEST_CASE("Os_SetCurrentDir (C)", "[OS FileSystem]") {

  char buffer[1024];
  bool const getOk = Os_GetCurrentDir(buffer, sizeof(buffer));
  REQUIRE(getOk);

  char buffer2[1024];
  bool const parentOk = Os_GetParentPath(buffer, buffer2, sizeof(buffer2));
  REQUIRE(parentOk);
  bool const setOk = Os_SetCurrentDir(buffer2);
  REQUIRE(setOk);

  char buffer3[1024];
  bool const getOk3 = Os_GetCurrentDir(buffer3, sizeof(buffer3));
  REQUIRE(getOk3);

  REQUIRE(strcmp(buffer3, buffer2) == 0);

  // reset
  bool const setOk1 = Os_SetCurrentDir(buffer);
  REQUIRE(setOk1);

}

TEST_CASE("File / Dir Exists (C)", "[OS FileSystem]") {

  char const testFilePath0[] = "test_data/test.txt";
  char const testFilePath1[] = "test_data/DOES_NOT_EXIST";
  char const testDirPath0[] = "test_data/";
  char const testDirPath1[] = "test_data/DOES_NOT_EXIST/";

  bool const fileOk = Os_FileExists(testFilePath0);
  REQUIRE(fileOk);
  bool const fileOk1 = Os_FileExists(testFilePath1);
  REQUIRE(fileOk1 == false);
  bool const fileOk2 = Os_FileExists(testDirPath1);
  REQUIRE(fileOk2 == false);

  bool const dirOk = Os_DirExists(testDirPath0);
  REQUIRE(dirOk);
  bool const dirOk1 = Os_DirExists(testDirPath1);
  REQUIRE(dirOk1 == false);
  bool const dirOk2 = Os_DirExists(testFilePath1);
  REQUIRE(dirOk2 == false);

}

TEST_CASE("File Copy (C)", "[OS FileSystem]") {
  char const testFilePath0[] = "test_data/test.txt";
  char const testFilePath1[] = "test_data/testcopy.txt";

  bool const copyOk = Os_FileCopy(testFilePath0, testFilePath1);
  REQUIRE(copyOk);
  bool const existOk = Os_FileExists(testFilePath1);
  REQUIRE(existOk);

}

TEST_CASE("File Delete (C)", "[OS  FileSystem]") {
  char const testFilePath1[] = "test_data/testcopy.txt";
  bool const exists = Os_FileExists(testFilePath1);
  if (!exists) {
    char const testFilePath0[] = "test_data/test.txt";
    bool const copyOk = Os_FileCopy(testFilePath0, testFilePath1);
    REQUIRE(copyOk);
  }
  bool const deleteOk = Os_FileDelete(testFilePath1);
  REQUIRE(deleteOk);
  bool const existOk = Os_FileExists(testFilePath1);
  REQUIRE(existOk == false);
}

TEST_CASE("GetExePath (C)", "[OS  FileSystem]") {

  char buffer[2048];
  bool okay = Os_GetExePath(buffer, sizeof(buffer));
  REQUIRE(okay);
  // complex to do more tests... need to think
}

TEST_CASE("GetUserDocumentsDir (C)", "[OS  FileSystem]") {

  char buffer[2048];
  bool okay = Os_GetUserDocumentsDir(buffer, sizeof(buffer));
  REQUIRE(okay);
  // complex to do more tests... need to think
}

TEST_CASE("GetAppPrefsDir (C)", "[OS  FileSystem]") {

  char buffer[2048];
  bool okay = Os_GetAppPrefsDir("test", "test", buffer, sizeof(buffer));
  REQUIRE(okay);
  // complex to do more tests... need to think
}

TEST_CASE("Os_GetLastModifiedTime (C)", "[OS  FileSystem]") {

  char const testFilePath0[] = "test_data/test.txt";
  bool okay = Os_GetLastModifiedTime(testFilePath0);
  REQUIRE(okay);
  // complex to do more tests... need to think
}
