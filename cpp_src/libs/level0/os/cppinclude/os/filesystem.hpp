#pragma once
#ifndef WYRD_OS_FILESYSTEM_HPP
#define WYRD_OS_FILESYSTEM_HPP

#include "os/filesystem.h"
#include "tinystl/string.h"
namespace FileSystem {

typedef void (*FileDialogCallbackFn)(tinystl::string url, void *userData);

bool SplitPath(tinystl::string const& fullPath, tinystl::string_view& fileName,
               tinystl::string_view& extension);

inline tinystl::string GetInternalPath(tinystl::string const& path) {
  char tmp[2048];
  if (FS_GetInternalPath(path.c_str(), tmp, 2048)) {
    return tinystl::string(tmp);
  } else {
    return tinystl::string();
  }
}

inline tinystl::string GetPlatformPath(tinystl::string const& path) {
  char tmp[2048];
  if (FS_GetPlatformPath(path.c_str(), tmp, 2048)) {
    return tinystl::string(tmp);
  } else {
    return tinystl::string();
  }
}

inline tinystl::string GetCurrentDir() {
  char cwd[2048];
  if (FS_GetCurrentDir(cwd, 2048)) {
    return tinystl::string(cwd);
  } else {
    return tinystl::string();
  }
}

inline tinystl::string GetExtension(tinystl::string const& path) {
  tinystl::string_view fileName;
  tinystl::string_view extension;
  bool splitOk = FileSystem::SplitPath(path, fileName, extension);
  if (splitOk) { return extension; }
  else { return {}; }
}

} // namespace FileSystem

#endif // WYRD_FILESYSTEM_HPP
