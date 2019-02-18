#include "core/core.h"
#include "catch/catch.hpp"
#include "os/file.h"

TEST_CASE("Open and close (C)", "[OS File]") {
  File_Handle fh = File_Open("test_data/test.txt", FM_Read);
  REQUIRE(fh != NULL);
  bool closeOk = File_Close(fh);
  REQUIRE(closeOk);
}

TEST_CASE("Read Testing 1, 2, 3 text file (C)", "[OS File]") {

  File_Handle fh = File_Open("test_data/test.txt", FM_Read);
  REQUIRE(fh != NULL);

  static char expectedBytes[] = "Testing 1, 2, 3";
  char buffer[1024];
  size_t bytesRead = File_Read(fh, buffer, 1024);
  REQUIRE(bytesRead == strlen(expectedBytes));
  REQUIRE(strcmp(expectedBytes, buffer) == 0);

  bool closeOk = File_Close(fh);
  REQUIRE(closeOk);
}

TEST_CASE("Write Testing 1, 2, 3 text file (C)", "[OS File]") {

  File_Handle fh = File_Open("test_data/test.txt", FM_Write);
  REQUIRE(fh != NULL);

  static char expectedBytes[] = "Testing 1, 2, 3";

  size_t bytesWritten = File_Write(fh, expectedBytes, strlen(expectedBytes));
  REQUIRE(bytesWritten == strlen(expectedBytes));

  // there not really an easy way of testing flush so we test it doesn't crash
  File_Flush(fh);

  bool closeWriteOk = File_Close(fh);
  REQUIRE(closeWriteOk);


  // verify write
  File_Handle fhr = File_Open("test_data/test.txt", FM_Read);
  REQUIRE(fhr != NULL);
  char buffer[1024];
  size_t bytesRead = File_Read(fhr, buffer, 1024);
  REQUIRE(bytesRead == strlen(expectedBytes));
  REQUIRE(strcmp(expectedBytes, buffer) == 0);

  bool closeReadOk = File_Close(fhr);
  REQUIRE(closeReadOk);
}

TEST_CASE("Seek & Tell Testing 1, 2, 3 text file (C)", "[OS File]") {

  File_Handle fh = File_Open("test_data/test.txt", FM_Read);
  REQUIRE(fh != NULL);

  static char expectedBytes[] = "Testing 1, 2, 3";
  char buffer[1024];
  char totalLen = strlen(expectedBytes);

  bool seek0 = File_Seek(fh, 4, FSD_BEGIN);
  REQUIRE(seek0);
  REQUIRE(File_Tell(fh) == 4);
  size_t bytesRead0 = File_Read(fh, buffer, 1024);
  REQUIRE(bytesRead0 == strlen(&expectedBytes[4]));
  REQUIRE(File_Tell(fh) == strlen(expectedBytes));

  File_Seek(fh, 4, FSD_BEGIN);
  bool seek1 = File_Seek(fh, 4, FSD_CUR);
  REQUIRE(seek1);
  REQUIRE(File_Tell(fh) == 8);
  size_t bytesRead1 = File_Read(fh, buffer, 1024);
  REQUIRE(bytesRead1 == strlen(&expectedBytes[8]));
  REQUIRE(File_Tell(fh) == strlen(expectedBytes));

  bool seek2 = File_Seek(fh, -4, FSD_END);
  REQUIRE(seek2);
  REQUIRE(File_Tell(fh) == totalLen - 4);
  size_t bytesRead2 = File_Read(fh, buffer, 1024);
  REQUIRE(bytesRead2 == strlen(&expectedBytes[totalLen - 4]));
  REQUIRE(File_Tell(fh) == strlen(expectedBytes));

  bool closeOk = File_Close(fh);
  REQUIRE(closeOk);
}