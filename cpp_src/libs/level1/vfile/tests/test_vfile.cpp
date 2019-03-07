#include "core/core.h"
#include "cmdlineshell/cmdlineshell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int Main(int argc, char *argv[]) {
  return Catch::Session().run(argc, argv);
}

#include "vfile/vfile.h"

TEST_CASE("Open and close (C)", "[VFile OsFile]") {
  Os_FileHandle ofh = Os_FileOpen("test_data/test.txt", Os_FM_Read);
  REQUIRE(ofh != NULL);
  VFile_Handle vfh = VFile_FromOsFile(ofh);
  REQUIRE(vfh);
  VFile_Close(vfh);
}

TEST_CASE("Read Testing 1, 2, 3 text file OsFile (C)", "[VFile]") {

  Os_FileHandle ofh = Os_FileOpen("test_data/test.txt", Os_FM_Read);
  REQUIRE(ofh != NULL);
  VFile_Handle vfh = VFile_FromOsFile(ofh);
  REQUIRE(vfh);

  static char expectedBytes[] = "Testing 1, 2, 3";
  char buffer[1024];
  size_t bytesRead = VFile_Read(vfh, buffer, 1024);
  REQUIRE(bytesRead == strlen(expectedBytes));
  REQUIRE(strcmp(expectedBytes, buffer) == 0);

  VFile_Close(vfh);
}

TEST_CASE("Write Testing 1, 2, 3 text file OsFile (C)", "[VFile]") {
  Os_FileHandle ofh = Os_FileOpen("test_data/test.txt", Os_FM_Write);
  REQUIRE(ofh != NULL);
  VFile_Handle vfh = VFile_FromOsFile(ofh);
  REQUIRE(vfh);

  static char expectedBytes[] = "Testing 1, 2, 3";

  size_t bytesWritten = VFile_Write(vfh, expectedBytes, strlen(expectedBytes));
  REQUIRE(bytesWritten == strlen(expectedBytes));

  // there not really an easy way of testing flush so we test it doesn't crash
  VFile_Flush(vfh);
  VFile_Close(vfh);

  // verify write
  Os_FileHandle fhr = Os_FileOpen("test_data/test.txt", Os_FM_Read);
  REQUIRE(fhr != NULL);
  VFile_Handle vfhr = VFile_FromOsFile(fhr);
  REQUIRE(vfhr);
  char buffer[1024];
  size_t bytesRead = VFile_Read(vfhr, buffer, 1024);
  REQUIRE(bytesRead == strlen(expectedBytes));
  REQUIRE(strcmp(expectedBytes, buffer) == 0);

  VFile_Close(vfhr);
}

TEST_CASE("Seek & Tell Testing 1, 2, 3 text file OsFile (C)", "[VFile]") {

  Os_FileHandle ofh = Os_FileOpen("test_data/test.txt", Os_FM_Read);
  REQUIRE(ofh != NULL);
  VFile_Handle vfh = VFile_FromOsFile(ofh);
  REQUIRE(vfh);

  static char expectedBytes[] = "Testing 1, 2, 3";
  char buffer[1024];
  size_t totalLen = strlen(expectedBytes);

  bool seek0 = VFile_Seek(vfh, 4, VFile_SD_Begin);
  REQUIRE(seek0);
  REQUIRE(VFile_Tell(vfh) == 4);
  size_t bytesRead0 = VFile_Read(vfh, buffer, 1024);
  REQUIRE(bytesRead0 == strlen(&expectedBytes[4]));
  REQUIRE(VFile_Tell(vfh) == strlen(expectedBytes));

  VFile_Seek(vfh, 4, VFile_SD_Begin);
  bool seek1 = VFile_Seek(vfh, 4, VFile_SD_Current);
  REQUIRE(seek1);
  REQUIRE(VFile_Tell(vfh) == 8);
  size_t bytesRead1 = VFile_Read(vfh, buffer, 1024);
  REQUIRE(bytesRead1 == strlen(&expectedBytes[8]));
  REQUIRE(VFile_Tell(vfh) == strlen(expectedBytes));

  bool seek2 = VFile_Seek(vfh, -4, VFile_SD_End);
  REQUIRE(seek2);
  REQUIRE(VFile_Tell(vfh) == totalLen - 4);
  size_t bytesRead2 = VFile_Read(vfh, buffer, 1024);
  REQUIRE(bytesRead2 == strlen(&expectedBytes[totalLen - 4]));
  REQUIRE(VFile_Tell(vfh) == strlen(expectedBytes));

  VFile_Close(vfh);
}

TEST_CASE("Size OsFile (C)", "[VFile]") {

  Os_FileHandle ofh = Os_FileOpen("test_data/test.txt", Os_FM_Read);
  REQUIRE(ofh != NULL);
  VFile_Handle vfh = VFile_FromOsFile(ofh);
  REQUIRE(vfh);

  size_t size = VFile_Size(vfh);
  REQUIRE(size == 15);

  VFile_Close(vfh);
}

TEST_CASE("Open and close MemFile (C)", "[VFile]") {

  static char testData[] = "Testing 1, 2, 3";

  VFile_Handle vfh = VFile_FromMemory(testData, sizeof(testData), false);
  REQUIRE(vfh);
  VFile_Close(vfh);
}

TEST_CASE("Read Testing 1, 2, 3 text file (C)", "[VFile MemFile]") {

  static char testData[] = "Testing 1, 2, 3";

  VFile_Handle vfh = VFile_FromMemory(testData, sizeof(testData) - 1, false);
  REQUIRE(vfh);

  static char expectedBytes[] = "Testing 1, 2, 3";
  char buffer[1024];
  size_t bytesRead = VFile_Read(vfh, buffer, 1024);
  REQUIRE(bytesRead == strlen(expectedBytes));
  REQUIRE(strcmp(expectedBytes, buffer) == 0);

  VFile_Close(vfh);
}

TEST_CASE("Write Testing 1, 2, 3 text file MemFile (C)", "[VFile]") {
  static char testData[1024];
  VFile_Handle vfh = VFile_FromMemory(testData, sizeof(testData), false);
  REQUIRE(vfh);

  static char expectedBytes[] = "Testing 1, 2, 3";
  size_t bytesWritten = VFile_Write(vfh, expectedBytes, strlen(expectedBytes));
  REQUIRE(bytesWritten == strlen(expectedBytes));

  // there not really an easy way of testing flush so we test it doesn't crash
  VFile_Flush(vfh);
  VFile_Close(vfh);

  // verify write
  VFile_Handle vfhr = VFile_FromMemory(testData, bytesWritten, false);
  REQUIRE(vfhr);
  char buffer[1024];
  size_t bytesRead = VFile_Read(vfhr, buffer, 1024);
  REQUIRE(bytesRead == strlen(expectedBytes));
  REQUIRE(strcmp(expectedBytes, buffer) == 0);

  VFile_Close(vfhr);
}

TEST_CASE("Seek & Tell Testing 1, 2, 3 text file MemFile (C)", "[VFile]") {

  static char testData[] = "Testing 1, 2, 3";

  VFile_Handle vfh = VFile_FromMemory(testData, sizeof(testData) - 1, false);
  REQUIRE(vfh);

  static char expectedBytes[] = "Testing 1, 2, 3";
  char buffer[1024];
  size_t totalLen = strlen(expectedBytes);

  bool seek0 = VFile_Seek(vfh, 4, VFile_SD_Begin);
  REQUIRE(seek0);
  REQUIRE(VFile_Tell(vfh) == 4);
  size_t bytesRead0 = VFile_Read(vfh, buffer, 1024);
  REQUIRE(bytesRead0 == strlen(&expectedBytes[4]));
  REQUIRE(VFile_Tell(vfh) == strlen(expectedBytes));

  VFile_Seek(vfh, 4, VFile_SD_Begin);
  bool seek1 = VFile_Seek(vfh, 4, VFile_SD_Current);
  REQUIRE(seek1);
  REQUIRE(VFile_Tell(vfh) == 8);
  size_t bytesRead1 = VFile_Read(vfh, buffer, 1024);
  REQUIRE(bytesRead1 == strlen(&expectedBytes[8]));
  REQUIRE(VFile_Tell(vfh) == strlen(expectedBytes));

  bool seek2 = VFile_Seek(vfh, -4, VFile_SD_End);
  REQUIRE(seek2);
  REQUIRE(VFile_Tell(vfh) == totalLen - 4);
  size_t bytesRead2 = VFile_Read(vfh, buffer, 1024);
  REQUIRE(bytesRead2 == strlen(&expectedBytes[totalLen - 4]));
  REQUIRE(VFile_Tell(vfh) == strlen(expectedBytes));

  VFile_Close(vfh);
}

TEST_CASE("Size MemFile (C)", "[VFile]") {

  static char testData[] = "Testing 1, 2, 3";

  VFile_Handle vfh = VFile_FromMemory(testData, sizeof(testData) - 1, false);
  REQUIRE(vfh);

  size_t size = VFile_Size(vfh);
  REQUIRE(size == 15);

  VFile_Close(vfh);
}
