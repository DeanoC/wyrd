
#include "core/core.h"
#include "core/logger.h"
#include "os/filesystem.h"
#include "tinystl/string.h"

EXTERN_C bool FS_SplitPath(char const *internalPath, size_t *fileName, size_t *extension) {
  ASSERT(FS_IsInternalPath(internalPath));
  ASSERT(internalPath != nullptr);

  tinystl::string path(internalPath);
  if (path.empty()) { return false; }

  if (fileName != nullptr) {
    tinystl::string::size_type pathPos = path.find_last('/');
    *fileName = pathPos + 1;
  }
  if (extension != nullptr) {
    tinystl::string::size_type extPos = path.find_last('.');
    *extension = extPos + 1;
  }

  return true;
}

EXTERN_C bool FS_ReplaceExtension(char const *path, char const *newExtension, char *dirOut, int maxSize) {
  tinystl::string_view fileNameView;
  tinystl::string_view extView;
  ASSERT(path);
  ASSERT(newExtension);
  ASSERT(dirOut);

  tinystl::string const p(path);
  tinystl::string const e(newExtension);
  tinystl::string::size_type const extPos = p.find_last('.');
  if (extPos == tinystl::string::npos) {
    tinystl::string n(p);

    if (p.back() != '.') { n.append('.'); }
    n += e;
    if (n.size() > maxSize) { return false; }
    else {
      strcpy(dirOut, n.c_str());
    }
  } else {
    tinystl::string n(p.c_str(), extPos + 1);

    n += e;
    if (n.size() > maxSize) { return false; }
    else {
      strcpy(dirOut, n.c_str());
    }
  }

  return true;
}

EXTERN_C bool FS_GetParentPath(char const *pathc, char *dirOut, int maxSize) {

  tinystl::string path(pathc);

  if (path.back() == '/') {
    path.resize(path.size() - 1);
  }

  size_t lastSlash = path.find_last('/');
  tinystl::string n(path.c_str(), lastSlash + 1);

  if (n.size() > maxSize) { return false; }
  else {
    strcpy(dirOut, n.c_str());
    return true;
  }
}

namespace FileSystem {

bool SplitPath(tinystl::string const& fullPath,
               tinystl::string_view& fileName,
               tinystl::string_view& extension) {

  size_t filePos = tinystl::string::npos;
  size_t extPos = tinystl::string::npos;

  bool splitOk = FS_SplitPath(fullPath.c_str(), &filePos, &extPos);
  if (splitOk == false) {
    fileName = {};
    extension = {};
    return false;
  }
  size_t const pathSize = fullPath.size();

  if (filePos == tinystl::string::npos) {
    filePos = 0;
  }

  if (extPos == tinystl::string::npos) {
    filePos = pathSize;
  }

  char const *fpcp = fullPath.c_str();
  fileName = tinystl::string_view(fpcp + filePos, extPos - filePos);
  extension = tinystl::string_view(fpcp + extPos, pathSize);

  return true;
}

} // end namespace