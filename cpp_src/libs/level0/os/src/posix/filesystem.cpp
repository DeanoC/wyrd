#include "core/core.h"
#include "os/filesystem.hpp"
#include "tinystl/vector.h"

#include <unistd.h>       // getcwd
#include <errno.h>        // errno
#include <sys/stat.h>     // stat
#include <stdio.h>        // remove

// internal and platform path are the same on posix
EXTERN_C bool Os_IsInternalPath(char const *path) {
  return true;
}

EXTERN_C bool Os_GetInternalPath(char const *path, char *pathOut, size_t maxSize) {
  // just copy
  if (strlen(path) >= maxSize) { return false; }
  strcpy(pathOut, path);
  return true;
}

EXTERN_C bool Os_GetPlatformPath(char const *path, char *pathOut, size_t maxSize) {
  // just copy
  if (strlen(path) >= maxSize) { return false; }
  strcpy(pathOut, path);
  return true;
}

EXTERN_C size_t Os_GetLastModifiedTime(const char *fileName) {
  struct stat fileInfo;

  if (!stat(fileName, &fileInfo)) {
    return (size_t) fileInfo.st_mtime;
  } else {
    // return an impossible large mod time as the file doesn't exist
    return ~0;
  }
}

EXTERN_C bool Os_GetCurrentDir(char *dirOut, size_t maxSize) {
  char buffer[maxSize];
  if (getcwd(buffer, maxSize) == NULL) { return false; }
  if (Os_GetInternalPath(buffer, dirOut, maxSize) == false) { return false; }

  size_t len = strlen(dirOut);
  if (dirOut[len] != '/') {
    if (len + 1 >= maxSize) { return false; }
    dirOut[len] = '/';
    dirOut[len + 1] = 0;
  }
  return true;

}

EXTERN_C bool Os_SetCurrentDir(char const *path) {
  return chdir(path) == 0;
}

EXTERN_C bool Os_FileExists(char const *path) {
  struct stat st;
  int result = stat(path, &st);
  if (result != 0) { return false; }

  return !(st.st_mode & S_IFDIR);
}

EXTERN_C bool Os_DirExists(char const *path) {
  struct stat st;
  int result = stat(path, &st);
  if (result != 0) { return false; }

  return (st.st_mode & S_IFDIR);
}

bool Os_FileDelete(char const *fileName) {
  char buffer[2048];

  if (Os_IsInternalPath(fileName)) {
    strcpy(buffer, fileName);
  } else {
    bool platformOk = Os_GetPlatformPath(fileName, buffer, sizeof(buffer));
    if (platformOk == false) { return false; }
  }

#ifdef _WIN32
  return DeleteFileA(GetNativePath(fileName).c_str()) != 0;
#else
  return remove(buffer) == 0;
#endif
}

bool Os_CreateDir(char const *pathName) {
  using namespace Os::FileSystem;

  // Create each of the parents if necessary
  tinystl::string parentPath = GetParentPath(pathName);
  if ((uint32_t) parentPath.size() > 1 && !DirExists(parentPath)) {
    if (!Os_CreateDir(parentPath.c_str())) {
      return false;
    }
  }

#ifdef _WIN32
  bool success = (CreateDirectoryA(RemoveTrailingSlash(pathName).c_str(), NULL) == TRUE) || (GetLastError() == ERROR_ALREADY_EXISTS);
#else
  bool success = mkdir(GetPlatformPath(pathName).c_str(), S_IRWXU) == 0 || errno == EEXIST;
#endif

  return success;
}

int Os_SystemRun(char const *fileName, int argc, const char **argv) {
  tinystl::string fixedFileName = Os::FileSystem::GetPlatformPath(fileName);

#ifdef _DURANGO
  ASSERT(!"UNIMPLEMENTED");
    return -1;

#elif defined(_WIN32)
  // Add .exe extension if no extension defined
    if (GetExtension(fixedFileName).size() == 0)
        fixedFileName += ".exe";

    tinystl::string commandLine = "\"" + fixedFileName + "\"";
    for (unsigned i = 0; i < (unsigned)arguments.size(); ++i)
        commandLine += " " + arguments[i];

    HANDLE stdOut = NULL;
    if (stdOutFile != "")
    {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        stdOut = CreateFileA(stdOutFile, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    STARTUPINFOA        startupInfo;
    PROCESS_INFORMATION processInfo;
    memset(&startupInfo, 0, sizeof startupInfo);
    memset(&processInfo, 0, sizeof processInfo);
    startupInfo.cb = sizeof(STARTUPINFO);
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;
    startupInfo.hStdOutput = stdOut;
    startupInfo.hStdError = stdOut;

    if (!CreateProcessA(
            NULL, (LPSTR)commandLine.c_str(), NULL, NULL, stdOut ? TRUE : FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo))
        return -1;

    WaitForSingleObject(processInfo.hProcess, INFINITE);
    DWORD exitCode;
    GetExitCodeProcess(processInfo.hProcess, &exitCode);

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    if (stdOut)
    {
        CloseHandle(stdOut);
    }

    return exitCode;
#elif defined(__linux__)
  tinystl::vector<const char*> argPtrs;
    tinystl::string              cmd(fixedFileName.c_str());
    char                         space = ' ';
    cmd.append(&space, &space + 1);
    for (unsigned i = 0; i < (unsigned)arguments.size(); ++i)
    {
        cmd.append(arguments[i].begin(), arguments[i].end());
    }

    int res = system(cmd.c_str());
    return res;
#else
  pid_t pid = fork();
  if (!pid) {
    tinystl::vector<const char *> argPtrs;
    argPtrs.push_back(fixedFileName.c_str());
    for (unsigned i = 0; i < (unsigned) argc; ++i) {
      argPtrs.push_back(argv[i]);
    }
    argPtrs.push_back(NULL);

    execvp(argPtrs[0], (char **) &argPtrs[0]);
    return -1;    // Return -1 if we could not spawn the process
  } else if (pid > 0) {
    int exitCode = EINTR;
    while (exitCode == EINTR) {
      wait(&exitCode);
    }
    return exitCode;
  } else {
    return -1;
  }
#endif
}
