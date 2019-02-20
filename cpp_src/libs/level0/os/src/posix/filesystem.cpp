#include "core/core.h"
#include "os/filesystem.hpp"
#include "os/os.h"
#include <sys/stat.h>     // for stat

// internal and platform path are the same on posix
EXTERN_C bool FS_IsInternalPath(char const *path) {
  return true;
}

EXTERN_C bool FS_GetInternalPath(char const *path, char *pathOut, int maxSize) {
  // just copy
  if (strlen(path) >= maxSize) { return false; }
  strcpy(pathOut, path);
  return true;
}

EXTERN_C bool FS_GetPlatformPath(char const *path, char *pathOut, int maxSize) {
  // just copy
  if (strlen(path) >= maxSize) { return false; }
  strcpy(pathOut, path);
  return true;
}

EXTERN_C size_t FS_GetLastModifiedTime(const char *_fileName) {
  struct stat fileInfo;

  if (!stat(_fileName, &fileInfo)) {
    return (size_t) fileInfo.st_mtime;
  } else {
    // return an impossible large mod time as the file doesn't exist
    return ~0;
  }
}

EXTERN_C bool FS_GetCurrentDir(char *dirOut, int maxSize) {
  char buffer[maxSize];
  if (getcwd(buffer, maxSize) == NULL) { return false; }
  if (FS_GetInternalPath(buffer, dirOut, maxSize) == false) { return false; }

  size_t len = strlen(dirOut);
  if (dirOut[len] != '/') {
    if (len + 1 >= maxSize) { return false; }
    dirOut[len] = '/';
    dirOut[len + 1] = 0;
  }
  return true;

}

EXTERN_C bool FS_SetCurrentDir(char const *path) {
  return chdir(path) == 0;
}

EXTERN_C bool FS_FileExists(char const *path) {
  struct stat st;
  int result = stat(path, &st);
  if (result != 0) { return false; }

  return !(st.st_mode & S_IFDIR);
}

EXTERN_C bool FS_DirExists(char const *path) {
  struct stat st;
  int result = stat(path, &st);
  if (result != 0) { return false; }

  return (st.st_mode & S_IFDIR);
}
